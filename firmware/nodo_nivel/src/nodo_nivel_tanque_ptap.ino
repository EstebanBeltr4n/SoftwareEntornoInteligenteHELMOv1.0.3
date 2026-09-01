/*
 * =========================================================================================
 * PROYECTO: SISTEMA MULTI-AGENTE DE MONITOREO HÍDRICO HELMO (PTAP)
 * MÓDULO: NODO DE NIVEL - CAPA LoRaWAN - V1_DOCUMENTAL
 * HARDWARE: Heltec WiFi LoRa 32 V3 (ESP32-S3) + HC-SR04 + etapa de potencia para sensado
 * ARCHIVO: firmware/nodo_nivel/src/nodo_nivel_tanque_ptap.ino
 * =========================================================================================
 *
 * DESCRIPCIÓN
 * Firmware embebido para un nodo de nivel separado dentro de la arquitectura HELMO.
 * El módulo mide el nivel superficial del agua en un tanque o compartimiento de PTAP,
 * estima el estado operativo del punto monitoreado y prepara una trama compacta para
 * transmisión LoRaWAN hacia la infraestructura de red del proyecto.
 *
 * El caso de uso contempla un tanque donde el material filtrante de arena puede hacerse
 * visible cuando la lámina de agua desciende aproximadamente a 20 cm. Por esa razón, el
 * firmware incorpora una lógica de clasificación que identifica condición crítica cuando
 * el filtro se vuelve visible o cuando el nivel útil cae por debajo del umbral definido.
 *
 * NOTA DOCUMENTAL
 * Este archivo se entrega como versión lista para expediente técnico y derechos de autor.
 * Las credenciales LoRaWAN se incluyen como marcadores nulos para evitar exposición de
 * secretos del despliegue real.
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
#define NODE_ID 2

#define TRIG_PIN 4
#define ECHO_PIN 5
#define MOSFET_PIN 6
#define LED_STATUS_PIN 7

#define SDA_OLED 17
#define SCL_OLED 18
#define RST_OLED 21

SSD1306Wire pantalla(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED);

/* =========================================================================================
 * 2. PARÁMETROS DE INSTALACIÓN Y CALIBRACIÓN
 * =========================================================================================
 * SENSOR_TO_REFERENCE_CM representa la distancia vertical entre el sensor y el fondo o
 * plano de referencia del punto monitoreado. Debe ajustarse en la instalación real.
 * FILTER_VISIBLE_THRESHOLD_CM define el umbral en el que el lecho filtrante comienza a
 * ser visible de forma superficial, según la condición operativa descrita para la PTAP.
 */
const float SENSOR_TO_REFERENCE_CM = 120.0f;
const float FILTER_VISIBLE_THRESHOLD_CM = 20.0f;
const float MIN_VALID_DISTANCE_CM = 2.0f;
const float MAX_VALID_DISTANCE_CM = 400.0f;
const uint8_t NUM_SAMPLES = 7;

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
uint8_t appPort = 3;
uint8_t confirmedNbTrials = 4;

/* =========================================================================================
 * 5. ESTRUCTURA DE DATOS DEL NODO
 * ========================================================================================= */
struct LevelData {
  float distanceCm;
  float waterColumnCm;
  float levelPercent;
  bool filterVisible;
  uint8_t levelState;
};

/* =========================================================================================
 * 6. PROTOTIPOS
 * ========================================================================================= */
void energizarSensores(bool estado);
float leerDistanciaSimple();
float leerDistanciaFiltrada();
void ordenarMuestras(float *arr, size_t len);
LevelData construirDatosNivel(float distanceCm);
uint8_t clasificarEstadoNivel(float levelPercent, bool filterVisible);
void mostrarEstadoOLED(const LevelData& data);
void imprimirDiagnosticoSerie(const LevelData& data);

/* =========================================================================================
 * 7. CONTROL DE POTENCIA
 * ========================================================================================= */
void energizarSensores(bool estado) {
  if (estado) {
    digitalWrite(MOSFET_PIN, HIGH);
    delay(2000);
  } else {
    digitalWrite(MOSFET_PIN, LOW);
  }
}

/* =========================================================================================
 * 8. MEDICIÓN ULTRASÓNICA BÁSICA
 * =========================================================================================
 * HC-SR04 requiere un pulso de disparo de 10 us y el cálculo de distancia a partir del
 * tiempo de vuelo del eco. La expresión usada es distancia = tiempo * 0.0343 / 2.
 */
float leerDistanciaSimple() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(4);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  noInterrupts();
  long duracion = pulseIn(ECHO_PIN, HIGH, 26000);
  interrupts();

  if (duracion == 0) return -1.0f;

  float distanciaCm = duracion * 0.0343f / 2.0f;

  if (distanciaCm < MIN_VALID_DISTANCE_CM || distanciaCm > MAX_VALID_DISTANCE_CM) {
    return -1.0f;
  }

  return distanciaCm;
}

/* =========================================================================================
 * 9. FILTRADO POR MEDIANA
 * ========================================================================================= */
float leerDistanciaFiltrada() {
  float muestras[NUM_SAMPLES];
  uint8_t validas = 0;

  for (uint8_t i = 0; i < NUM_SAMPLES; i++) {
    float lectura = leerDistanciaSimple();
    if (lectura > 0) {
      muestras[validas++] = lectura;
    }
    delay(60);
  }

  if (validas < 3) {
    return -1.0f;
  }

  ordenarMuestras(muestras, validas);
  return muestras[validas / 2];
}

/* =========================================================================================
 * 10. ORDENAMIENTO SIMPLE PARA MEDIANA
 * ========================================================================================= */
void ordenarMuestras(float *arr, size_t len) {
  for (size_t i = 0; i < len; i++) {
    for (size_t j = i + 1; j < len; j++) {
      if (arr[j] < arr[i]) {
        float temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
      }
    }
  }
}

/* =========================================================================================
 * 11. CONSTRUCCIÓN DE VARIABLES DERIVADAS
 * ========================================================================================= */
LevelData construirDatosNivel(float distanceCm) {
  LevelData data;
  data.distanceCm = distanceCm;

  float waterColumn = SENSOR_TO_REFERENCE_CM - distanceCm;
  if (waterColumn < 0.0f) waterColumn = 0.0f;
  if (waterColumn > SENSOR_TO_REFERENCE_CM) waterColumn = SENSOR_TO_REFERENCE_CM;

  data.waterColumnCm = waterColumn;
  data.levelPercent = (waterColumn / SENSOR_TO_REFERENCE_CM) * 100.0f;
  data.filterVisible = (waterColumn <= FILTER_VISIBLE_THRESHOLD_CM);
  data.levelState = clasificarEstadoNivel(data.levelPercent, data.filterVisible);

  return data;
}

/* =========================================================================================
 * 12. CLASIFICACIÓN DEL ESTADO
 * =========================================================================================
 * 0 = crítico_bajo
 * 1 = bajo
 * 2 = normal
 * 3 = alto
 */
uint8_t clasificarEstadoNivel(float levelPercent, bool filterVisible) {
  if (filterVisible || levelPercent <= 20.0f) return 0;
  if (levelPercent <= 40.0f) return 1;
  if (levelPercent <= 85.0f) return 2;
  return 3;
}

/* =========================================================================================
 * 13. SALIDA LOCAL EN OLED
 * ========================================================================================= */
void mostrarEstadoOLED(const LevelData& data) {
  pantalla.clear();
  pantalla.setTextAlignment(TEXT_ALIGN_LEFT);
  pantalla.setFont(ArialMT_Plain_10);
  pantalla.drawString(0, 0,  "HELMO Nodo Nivel");
  pantalla.drawString(0, 14, "Dist: " + String(data.distanceCm, 1) + " cm");
  pantalla.drawString(0, 28, "Nivel: " + String(data.levelPercent, 1) + " %");
  pantalla.drawString(0, 42, "Filtro: " + String(data.filterVisible ? "VISIBLE" : "OK"));
  pantalla.display();
}

/* =========================================================================================
 * 14. DIAGNÓSTICO SERIAL
 * ========================================================================================= */
void imprimirDiagnosticoSerie(const LevelData& data) {
  Serial.println("\n--- [TX] HELMO NODO DE NIVEL ---");
  Serial.printf("[-] NODE_ID       : %d\n", NODE_ID);
  Serial.printf("[-] Distancia     : %.2f cm\n", data.distanceCm);
  Serial.printf("[-] Columna agua  : %.2f cm\n", data.waterColumnCm);
  Serial.printf("[-] Nivel         : %.2f %%\n", data.levelPercent);
  Serial.printf("[-] Filtro visible: %s\n", data.filterVisible ? "SI" : "NO");
  Serial.printf("[-] Estado nivel  : %u\n", data.levelState);
}

/* =========================================================================================
 * 15. PREPARACIÓN DE PAYLOAD
 * =========================================================================================
 * Payload de 6 bytes:
 * Byte 0-1 -> distancia x10 (uint16, big-endian)
 * Byte 2-3 -> columna de agua x10 (uint16, big-endian)
 * Byte 4   -> porcentaje de nivel entero (uint8)
 * Byte 5   -> nibble alto: filterVisible, nibble bajo: levelState
 */
static void prepareTxFrame(uint8_t port) {
  (void)port;

  energizarSensores(true);
  float distanciaCm = leerDistanciaFiltrada();
  energizarSensores(false);

  if (distanciaCm < 0.0f) {
    distanciaCm = SENSOR_TO_REFERENCE_CM;
  }

  LevelData data = construirDatosNivel(distanciaCm);

  uint16_t distanceScaled = (uint16_t)(data.distanceCm * 10.0f);
  uint16_t waterScaled = (uint16_t)(data.waterColumnCm * 10.0f);
  uint8_t levelPercentInt = (uint8_t)(data.levelPercent);
  uint8_t statePacked = (uint8_t)(((data.filterVisible ? 1 : 0) << 4) | (data.levelState & 0x0F));

  appDataSize = 6;
  appData[0] = (uint8_t)(distanceScaled >> 8);
  appData[1] = (uint8_t)(distanceScaled & 0xFF);
  appData[2] = (uint8_t)(waterScaled >> 8);
  appData[3] = (uint8_t)(waterScaled & 0xFF);
  appData[4] = levelPercentInt;
  appData[5] = statePacked;

  imprimirDiagnosticoSerie(data);

  Serial.printf("[-] Payload bytes : [%02X %02X %02X %02X %02X %02X]\n",
                appData[0], appData[1], appData[2], appData[3], appData[4], appData[5]);
  Serial.println("---------------------------------------------");

  mostrarEstadoOLED(data);
}

/* =========================================================================================
 * 16. CONFIGURACIÓN INICIAL
 * ========================================================================================= */
void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(MOSFET_PIN, OUTPUT);
  digitalWrite(MOSFET_PIN, LOW);

  pinMode(TRIG_PIN, OUTPUT);
  digitalWrite(TRIG_PIN, LOW);
  pinMode(ECHO_PIN, INPUT_PULLDOWN);

  pinMode(LED_STATUS_PIN, OUTPUT);
  digitalWrite(LED_STATUS_PIN, LOW);

  Wire.begin(SDA_OLED, SCL_OLED);
  pantalla.init();
  pantalla.clear();
  pantalla.setTextAlignment(TEXT_ALIGN_LEFT);
  pantalla.setFont(ArialMT_Plain_10);
  pantalla.drawString(0, 0, "HELMO inicializando");
  pantalla.drawString(0, 14, "Nodo de nivel");
  pantalla.display();

  Mcu.begin(HELTEC_BOARD, SLOW_CLK_TPYE);
  deviceState = DEVICE_STATE_INIT;
}

/* =========================================================================================
 * 17. MÁQUINA DE ESTADOS LORAWAN
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
