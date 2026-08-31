/**
 * HELMO - Puente de telemetría MQTT a MongoDB
 * Archivo: backend/helmo-bridge/index.js
 * Versión: 1.0.0
 *
 * Descripción:
 * Servicio backend encargado de:
 * 1. Conectarse al broker MQTT de The Things Stack (TTN).
 * 2. Suscribirse al tópico de uplinks de la aplicación HELMO.
 * 3. Procesar mensajes con telemetría decodificada.
 * 4. Construir documentos estructurados con variables y metadatos de radio.
 * 5. Guardar la información en MongoDB local.
 * 6. Replicar la información en MongoDB Atlas cuando la nube esté disponible.
 * 7. Mantener operación degradada y reintentos automáticos si Atlas falla.
 *
 * Autoría:
 * - Esteban Eduardo Escárraga Túquerres
 * - Johan Felipe Ordoñez Jenoy
 * - Dayerli Katerine Tamayo Solarte
 */

require('dotenv').config();
global.crypto = require('crypto');

const mqtt = require('mqtt');
const { MongoClient } = require('mongodb');

/* =========================================================
 * Variables de entorno
 * ========================================================= */
const localMongoUrl = process.env.LOCAL_MONGO_URL;
const localDbName = process.env.LOCAL_DB_NAME || 'helmo_database';
const localCollectionName =
  process.env.LOCAL_COLLECTION_NAME || 'monolito_lecturas';

const atlasMongoUrl = process.env.ATLAS_MONGO_URL;
const atlasDbName = process.env.ATLAS_DB_NAME || 'helmo_database';
const atlasCollectionName =
  process.env.ATLAS_COLLECTION_NAME || 'monolito_lecturas';

const ttnHost = process.env.TTN_HOST;
const ttnUsername = process.env.TTN_USERNAME;
const ttnPassword = process.env.TTN_PASSWORD;
const ttnTopic = process.env.TTN_TOPIC;

/* =========================================================
 * Validación mínima de configuración crítica
 * ========================================================= */
if (!localMongoUrl || !ttnHost || !ttnUsername || !ttnPassword || !ttnTopic) {
  console.error('❌ Faltan variables de entorno obligatorias.');
  console.error(
    'Verifica LOCAL_MONGO_URL, TTN_HOST, TTN_USERNAME, TTN_PASSWORD y TTN_TOPIC.'
  );
  process.exit(1);
}

/* =========================================================
 * Clientes MongoDB
 * ========================================================= */
const localClient = new MongoClient(localMongoUrl, {
  serverSelectionTimeoutMS: 5000
});

const atlasClient = atlasMongoUrl
  ? new MongoClient(atlasMongoUrl, {
      serverSelectionTimeoutMS: 10000,
      connectTimeoutMS: 10000,
      socketTimeoutMS: 20000,
      retryWrites: true,
      autoSelectFamily: false
    })
  : null;

/* =========================================================
 * Estado interno del bridge
 * ========================================================= */
let localCollection = null;
let atlasCollection = null;
let atlasDisponible = false;

/* =========================================================
 * Conexión a MongoDB local
 * Base principal de persistencia del sistema
 * ========================================================= */
async function conectarMongoLocal() {
  await localClient.connect();
  const localDb = localClient.db(localDbName);
  localCollection = localDb.collection(localCollectionName);
  console.log('✅ Conectado a MongoDB local.');
}

/* =========================================================
 * Conexión inicial a MongoDB Atlas
 * Si falla, el sistema continúa solo con Mongo local
 * ========================================================= */
async function conectarMongoAtlas() {
  if (!atlasClient) {
    console.log('ℹ️ Atlas no configurado. El bridge operará solo con Mongo local.');
    atlasDisponible = false;
    atlasCollection = null;
    return;
  }

  try {
    await atlasClient.connect();
    const atlasDb = atlasClient.db(atlasDbName);
    atlasCollection = atlasDb.collection(atlasCollectionName);
    atlasDisponible = true;
    console.log('✅ Conectado a MongoDB Atlas.');
  } catch (error) {
    atlasDisponible = false;
    atlasCollection = null;
    console.error(
      '⚠️ Atlas no disponible al iniciar. El bridge seguirá solo con Mongo local.'
    );
    console.error('Detalle Atlas:', error.message);
  }
}

/* =========================================================
 * Reintento de conexión a Atlas
 * Se ejecuta cuando la base cloud no está disponible
 * ========================================================= */
async function reintentarAtlas() {
  if (atlasDisponible || !atlasClient) return;

  try {
    await atlasClient.connect();
    const atlasDb = atlasClient.db(atlasDbName);
    atlasCollection = atlasDb.collection(atlasCollectionName);
    atlasDisponible = true;
    console.log('✅ Reconectado a MongoDB Atlas.');
  } catch (error) {
    atlasDisponible = false;
    atlasCollection = null;
    console.error('⚠️ Reintento Atlas falló:', error.message);
  }
}

/* =========================================================
 * Temporizador periódico de reintento hacia Atlas
 * Intervalo: 60 segundos
 * ========================================================= */
function iniciarReintentoAtlas() {
  setInterval(async () => {
    if (!atlasDisponible) {
      await reintentarAtlas();
    }
  }, 60000);
}

/* =========================================================
 * Persistencia dual del documento de telemetría
 * 1. Guarda en Mongo local
 * 2. Intenta replicar en Atlas si está disponible
 * ========================================================= */
async function guardarDocumento(documento) {
  let localId = null;
  let atlasId = null;

  try {
    if (!localCollection) {
      throw new Error('Colección local no inicializada');
    }

    const localRes = await localCollection.insertOne(documento);
    localId = localRes.insertedId;
  } catch (err) {
    console.error('❌ Error guardando en Mongo local:', err.message);
  }

  if (atlasDisponible && atlasCollection) {
    try {
      const atlasRes = await atlasCollection.insertOne(documento);
      atlasId = atlasRes.insertedId;
    } catch (err) {
      atlasDisponible = false;
      atlasCollection = null;
      console.error(
        '⚠️ Error guardando en Mongo Atlas. Se desactiva temporalmente Atlas:',
        err.message
      );
    }
  }

  console.log(`💾 Local ID: ${localId || 'falló'} | ☁️ Atlas ID: ${atlasId || 'no disponible'}`);
  console.log(`📡 Device: ${documento.dispositivo} | Variables: ${JSON.stringify(documento.variables)}`);
}

/* =========================================================
 * Arranque principal del bridge
 * ========================================================= */
async function iniciarPuente() {
  try {
    await conectarMongoLocal();
    await conectarMongoAtlas();
    iniciarReintentoAtlas();

    const client = mqtt.connect(ttnHost, {
      username: ttnUsername,
      password: ttnPassword
    });

    /* -----------------------------------------
     * Conexión exitosa al broker MQTT
     * ----------------------------------------- */
    client.on('connect', () => {
      console.log('✅ Conectado al broker MQTT de TTN.');

      client.subscribe(ttnTopic, (err) => {
        if (!err) {
          console.log('📡 Suscrito al tópico. Escuchando telemetría...');
        } else {
          console.error('❌ Error al suscribirse:', err.message);
        }
      });
    });

    /* -----------------------------------------
     * Procesamiento de cada mensaje MQTT recibido
     * ----------------------------------------- */
    client.on('message', async (_topic, message) => {
      try {
        const ttnJson = JSON.parse(message.toString());
        const datosSensores = ttnJson.uplink_message?.decoded_payload;

        if (!datosSensores) {
          console.log('⚠️ Uplink sin decoded_payload');
          return;
        }

        const documento = {
          dispositivo: ttnJson.end_device_ids?.device_id || 'desconocido',
          timestamp: ttnJson.received_at
            ? new Date(ttnJson.received_at)
            : new Date(),
          variables: datosSensores,
          radio_metadata: {
            fcnt: ttnJson.uplink_message?.f_cnt,
            rssi: ttnJson.uplink_message?.rx_metadata?.[0]?.rssi,
            snr: ttnJson.uplink_message?.rx_metadata?.[0]?.snr,
            gateway: ttnJson.uplink_message?.rx_metadata?.[0]?.gateway_ids?.gateway_id
          }
        };

        await guardarDocumento(documento);
      } catch (err) {
        console.error('❌ Error procesando mensaje:', err.message);
      }
    });

    /* -----------------------------------------
     * Manejo de errores del cliente MQTT
     * ----------------------------------------- */
    client.on('error', (err) => {
      console.error('❌ Error MQTT:', err.message);
    });
  } catch (error) {
    console.error('❌ Fallo crítico en el arranque del bridge:', error.message);
    process.exit(1);
  }
}

/* =========================================================
 * Inicio del servicio
 * ========================================================= */
iniciarPuente().catch((err) => {
  console.error('❌ Error no controlado al iniciar el bridge:', err.message);
  process.exit(1);
});