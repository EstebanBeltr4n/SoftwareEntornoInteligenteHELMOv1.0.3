# HELMO Bridge - MQTT a MongoDB

El módulo `helmo-bridge` es un componente backend del sistema HELMO encargado de recibir telemetría IoT desde The Things Stack mediante MQTT, procesarla como eventos JSON y almacenarla en bases de datos MongoDB. Su objetivo es constituirse en el puente funcional entre la infraestructura LoRaWAN del proyecto y la capa de persistencia de datos utilizada para visualización, análisis y respaldo operativo.[cite:1]

En la arquitectura general del sistema, este servicio se ejecuta sobre un servidor físico con Debian 12 y opera como intermediario entre la red LoRaWAN y las capas de almacenamiento local y cloud. HELMO utiliza un flujo de telemetría donde los nodos envían datos al gateway LoRa LP8N, TTN recibe y decodifica los mensajes, y posteriormente este bridge los consume para escribirlos en MongoDB local y MongoDB Atlas cuando la nube está disponible.[cite:1][cite:45]

## Propósito del módulo

El archivo principal `index.js` implementa la lógica de suscripción al broker MQTT de TTN, extracción de `decoded_payload`, estructuración de documentos de telemetría y persistencia dual en MongoDB. Este enfoque permite que la solución opere de forma tolerante a fallos, ya que si MongoDB Atlas no está disponible, el bridge continúa funcionando con la base local y reintenta la conexión cloud de forma periódica.[cite:1]

Este comportamiento es especialmente importante en entornos de monitoreo ambiental y de calidad del agua, donde la continuidad operativa del almacenamiento local resulta prioritaria aun ante fallos temporales de conectividad externa o indisponibilidad de la nube. El módulo, por tanto, materializa una estrategia de redundancia lógica dentro del backend HELMO.[cite:1][cite:91]

## Ubicación sugerida en el repositorio

```text
backend/
└── helmo-bridge/
    ├── index.js
    ├── package.json
    ├── .env.example
    ├── .gitignore
    ├── README.md
    └── execution-flow.md
```

Esta estructura separa claramente el puente MQTT–MongoDB de otros componentes backend como la API HTTP de consulta o los scripts de despliegue. La organización modular mejora la mantenibilidad del software, facilita su revisión técnica y fortalece la presentación del repositorio con fines de documentación académica o registro de software.[cite:1][cite:89]

## Flujo funcional resumido

1. El bridge establece conexión con MongoDB local.[cite:1]
2. Intenta conectarse a MongoDB Atlas; si no es posible, opera en modo degradado solo con almacenamiento local.[cite:1]
3. Inicia un ciclo periódico de reintento para la reconexión cloud.[cite:1]
4. Se conecta al broker MQTT de The Things Stack y se suscribe al tópico de uplinks de la aplicación HELMO.[cite:47]
5. Cada mensaje recibido se parsea como JSON y se analiza el campo `uplink_message.decoded_payload`.[cite:48]
6. Se construye un documento estructurado con identificador del dispositivo, fecha/hora, variables y metadatos de radio.[cite:48]
7. El documento se almacena en MongoDB local y, si está habilitado, también en MongoDB Atlas.[cite:1][cite:48]

## Variables de entorno

La configuración sensible del módulo no debe quedar incrustada en el código fuente. Node.js documenta `process.env` como interfaz estándar para variables de entorno, por lo que el bridge debe utilizar este mecanismo para definir cadenas de conexión, usuario, contraseña y tópico de suscripción.[cite:110][cite:141]

El archivo `.env.example` documenta las variables requeridas por el servicio sin exponer credenciales reales. Este archivo debe subirse al repositorio, mientras que el archivo `.env` con los secretos reales debe mantenerse excluido del control de versiones mediante `.gitignore`.[cite:110][cite:111]

## Documento persistido

Cada mensaje procesado por el bridge genera un documento con la siguiente estructura lógica:

| Campo | Descripción |
|---|---|
| `dispositivo` | Identificador del nodo emisor obtenido desde TTN.[cite:48] |
| `timestamp` | Fecha y hora de recepción del mensaje, tomada de `received_at` o generada localmente.[cite:48] |
| `variables` | Objeto con variables decodificadas del nodo o sensor.[cite:48] |
| `radio_metadata.fcnt` | Contador de trama uplink.[cite:48] |
| `radio_metadata.rssi` | Intensidad de señal recibida.[cite:48] |
| `radio_metadata.snr` | Relación señal-ruido del enlace.[cite:48] |
| `radio_metadata.gateway` | Identificador del gateway que recibió el mensaje.[cite:48] |

## Ejecución básica

Con las variables correctamente definidas, el módulo puede ejecutarse mediante Node.js como servicio backend del proyecto. La carpeta del módulo debe contener las dependencias instaladas definidas en `package.json`, incluyendo `mqtt`, `mongodb` y `dotenv`, entre otras necesarias para la ejecución del bridge.[cite:152][cite:110]

Ejemplo de ejecución local:

```bash
node index.js
```

## Valor documental para derechos de autor

La Dirección Nacional de Derecho de Autor de Colombia admite como soporte de registro de software el código fuente, una descripción detallada del programa y material auxiliar como manuales o documentos técnicos. En este contexto, el módulo `helmo-bridge` y su documentación asociada constituyen evidencia directa de la lógica implementada para la integración TTN–MongoDB dentro del sistema HELMO.[cite:13][cite:14]

El presente `README.md` tiene la función de identificar el módulo, explicar su propósito, contextualizar su rol dentro del sistema y describir sus principales flujos operativos. Junto con `execution-flow.md`, el archivo fuente `index.js` y el archivo `.env.example`, conforma un conjunto documental útil para fines de trazabilidad, mantenimiento y soporte de autoría técnica.[cite:13]

## Autoría

La autoría del módulo `helmo-bridge` y su documentación asociada debe reconocerse a:

- Esteban Eduardo Escárraga Túquerres.
- Johan Felipe Ordoñez Jenoy.
- Dayerli Katerine Tamayo Solarte.
