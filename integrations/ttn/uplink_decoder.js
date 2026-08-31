/**
 * HELMO - Decodificador Uplink para The Things Stack
 * Módulo: integrations/ttn/uplink-decoder.js
 * Versión: 1.0.0
 * Autes: Esteban Eduardo Escárraga Túquerres, Johan Felipe Ordoñez Jenoy y Dayerli Katerine Tamayo Solarte
 *  
 * Propósito:
 * Decodificar payloads LoRaWAN recibidos por TTN para nodos del sistema HELMO.
 *
 * Soporta:
 * - Nodo monolítico multisensor
 * - Nodo dedicado de pH con identificador
 * - Formato heredado de compatibilidad turbidez+pH
 *
 * Entrada:
 * input.bytes -> arreglo de bytes del payload
 * input.fPort -> puerto LoRaWAN
 *
 * Salida:
 * Objeto con estructura:
 * {
 *   data: {...},
 *   warnings: [],
 *   errors: []
 * }
 */

function getPhNodeAlias(nodeId) {
  var aliases = {
    1: "phcasa1",
    2: "phcasa2",
    3: "phcasa3",
    4: "phcasa4",
    5: "phcasa5",
    6: "phcasa6"
  };

  return aliases[nodeId] || ("phcasa_desconocido_" + nodeId);
}

function decodeUplink(input) {
  var data = {};
  var bytes = input.bytes;
  var longitud = bytes.length;
  var puerto = input.fPort;

  // Caso 1:
  // Puerto 2, 6 bytes
  // Estructura:
  // [nivel_H][nivel_L][turbidez_H][turbidez_L][ph_H][ph_L]
  if (puerto === 2 && longitud === 6) {
    var nivelRaw = (bytes[0] << 8) | bytes[1];
    var turbidezRaw = (bytes[2] << 8) | bytes[3];
    var phRaw = (bytes[4] << 8) | bytes[5];

    data.tipo_nodo = "monolito";
    data.familia_nodo = "multisensor";
    data.nivel_cm = nivelRaw / 10.0;
    data.turbidez_ntu = turbidezRaw / 10.0;
    data.ph = phRaw / 100.0;
  }

  // Caso 2:
  // Puerto 3, 5 bytes
  // Estructura:
  // [node_id][ph_H][ph_L][volt_H][volt_L]
  else if (puerto === 3 && longitud === 5) {
    var nodeId = bytes[0];
    var phRaw5 = (bytes[1] << 8) | bytes[2];
    var voltRaw5 = (bytes[3] << 8) | bytes[4];

    data.node_id = nodeId;
    data.tipo_nodo = getPhNodeAlias(nodeId);
    data.familia_nodo = "ph";
    data.ph = phRaw5 / 100.0;
    data.ph_voltage = voltRaw5 / 1000.0;
  }

  // Caso 3:
  // Puerto 2, 4 bytes
  // Estructura heredada:
  // [turbidez_H][turbidez_L][ph_H][ph_L]
  else if (puerto === 2 && longitud === 4) {
    var turbidezRaw4 = (bytes[0] << 8) | bytes[1];
    var phRaw4 = (bytes[2] << 8) | bytes[3];

    data.tipo_nodo = "legacy_tp";
    data.familia_nodo = "legacy";
    data.turbidez_ntu = turbidezRaw4 / 10.0;
    data.ph = phRaw4 / 100.0;
    data.nivel_cm = 0.0;
  }

  else {
    return {
      errors: ["Payload no reconocido. fPort=" + puerto + ", bytes=" + longitud]
    };
  }

  return {
    data: data,
    warnings: [],
    errors: []
  };
}