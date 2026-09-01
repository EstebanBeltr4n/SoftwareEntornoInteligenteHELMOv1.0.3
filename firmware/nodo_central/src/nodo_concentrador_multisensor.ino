/*
 * =========================================================================================
 * PROYECTO: SISTEMA MULTI-AGENTE DE MONITOREO HÍDRICO HELMO (PTAP)
 * MÓDULO: NODO CONCENTRADOR MULTISENSOR (NIVEL, TURBIDEZ, pH) - CAPA LoRaWAN - V3_ESTABLE
 * HARDWARE: Heltec WiFi LoRa 32 V3 (ESP32-S3) + HC-SR04 + sensor de turbidez + sensor de pH + IRF520
 * ARCHIVO: firmware/nodo_central/src/nodo_concentrador_multisensor_v3_estable.ino
 * VERSIÓN DOCUMENTAL: 1.0.0
 * FECHA BASE DE REFERENCIA: 22 de julio, versión estable
 * =========================================================================================
 *
 * DESCRIPCIÓN TÉCNICA
 * Este firmware implementa un nodo monolítico multisensor para el entorno HELMO.
 * Integra tres variables de proceso: nivel, turbidez y pH. El nodo energiza la etapa
 * de sensado mediante un MOSFET, realiza adquisición secuencial protegida, convierte
 * las magnitudes físicas a representación digital y empaqueta una carga útil de 6 bytes
 * en formato big-endian para transmisión LoRaWAN por OTAA.
 *
 * El diseño responde al papel de “unidad de proceso local” descrito en la arquitectura
 * HELMO, donde sensores de campo alimentan una capa de comunicación inalámbrica que se
 * conecta con TTN/TTS, backend, base de datos y visualización en Grafana.
 *
 * NOTA PARA REGISTRO DE DERECHOS DE AUTOR
 * Este archivo se entrega documentado como obra de software embebido y material fuente
 * principal del módulo concentrador multisensor del proyecto HELMO. Las credenciales
 * LoRaWAN han sido sustituidas por marcadores para evitar exposición de secretos.
 *
 * AUTORÍA
 * - Esteban Eduardo Escárraga Túquerres
 * - Johan Felipe Ordoñez Jenoy
 * - Dayerli Katerine Tamayo Solarte
 */

#include "LoRaWan_APP.h"
#include <Wire.h>
#include "HT_SSD1306Wire.h"

/* =========================================================================================
 * 1. DEFINICIÓN DE PINES (CAPA FÍSICA)
 * ========================================================================================= */
#define NODE_ID 6

// Sensor ultrasónico HC-SR04
#define TRIG_PIN 4
#define ECHO_PIN 5

// Canales analógicos
#define ADC_TURB_PIN 2
#define ADC_PH_PIN 1

// Control de potencia y OLED
#define MOSFET_PIN 6
#define SDA_OLED 17
#define SCL_OLED 18
#define RST_OLED 21

SSD1306Wire pantalla(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED);

/* =========================================================================================
 * 2. CONSTANTES DE CALIBRACIÓN Y CONVERSIÓN
 * ========================================================================================= */
#define DIVISOR_FACTOR 1.515

// Calibración pH por dos puntos (buffers pH 4.00 y pH 7.00)
#define PH4_BUFFER_VALUE 4.00
#define PH7_BUFFER_VALUE 7.00
#define PH4_VOLTAGE 2.560
#define PH7_VOLTAGE 2.250

// Recta de calibración: pH = PH_M * V + PH_B
#define PH_M ((PH7_BUFFER_VALUE - PH4_BUFFER_VALUE) / (PH7_VOLTAGE - PH4_VOLTAGE))
#define PH_B (PH7_BUFFER_VALUE - (PH_M * PH7_VOLTAGE))

/* =========================================================================================
 * 3. CREDENCIALES LORAWAN (OTAA)
 * =========================================================================================
 * Las credenciales reales se omiten deliberadamente en esta versión documental para evitar
 * exposición de secretos operativos. Deben sustituirse en despliegue real por los valores
 * registrados en el servidor LoRaWAN.
 */
uint8_t devEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// Variables ABP en desuso, conservadas por compatibilidad con la plantilla Heltec
uint8_t nwkSKey[] = { 0x00 };
uint8_t appSKey[] = { 0x00 };
uint32_t devAddr  = 0x00;

/* =========================================================================================
 * 4. PARÁMETROS LORAWAN (CAPA MAC)
 * ========================================================================================= */
uint16_t userChannelsMask[6] = { 0xFF00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };
LoRaMacRegion_t loraWanRegion = ACTIVE_REGION;
DeviceClass_t loraWanClass = CLASS_A;
uint32_t appTxDutyCycle = 60000;
bool overTheAirActivation = true;
bool loraWanAdr = true;
bool keepNet = true;
bool isTxConfirmed = false;
uint8_t appPort = 2;
uint8_t confirmedNbTrials = 4;

/* =========================================================================================
 * 5. FUNCIONES DE SOPORTE
 * ========================================================================================= */
void energizarSensores(bool estado);
float leerNivel();
float leerTurbidez();
void ordenarMuestras(int *arr, int n);
float leerVoltajePHFiltrado();
float leerPH();
void mostrarEstadoOLED(float nivelCm, float turbidezNtu, float phValor);

/* =========================================================================================
 * 6. CONTROL DE POTENCIA DE SENSORES
 * ========================================================================================= */
void energizarSensores(bool estado) {
  if (estado) {
    digitalWrite(MOSFET_PIN, HIGH);
    delay(3000);
  } else {
    digitalWrite(MOSFET_PIN, LOW);
  }
}

/* =========================================================================================
 * 7. MEDICIÓN DE NIVEL POR ULTRASÓNICO
 * =========================================================================================
 * Se protege la captura del pulso de eco deshabilitando temporalmente interrupciones de la
 * radio, ya que el HC-SR04 depende de medición precisa del tiempo de vuelo.
 */
float leerNivel() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(4);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  noInterrupts();
  long duracion = pulseIn(ECHO_PIN, HIGH, 26000);
  interrupts();

  if (duracion == 0) return 0.0;

  float distanciaCm = duracion * 0.0343f / 2.0f;

  if (distanciaCm > 400.0f || distanciaCm < 2.0f) return 0.0f;

  return distanciaCm;
}

/* =========================================================================================
 * 8. MEDICIÓN DE TURBIDEZ
 * =========================================================================================
 * Se aplica sobremuestreo de 10 lecturas para reducir fluctuación. La salida se expresa en
 * NTU a partir del voltaje reconstruido sobre el divisor de acondicionamiento analógico.
 */
float leerTurbidez() {
  long sumaADC = 0;

  for (int i = 0; i < 10; i++) {
    sumaADC += analogRead(ADC_TURB_PIN);
    delay(20);
  }

  float lecturaADC = sumaADC / 10.0f;
  float voltajeADC = lecturaADC * (3.3f / 4095.0f);
  float voltajeSensor = voltajeADC * DIVISOR_FACTOR;
  float turbidezNTU = (5.0f - voltajeSensor) * 200.0f;

  if (turbidezNTU < 0.0f) turbidezNTU = 0.0f;

  return turbidezNTU;
}

/* =========================================================================================
 * 9. UTILIDAD DE ORDENAMIENTO PARA FILTRADO DE pH
 * ========================================================================================= */
void ordenarMuestras(int *arr, int n) {
  for (int i = 0; i < n - 1; i++) {
    for (int j = i + 1; j < n; j++) {
      if (arr[i] > arr[j]) {
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
      }
    }
  }
}

/* =========================================================================================
 * 10. ADQUISICIÓN FILTRADA DEL VOLTAJE DE pH
 * =========================================================================================
 * Se toman 10 muestras, se ordenan y se promedian las centrales para reducir el efecto de
 * ruido, outliers y pequeñas perturbaciones en el acondicionamiento analógico.
 */
float leerVoltajePHFiltrado() {
  const int NUM_SAMPLES = 10;
  int muestras[NUM_SAMPLES];

  for (int i = 0; i < NUM_SAMPLES; i++) {
    muestras[i] = analogRead(ADC_PH_PIN);
    delay(20);
  }

  ordenarMuestras(muestras, NUM_SAMPLES);

  long suma = 0;
  for (int i = 2; i < 8; i++) {
    suma += muestras[i];
  }

  float adcPromedio = suma / 6.0f;
  float voltajeSensor = adcPromedio * (3.3f / 4095.0f);

  return voltajeSensor;
}

/* =========================================================================================
 * 11. CONVERSIÓN A pH
 * ========================================================================================= */
float leerPH() {
  float voltajeSensor = leerVoltajePHFiltrado();
  float phValor = (PH_M * voltajeSensor) + PH_B;

  if (phValor < 0.0f) phValor = 0.0f;
  if (phValor > 14.0f) phValor = 14.0f;

  Serial.printf("[pH] Voltaje: %.3f V | m: %.4f | b: %.4f | pH: %.2f\n",
                voltajeSensor, PH_M, PH_B, phValor);

  return phValor;
}

/* =========================================================================================
 * 12. SALIDA LOCAL EN PANTALLA OLED
 * ========================================================================================= */
void mostrarEstadoOLED(float nivelCm, float turbidezNtu, float phValor) {
  pantalla.clear();
  pantalla.setTextAlignment(TEXT_ALIGN_LEFT);
  pantalla.setFont(ArialMT_Plain_10);
  pantalla.drawString(0, 0,  "HELMO Nodo V3");
  pantalla.drawString(0, 14, "Nivel: " + String(nivelCm, 1) + " cm");
  pantalla.drawString(0, 28, "Turb:  " + String(turbidezNtu, 1) + " NTU");
  pantalla.drawString(0, 42, "pH:    " + String(phValor, 2));
  pantalla.display();
}

/* =========================================================================================
 * 13. PREPARACIÓN DE TELEMETRÍA
 * =========================================================================================
 * Payload de 6 bytes:
 * Byte 0-1 -> nivel x10 en cm (uint16, big-endian)
 * Byte 2-3 -> turbidez x10 en NTU (uint16, big-endian)
 * Byte 4-5 -> pH x100 (uint16, big-endian)
 */
static void prepareTxFrame(uint8_t port) {
  (void)port;

  energizarSensores(true);

  float nivelCm = leerNivel();
  float turbidezNtu = leerTurbidez();
  float phValor = leerPH();

  energizarSensores(false);

  uint16_t nivelInt = (uint16_t)(nivelCm * 10.0f);
  uint16_t turbInt  = (uint16_t)(turbidezNtu * 10.0f);
  uint16_t phInt    = (uint16_t)(phValor * 100.0f);

  appDataSize = 6;

  appData[0] = (uint8_t)(nivelInt >> 8);
  appData[1] = (uint8_t)(nivelInt & 0xFF);
  appData[2] = (uint8_t)(turbInt >> 8);
  appData[3] = (uint8_t)(turbInt & 0xFF);
  appData[4] = (uint8_t)(phInt >> 8);
  appData[5] = (uint8_t)(phInt & 0xFF);

  Serial.println("\n--- [TX] HELMO TRANSMISIÓN PROTEGIDA ---");
  Serial.printf("[-] NODE_ID  : %d\n", NODE_ID);
  Serial.printf("[-] Nivel    : %.1f cm   -> Bytes: [%02X %02X]\n", nivelCm, appData[0], appData[1]);
  Serial.printf("[-] Turbidez : %.1f NTU  -> Bytes: [%02X %02X]\n", turbidezNtu, appData[2], appData[3]);
  Serial.printf("[-] pH       : %.2f      -> Bytes: [%02X %02X]\n", phValor, appData[4], appData[5]);
  Serial.println("-------------------------------------------------");

  mostrarEstadoOLED(nivelCm, turbidezNtu, phValor);
}

/* =========================================================================================
 * 14. CONFIGURACIÓN INICIAL DEL SISTEMA
 * ========================================================================================= */
void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(MOSFET_PIN, OUTPUT);
  digitalWrite(MOSFET_PIN, LOW);

  pinMode(TRIG_PIN, OUTPUT);
  digitalWrite(TRIG_PIN, LOW);
  pinMode(ECHO_PIN, INPUT_PULLDOWN);

  analogReadResolution(12);
  analogSetPinAttenuation(ADC_PH_PIN, ADC_11db);
  analogSetPinAttenuation(ADC_TURB_PIN, ADC_11db);

  Wire.begin(SDA_OLED, SCL_OLED);
  pantalla.init();
  pantalla.clear();
  pantalla.setTextAlignment(TEXT_ALIGN_LEFT);
  pantalla.setFont(ArialMT_Plain_10);
  pantalla.drawString(0, 0, "HELMO inicializando");
  pantalla.drawString(0, 14, "Nodo multisensor");
  pantalla.display();

  Mcu.begin(HELTEC_BOARD, SLOW_CLK_TPYE);
  deviceState = DEVICE_STATE_INIT;
}

/* =========================================================================================
 * 15. MÁQUINA DE ESTADOS LORAWAN
 * ========================================================================================= */
void loop() {
  switch (deviceState) {
    case DEVICE_STATE_INIT: {
      LoRaWAN.init(loraWanClass, loraWanRegion);
      break;
    }

    case DEVICE_STATE_JOIN: {
      LoRaWAN.join();
      break;
    }

    case DEVICE_STATE_SEND: {
      prepareTxFrame(appPort);
      LoRaWAN.send();
      deviceState = DEVICE_STATE_CYCLE;
      break;
    }

    case DEVICE_STATE_CYCLE: {
      txDutyCycleTime = appTxDutyCycle + randr(-APP_TX_DUTYCYCLE_RND, APP_TX_DUTYCYCLE_RND);
      LoRaWAN.cycle(txDutyCycleTime);
      deviceState = DEVICE_STATE_SLEEP;
      break;
    }

    case DEVICE_STATE_SLEEP: {
      LoRaWAN.sleep(loraWanClass);
      break;
    }

    default: {
      deviceState = DEVICE_STATE_INIT;
      break;
    }
  }
}
