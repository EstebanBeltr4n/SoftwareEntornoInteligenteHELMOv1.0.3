/**
 * HELMO - API de ingestión y consulta MongoDB
 * Archivo: backend/api/api-mongodb.js
 * Versión: 1.0.0
 *
 * Descripción:
 * Servicio backend desarrollado en Node.js y Express para:
 * 1. Recibir webhooks HTTP enviados desde The Things Stack.
 * 2. Extraer telemetría decodificada desde decoded_payload.
 * 3. Almacenar lecturas en MongoDB local.
 * 4. Exponer una ruta autenticada para consulta de datos recientes.
 *
 * Autoría:
 * - Esteban Eduardo Escárraga Túquerres
 * - Johan Felipe Ordoñez Jenoy
 * - Dayerli Katerine Tamayo Solarte
 */

require('dotenv').config();
globalThis.crypto = require('node:crypto').webcrypto;

const express = require('express');
const { MongoClient } = require('mongodb');
const auth = require('basic-auth');

const app = express();

const PORT = Number(process.env.API_PORT || 3001);
const API_USERNAME = process.env.API_USERNAME;
const API_PASSWORD = process.env.API_PASSWORD;

const MONGO_URL = process.env.MONGO_URL;
const DB_NAME = process.env.DB_NAME || 'helmo_database';
const COLLECTION_NAME = process.env.COLLECTION_NAME || 'monolito_lecturas';

if (!API_USERNAME || !API_PASSWORD || !MONGO_URL) {
  console.error('❌ Faltan variables de entorno obligatorias.');
  console.error('Verifica API_USERNAME, API_PASSWORD y MONGO_URL.');
  process.exit(1);
}

app.use(express.json());

const mongoClient = new MongoClient(MONGO_URL, {
  serverSelectionTimeoutMS: 5000
});

let collection;

/**
 * Inicializa la conexión a MongoDB y deja lista la colección de trabajo.
 */
async function initMongo() {
  await mongoClient.connect();
  const db = mongoClient.db(DB_NAME);
  collection = db.collection(COLLECTION_NAME);
  console.log('✅ Conectado a MongoDB.');
}

/**
 * Middleware de autenticación básica para proteger la ruta de consulta.
 */
function basicAuthMiddleware(req, res, next) {
  const credentials = auth(req);

  if (
    !credentials ||
    credentials.name !== API_USERNAME ||
    credentials.pass !== API_PASSWORD
  ) {
    res.set('WWW-Authenticate', 'Basic realm="helmo-api"');
    return res.status(401).send('Acceso denegado');
  }

  next();
}

/**
 * Ruta de estado básica para verificación local del servicio.
 */
app.get('/', (req, res) => {
  res.json({
    ok: true,
    servicio: 'helmo-api-mongodb',
    puerto: PORT
  });
});

/**
 * Ruta de salud para monitoreo del proceso.
 */
app.get('/health', (req, res) => {
  res.json({
    ok: true,
    status: 'up'
  });
});

/**
 * Endpoint webhook para recibir telemetría desde TTN.
 * Extrae decoded_payload y guarda un documento estructurado en MongoDB.
 */
app.post('/datos', async (req, res) => {
  try {
    console.log('📥 Webhook TTN recibido');

    const ttnJson = req.body;
    const datosSensores = ttnJson.uplink_message?.decoded_payload;

    if (!datosSensores) {
      console.log('⚠️ Webhook recibido sin decoded_payload');
      return res.status(200).json({
        ok: true,
        warning: 'Webhook sin decoded_payload'
      });
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

    const resultado = await collection.insertOne(documento);

    console.log(`💾 Webhook guardado en MongoDB. ID: ${resultado.insertedId}`);
    console.log(
      `📡 Device: ${documento.dispositivo} | Variables: ${JSON.stringify(documento.variables)}`
    );

    return res.status(200).json({
      ok: true,
      insertedId: resultado.insertedId
    });
  } catch (error) {
    console.error('❌ Error webhook:', error.message);
    return res.status(500).json({
      ok: false,
      error: error.message
    });
  }
});

/**
 * Endpoint autenticado para consultar las últimas lecturas almacenadas.
 */
app.get('/datos', basicAuthMiddleware, async (req, res) => {
  try {
    const data = await collection
      .find({})
      .sort({ timestamp: -1 })
      .limit(100)
      .toArray();

    return res.json(data);
  } catch (error) {
    console.error('❌ Error consultando MongoDB:', error.message);
    return res.status(500).send('Error al obtener datos');
  }
});

/**
 * Inicializa MongoDB y arranca el servidor HTTP.
 */
async function startServer() {
  try {
    await initMongo();

    app.listen(PORT, () => {
      console.log(`✅ API activa en http://localhost:${PORT}`);
    });
  } catch (error) {
    console.error('❌ Fallo crítico al iniciar la API:', error.message);
    process.exit(1);
  }
}

startServer();