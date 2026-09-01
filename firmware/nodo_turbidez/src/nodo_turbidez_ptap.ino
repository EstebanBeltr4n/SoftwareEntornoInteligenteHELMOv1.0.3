/*
 * =========================================================================================
 * PROYECTO: SISTEMA MULTI-AGENTE DE MONITOREO HÍDRICO HELMO (PTAP)
 * MÓDULO: NODO DE TURBIDEZ - CAPA LoRaWAN - V1_DOCUMENTAL
 * HARDWARE: Heltec WiFi LoRa 32 V3 (ESP32-S3) + sensor analógico de turbidez + MOSFET
 * ARCHIVO: firmware/nodo_turbidez/src/nodo_turbidez_ptap.ino
 * =========================================================================================
 *
 * DESCRIPCIÓN
 * Firmware embebido para un nodo de turbidez separado dentro de la arquitectura HELMO.
 * El nodo realiza adquisición analógica de la salida del sensor de turbidez, aplica
 * sobremuestreo y conversión a voltaje, estima un valor de turbidez en NTU, clasifica el
 * estado del agua y empaqueta una trama compacta para transmisión LoRaWAN.
 *
 * Este archivo se entrega como versión documental preparada para expediente técnico y
 * soporte de derechos de autor. Las credenciales LoRaWAN fueron neutralizadas para evitar
 * exposición de secretos operativos del sistema en campo.
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
 * 1. IDENTIFICACIÓN Y PINES DEL NODO
 * ========================================================================================= */
#define NODE_ID 3

#define ADC_TURB_PIN 2
#define MOSFET_PIN 6
#define LED_STATUS_PIN 7

#define SDA_OLED 17
#define SCL_OLED 18
#define RST_OLED 21

SSD1306Wire pantalla(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED);

/* =========================================================================================
 * 2. PARÁMETROS DE CALIBRACIÓN
 * =========================================================================================
 * DIVISOR_FACTOR corrige el voltaje real del sensor cuando la etapa analógica incluye un
 * divisor o acondicionamiento entre la salida del sensor y el ADC del ESP32-S3.
 * NTU_A y NTU_B representan una aproximación lineal configurable, útil como base de campo.
 */
const float DIVISOR_FACTOR = 1.515f;
const float NTU_A = -112.0f;
const float NTU_B = 193.33f;
const uint8_t NUM_SAMPLES = 20;

/* Umbrales operativos sugeridos para clasificación documental */
const float NTU_LOW_THRESHOLD = 1.0f;
const float NTU_MEDIUM_THRESHOLD = 5.0f;
const float NTU_HIGH_THRESHOLD = 50.0f;

/* =========================================================================================
 * 3. CREDENCIALES LORAWAN (OTAA)
 * ========================================================================================= */
uint8_t devEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

uint8_t nwkSKey[] = { 0x00 };
uint8_t appSKey[] = { 0x00 };
uint32_t devAddr  = 0x00;

/* =========================================================================================
 * 4. PARÁMETROS LoRaWAN
 * ========================================================================================= */
uint16_t userChannelsMask[6] = { 0xFF00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };
LoRaMacRegion_t loraWanRegion = ACTIVE_REGION;
DeviceClass_t loraWanClass = CLASS_A;
uint32_t appTxDutyCycle = 60000;
bool overTheAirActivation = true;
bool loraWanAdr = true;
bool keepNet = true;
bool isTxConfirmed = false;
uint8_t appPort = 4;
uint8_t confirmedNbTrials = 4;

/* =========================================================================================
 * 5. ESTRUCTURA DE DATOS DEL NODO
 * ========================================================================================= */
struct TurbidityData {
  uint16_t adcRaw;
  float adcVoltage;
  float sensorVoltage;
  float turbidityNtu;
  uint8_t turbidityState;
};

/* =========================================================================================
 * 6. PROTOTIPOS
 * ========================================================================================= */
void energizarSensores(bool estado);
uint16_t leerADCFiltrado();
TurbidityData construirDatosTurbidez(uint16_t adcRaw);
uint8_t clasificarTurbidez(float ntu);
void mostrarEstadoOLED(const TurbidityData& data);
void imprimirDiagnosticoSerie(const TurbidityData& data);

/* =========================================================================================
 * 7. CONTROL DE POTENCIA
 * ========================================================================================= */
void energizarSensores(bool estado) {
  if (estado) {
    digitalWrite(MOSFET_PIN, HIGH);
    delay(2500);
  } else {
    digitalWrite(MOSFET_PIN, LOW);
  }
}

/* =========================================================================================
 * 8. LECTURA ANALÓGICA FILTRADA
 * =========================================================================================
 * Se usa sobremuestreo simple y promedio para reducir ruido del ADC y pequeñas variaciones
 * en la salida del sensor de turbidez.
 */
uint16_t leerADCFiltrado() {
  uint32_t suma = 0;

  for (uint8_t i = 0; i < NUM_SAMPLES; i++) {
    suma += analogRead(ADC_TURB_PIN);
    delay(20);
  }

  return (uint16_t)(suma / NUM_SAMPLES);
}

/* =========================================================================================
 * 9. CONSTRUCCIÓN DE VARIABLES DERIVADAS
 * ========================================================================================= */
TurbidityData construirDatosTurbidez(uint16_t adcRaw) {
  TurbidityData data;
  data.adcRaw = adcRaw;
  data.adcVoltage = adcRaw * (3.3f / 4095.0f);
  data.sensorVoltage = data.adcVoltage * DIVISOR_FACTOR;
  data.turbidityNtu = (NTU_A * data.sensorVoltage) + NTU_B;

  if (data.turbidityNtu < 0.0f) data.turbidityNtu = 0.0f;
  data.turbidityState = clasificarTurbidez(data.turbidityNtu);

  return data;
}

/* =========================================================================================
 * 10. CLASIFICACIÓN DEL ESTADO DE TURBIDEZ
 * =========================================================================================
 * 0 = muy_baja
 * 1 = baja
 * 2 = media
 * 3 = alta
 */
uint8_t clasificarTurbidez(float ntu) {
  if (ntu <= NTU_LOW_THRESHOLD) return 0;
  if (ntu <= NTU_MEDIUM_THRESHOLD) return 1;
  if (ntu <= NTU_HIGH_THRESHOLD) return 2;
  return 3;
}

/* =========================================================================================
 * 11. SALIDA LOCAL EN OLED
 * ========================================================================================= */
void mostrarEstadoOLED(const TurbidityData& data) {
  pantalla.clear();
  pantalla.setTextAlignment(TEXT_ALIGN_LEFT);
  pantalla.setFont(ArialMT_Plain_10);
  pantalla.drawString(0, 0,  "HELMO Nodo Turb");
  pantalla.drawString(0, 14, "ADC: " + String(data.adcRaw));
  pantalla.drawString(0, 28, "V:   " + String(data.sensorVoltage, 3));
  pantalla.drawString(0, 42, "NTU: " + String(data.turbidityNtu, 2));
  pantalla.display();
}

/* =========================================================================================
 * 12. DIAGNÓSTICO SERIAL
 * ========================================================================================= */
void imprimirDiagnosticoSerie(const TurbidityData& data) {
  Serial.println("\n--- [TX] HELMO NODO DE TURBIDEZ ---");
  Serial.printf("[-] NODE_ID        : %d\n", NODE_ID);
  Serial.printf("[-] ADC raw        : %u\n", data.adcRaw);
  Serial.printf("[-] ADC voltage    : %.3f V\n", data.adcVoltage);
  Serial.printf("[-] Sensor voltage : %.3f V\n", data.sensorVoltage);
  Serial.printf("[-] Turbidity      : %.2f NTU\n", data.turbidityNtu);
  Serial.printf("[-] TurbidityState : %u\n", data.turbidityState);
}

/* =========================================================================================
 * 13. PREPARACIÓN DE PAYLOAD
 * =========================================================================================
 * Payload de 6 bytes:
 * Byte 0-1 -> adcRaw (uint16, big-endian)
 * Byte 2-3 -> turbidity x10 NTU (uint16, big-endian)
 * Byte 4   -> sensor voltage x100 (uint8, saturado)
 * Byte 5   -> turbidityState (uint8)
 */
static void prepareTxFrame(uint8_t port) {
  (void)port;

  energizarSensores(true);
  uint16_t adcRaw = leerADCFiltrado();
  energizarSensores(false);

  TurbidityData data = construirDatosTurbidez(adcRaw);

  uint16_t ntuScaled = (uint16_t)(data.turbidityNtu * 10.0f);
  uint16_t adcField = data.adcRaw;
  uint8_t voltageScaled = (uint8_t)((data.sensorVoltage * 100.0f) > 255.0f ? 255.0f : (data.sensorVoltage * 100.0f));
  uint8_t stateField = data.turbidityState;

  appDataSize = 6;
  appData[0] = (uint8_t)(adcField >> 8);
  appData[1] = (uint8_t)(adcField & 0xFF);
  appData[2] = (uint8_t)(ntuScaled >> 8);
  appData[3] = (uint8_t)(ntuScaled & 0xFF);
  appData[4] = voltageScaled;
  appData[5] = stateField;

  imprimirDiagnosticoSerie(data);
  Serial.printf("[-] Payload bytes  : [%02X %02X %02X %02X %02X %02X]\n",
                appData[0], appData[1], appData[2], appData[3], appData[4], appData[5]);
  Serial.println("-----------------------------------------------");

  mostrarEstadoOLED(data);
}

/* =========================================================================================
 * 14. CONFIGURACIÓN INICIAL
 * ========================================================================================= */
void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(MOSFET_PIN, OUTPUT);
  digitalWrite(MOSFET_PIN, LOW);

  pinMode(LED_STATUS_PIN, OUTPUT);
  digitalWrite(LED_STATUS_PIN, LOW);

  analogReadResolution(12);
  analogSetPinAttenuation(ADC_TURB_PIN, ADC_11db);

  Wire.begin(SDA_OLED, SCL_OLED);
  pantalla.init();
  pantalla.clear();
  pantalla.setTextAlignment(TEXT_ALIGN_LEFT);
  pantalla.setFont(ArialMT_Plain_10);
  pantalla.drawString(0, 0, "HELMO inicializando");
  pantalla.drawString(0, 14, "Nodo turbidez");
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
      digitalWrite(LED_STATUS_PIN, HIGH);
      prepareTxFrame(appPort);
      LoRaWAN.send();
      digitalWrite(LED_STATUS_PIN, LOW);
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
