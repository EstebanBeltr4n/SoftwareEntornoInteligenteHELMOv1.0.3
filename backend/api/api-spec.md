# Especificación funcional - API MongoDB de HELMO

Este documento describe técnicamente el funcionamiento del módulo `backend/api/api-mongo.js`, componente backend encargado de la ingestión HTTP de telemetría y de la consulta autenticada de lecturas almacenadas en MongoDB local dentro del sistema HELMO.[cite:1]

## Finalidad del módulo

La API MongoDB de HELMO cumple una doble función. Por una parte, recibe eventos HTTP tipo webhook enviados por The Things Stack; por otra, ofrece un mecanismo controlado de consulta de datos recientes para herramientas internas, procesos de verificación o módulos de visualización conectados al sistema.[cite:1][cite:45]

## Entorno tecnológico

El módulo está construido en Node.js y utiliza Express para la exposición de rutas HTTP, MongoDB como sistema de persistencia y autenticación básica para la protección del endpoint de consulta. Node.js documenta `process.env` como interfaz estándar de configuración, por lo que la parametrización del servicio debe apoyarse en variables de entorno y no en secretos embebidos directamente en el código fuente.[cite:110][cite:140]

## Flujo de operación

### 1. Carga de configuración

Al iniciar el proceso, el servicio carga parámetros como puerto, credenciales de acceso y cadena de conexión MongoDB desde variables de entorno. Si faltan variables críticas como `API_USERNAME`, `API_PASSWORD` o `MONGO_URL`, el proceso termina para evitar arranques incompletos o inseguros.[cite:1][cite:110]

### 2. Conexión a MongoDB

El módulo crea un cliente `MongoClient`, se conecta a la base de datos definida y deja lista la colección de trabajo `monolito_lecturas` para operaciones posteriores. MongoDB documenta `MongoClient` como el punto de entrada principal para gestionar conexiones y opciones de acceso desde aplicaciones Node.js.[cite:152][cite:162]

### 3. Ruta `GET /`

Esta ruta entrega una respuesta simple con estado del servicio, nombre lógico y puerto activo. Su función es servir como verificación rápida de que el proceso HTTP está en ejecución local.[cite:1]

### 4. Ruta `GET /health`

La ruta de salud permite comprobar que el servicio continúa operativo. Esta convención resulta útil en procesos de monitoreo, scripts de verificación y pruebas de conectividad local.[cite:1]

### 5. Ruta `POST /datos`

La ruta `POST /datos` recibe un objeto JSON desde TTN, extrae el contenido ubicado en `uplink_message.decoded_payload` y valida que exista información sensórica útil antes de continuar. Si no hay `decoded_payload`, la API responde con confirmación del webhook, pero advierte que no hubo persistencia funcional del evento.[cite:1]

Cuando el payload decodificado existe, se construye un documento con:

- identificador del dispositivo;
- marca temporal del mensaje;
- variables sensóricas decodificadas;
- metadatos de radio, incluyendo `fcnt`, `rssi`, `snr` y `gateway`.[cite:1]

Posteriormente, dicho documento se inserta en la colección MongoDB configurada. Tras la inserción, la API devuelve una respuesta JSON con confirmación de éxito e identificador del documento generado.[cite:1]

### 6. Ruta `GET /datos`

La ruta `GET /datos` permite consultar las últimas 100 lecturas almacenadas, ordenadas de forma descendente por fecha. Antes de responder, el servicio valida las credenciales del cliente mediante autenticación HTTP Basic; si estas no coinciden con la configuración establecida, devuelve error 401 y solicita autenticación.[cite:1]

## Estructura del documento persistido

El documento almacenado por la API tiene la siguiente estructura conceptual:

```json
{
  "dispositivo": "identificador_del_nodo",
  "timestamp": "fecha_hora_del_evento",
  "variables": {
    "variable_1": "valor",
    "variable_2": "valor"
  },
  "radio_metadata": {
    "fcnt": "contador_de_trama",
    "rssi": "potencia_recibida",
    "snr": "relacion_senal_ruido",
    "gateway": "identificador_gateway"
  }
}
```

Esta estructura normalizada facilita búsquedas, ordenamiento cronológico, consumo por dashboards, depuración y análisis posterior de telemetría del sistema HELMO.[cite:1][cite:53]

## Seguridad y buenas prácticas

La versión destinada a repositorio y documentación de autoría no debe incluir credenciales reales ni cadenas sensibles dentro del código fuente. Node.js documenta `process.env` como mecanismo estándar para separar configuración y lógica, por lo que el archivo `.env.example` cumple la función de indicar qué variables requiere el servicio sin divulgar datos reservados.[cite:110][cite:141]

## Valor dentro de HELMO

Este módulo complementa al bridge MQTT del proyecto al ofrecer una vía HTTP formal para ingestión y consulta de datos. De esta forma, el backend HELMO no depende de una única interfaz de entrada, sino que combina mecanismos de integración por MQTT y por webhook HTTP según el componente o servicio que interactúe con la plataforma.[cite:1][cite:45]

## Valor para expediente de derechos de autor

El presente documento complementa el código fuente `api-mongo.js` y ayuda a describir funciones, estructura, módulos y finalidad del servicio backend, elementos que la guía procedimental de la DNDA considera pertinentes dentro del soporte documental del registro de software.[cite:129]

## Autoría

La autoría del módulo `backend/api` y de la presente especificación técnica debe reconocerse a:

- Esteban Eduardo Escárraga Túquerres.
- Johan Felipe Ordoñez Jenoy.
- Dayerli Katerine Tamayo Solarte.
