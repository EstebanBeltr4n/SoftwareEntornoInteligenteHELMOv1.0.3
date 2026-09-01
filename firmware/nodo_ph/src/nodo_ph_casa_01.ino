/*
 * =========================================================================================
 * PROYECTO: HELMO - SISTEMA INTELIGENTE DE MONITOREO HÍDRICO
 * MÓDULO: NODO DEDICADO DE pH - CAPA LoRaWAN - V1_DOCUMENTAL
 * HARDWARE: Heltec WiFi LoRa 32 V3 (ESP32-S3) + sensor de pH analógico + MOSFET
 * ENTORNO: Arduino IDE
 * ARCHIVO: firmware/nodo_ph/src/nodo_ph_casa_01.ino
 * =========================================================================================
 *
 * DESCRIPCIÓN
 * Firmware para nodo LoRaWAN OTAA especializado en medición de pH dentro de la red HELMO.
 * Conserva la lógica robusta de adquisición del nodo multisensor, pero se simplifica a una
 * sola variable de proceso para reducir complejidad eléctrica, ruido, consumo energético y
 * esfuerzo de depuración. El nodo energiza el sensor temporalmente, adquiere múltiples
 * muestras ADC, filtra por ordenamiento y recorte de extremos, convierte voltaje a pH por
 * calibración lineal de dos puntos y transmite una trama compacta por LoRaWAN.
 *
 * CONTEXTO DE DESPLIEGUE
 * Punto de instalación de referencia: Casa usuario Elvira.
 *
 * NOTA DOCUMENTAL
 * Esta versión está preparada para soporte de derechos de autor. Las credenciales LoRaWAN
 * incluidas por el usuario fueron reemplazadas por marcadores nulos para evitar exposición
 * de secretos operativos en el expediente o repositorio público.
 *
 * EXTENSIBILIDAD DENTRO DE HELMO
 * Esta base de firmware permite derivar otros nodos dedicados de la red LoRa HELMO para
 * monitoreo en diferentes ubicaciones, manteniendo la misma estructura de adquisición,
 * payload, transmisión y documentación técnica del proyecto.
 *
 * AUTORÍA
 * - Esteban Eduardo Escárraga Túquerres
 * - Johan Felipe Ordoñez Jenoy
 * - Dayerli Katerine Tamayo Solarte
 */

#include "LoRaWan_APP.h"
#include <Wire.h>
#include "HT_SSD1306Wire.h"

/* -----------------------------------------------------------------------------------------
 * 1. DEFINICIÓN DE HARDWARE
 * ----------------------------------------------------------------------------------------- */
#define NODE_ID 1
const char* NODO_ALIAS = "ph-casa-01";

#define ADC_PH_PIN 1
#define MOSFET_PIN 6

#define SDA_OLED 17
#define SCL_OLED 18
#define RST_OLED 21

SSD1306Wire pantalla(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED);

/* -----------------------------------------------------------------------------------------
 * 2. CALIBRACIÓN DE pH
 * -----------------------------------------------------------------------------------------
 * Calibración lineal de dos puntos:
 *  - Buffer ácido: pH 4.00
 *  - Buffer neutro: pH 7.00
 * Modelo: pH = m * V + b
 */
#define PH4_BUFFER_VALUE 4.00
#define PH7_BUFFER_VALUE 7.00
#define PH4_VOLTAGE 2.560
#define PH7_VOLTAGE 2.250
#define PH_M ((PH7_BUFFER_VALUE - PH4_BUFFER_VALUE) / (PH7_VOLTAGE - PH4_VOLTAGE))
#define PH_B (PH7_BUFFER_VALUE - (PH_M * PH7_VOLTAGE))

/* -----------------------------------------------------------------------------------------
 * 3. CREDENCIALES LORAWAN OTAA
 * ----------------------------------------------------------------------------------------- */
uint8_t devEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

uint8_t nwkSKey[] = { 0x00 };
uint8_t appSKey[] = { 0x00 };
uint32_t devAddr = 0x00;

/* -----------------------------------------------------------------------------------------
 * 4. PARÁMETROS MAC / REGIÓN
 * ----------------------------------------------------------------------------------------- */
uint16_t userChannelsMask[6] = { 0xFF00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };
LoRaMacRegion_t loraWanRegion = ACTIVE_REGION;
DeviceClass_t loraWanClass = CLASS_A;
uint32_t appTxDutyCycle = 60000;
bool overTheAirActivation = true;
bool loraWanAdr = true;
bool keepNet = true;
bool isTxConfirmed = false;
uint8_t appPort = 3;
uint8_t confirmedNbTrials = 4;

/* -----------------------------------------------------------------------------------------
 * 5. PROTOTIPOS
 * ----------------------------------------------------------------------------------------- */
void energizarSensorPH(bool estado);
void ordenarMuestras(int *arr, int n);
float leerVoltajePHFiltrado();
float leerPH();
void mostrarEstadoOLED(float phValor, float voltajeSensor);

/* -----------------------------------------------------------------------------------------
 * 6. GESTIÓN DE ENERGÍA DEL SENSOR
 * ----------------------------------------------------------------------------------------- */
void energizarSensorPH(bool estado) {
  if (estado) {
    digitalWrite(MOSFET_PIN, HIGH);
    delay(3000);
  } else {
    digitalWrite(MOSFET_PIN, LOW);
  }
}

/* -----------------------------------------------------------------------------------------
 * 7. FILTRADO DE MUESTRAS ADC
 * ----------------------------------------------------------------------------------------- */
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

/* -----------------------------------------------------------------------------------------
 * 8. CONVERSIÓN A pH
 * ----------------------------------------------------------------------------------------- */
float leerPH() {
  float voltajeSensor = leerVoltajePHFiltrado();
  float phValor = (PH_M * voltajeSensor) + PH_B;

  if (phValor < 0.0f) phValor = 0.0f;
  if (phValor > 14.0f) phValor = 14.0f;

  Serial.printf("[pH] Voltaje: %.3f V | m: %.4f | b: %.4f | pH: %.2f\n",
                voltajeSensor, PH_M, PH_B, phValor);

  return phValor;
}

/* -----------------------------------------------------------------------------------------
 * 9. SALIDA LOCAL OLED
 * ----------------------------------------------------------------------------------------- */
void mostrarEstadoOLED(float phValor, float voltajeSensor) {
  pantalla.clear();
  pantalla.setTextAlignment(TEXT_ALIGN_LEFT);
  pantalla.setFont(ArialMT_Plain_10);
  pantalla.drawString(0, 0,  "HELMO Nodo pH");
  pantalla.drawString(0, 14, "Alias: ph-casa-01");
  pantalla.drawString(0, 28, "pH: " + String(phValor, 2));
  pantalla.drawString(0, 42, "V:  " + String(voltajeSensor, 3));
  pantalla.display();
}

/* -----------------------------------------------------------------------------------------
 * 10. EMPAQUETADO DE TELEMETRÍA
 * -----------------------------------------------------------------------------------------
 * Payload de 5 bytes:
 * [0]    NODE_ID
 * [1-2]  pH x100 (uint16, big-endian)
 * [3-4]  voltaje x1000 (uint16, big-endian)
 */
static void prepareTxFrame(uint8_t port) {
  (void)port;

  energizarSensorPH(true);

  float voltajeSensor = leerVoltajePHFiltrado();
  float phVal = (PH_M * voltajeSensor) + PH_B;

  if (phVal < 0.0f) phVal = 0.0f;
  if (phVal > 14.0f) phVal = 14.0f;

  energizarSensorPH(false);

  uint16_t phInt = (uint16_t)(phVal * 100.0f);
  uint16_t voltInt = (uint16_t)(voltajeSensor * 1000.0f);

  appDataSize = 5;
  appData[0] = (uint8_t)NODE_ID;
  appData[1] = (uint8_t)(phInt >> 8);
  appData[2] = (uint8_t)(phInt & 0xFF);
  appData[3] = (uint8_t)(voltInt >> 8);
  appData[4] = (uint8_t)(voltInt & 0xFF);

  Serial.printf("[NODO %s] NODE_ID: %d | pH: %.2f | Voltaje: %.3f V\n",
                NODO_ALIAS, NODE_ID, phVal, voltajeSensor);
  Serial.printf("[-] NODE_ID : %d -> Byte: [%02X]\n", NODE_ID, appData[0]);
  Serial.printf("[-] pH      : %.2f -> Bytes: [%02X %02X]\n", phVal, appData[1], appData[2]);
  Serial.printf("[-] Voltaje : %.3f -> Bytes: [%02X %02X]\n", voltajeSensor, appData[3], appData[4]);
  Serial.println("------------------------------------------------");

  mostrarEstadoOLED(phVal, voltajeSensor);
}

/* -----------------------------------------------------------------------------------------
 * 11. CONFIGURACIÓN INICIAL
 * ----------------------------------------------------------------------------------------- */
void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(MOSFET_PIN, OUTPUT);
  digitalWrite(MOSFET_PIN, LOW);

  analogReadResolution(12);
  analogSetPinAttenuation(ADC_PH_PIN, ADC_11db);

  Wire.begin(SDA_OLED, SCL_OLED);
  pantalla.init();
  pantalla.clear();
  pantalla.setTextAlignment(TEXT_ALIGN_LEFT);
  pantalla.setFont(ArialMT_Plain_10);
  pantalla.drawString(0, 0, "HELMO inicializando");
  pantalla.drawString(0, 14, "Nodo pH dedicado");
  pantalla.display();

  Mcu.begin(HELTEC_BOARD, SLOW_CLK_TPYE);
  deviceState = DEVICE_STATE_INIT;
}

/* -----------------------------------------------------------------------------------------
 * 12. BUCLE PRINCIPAL LoRaWAN
 * ----------------------------------------------------------------------------------------- */
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
