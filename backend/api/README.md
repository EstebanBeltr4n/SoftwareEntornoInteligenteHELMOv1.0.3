# HELMO API MongoDB

El módulo `backend/api` contiene el servicio HTTP encargado de recibir webhooks enviados por The Things Stack, extraer telemetría decodificada desde `decoded_payload`, almacenarla en MongoDB local y exponer un endpoint autenticado para consulta de datos recientes del sistema HELMO.[cite:1]

Este componente está desarrollado en Node.js con Express y MongoDB, y forma parte de la capa backend del proyecto. Su propósito es ofrecer una interfaz HTTP estructurada para ingestión y consulta de telemetría, complementando el bridge MQTT–MongoDB y facilitando la integración con herramientas de visualización, depuración o consumo interno del sistema.[cite:1][cite:45]

## Archivo principal

```text
backend/api/api-mongo.js
```

El archivo principal implementa la lógica de arranque del servidor, conexión a MongoDB, validación de variables de entorno, recepción de datos vía `POST /datos`, consulta autenticada vía `GET /datos` y rutas de verificación como `/` y `/health`. Node.js documenta `process.env` como la interfaz estándar para configuración sensible, por lo que el módulo debe operar con variables externas y no con credenciales incrustadas en el código fuente.[cite:110][cite:141]

## Estructura sugerida del módulo

```text
backend/
└── api/
    ├── api-mongo.js
    ├── package.json
    ├── .env.example
    ├── .gitignore
    ├── README.md
    └── api-spec.md
```

Esta estructura deja el módulo autocontenido, separa la configuración sensible del código fuente y mejora la trazabilidad del componente dentro del repositorio general de HELMO. También resulta adecuada para acompañar un expediente de registro de software con código fuente y documentos descriptivos del programa.[cite:129]

## Funciones del módulo

- Recibir peticiones HTTP `POST` con payloads JSON provenientes de TTN.[cite:1]
- Extraer el contenido de `uplink_message.decoded_payload` desde el cuerpo recibido.[cite:1]
- Construir un documento normalizado con identificador del dispositivo, fecha/hora, variables y metadatos de radio.[cite:1]
- Almacenar cada documento en MongoDB local.[cite:1]
- Proteger la consulta de datos mediante autenticación HTTP Basic en `GET /datos`.[cite:1]
- Exponer rutas de verificación operativa para estado general y salud del servicio.[cite:1]

## Variables de entorno

La configuración del servicio debe mantenerse fuera del código fuente. La documentación oficial de Node.js indica que las variables de entorno están disponibles mediante `process.env`, y además pueden cargarse desde archivos `.env` mediante mecanismos nativos o herramientas complementarias.[cite:110][cite:127][cite:145]

El archivo `.env.example` adjunto documenta las variables requeridas sin incluir credenciales reales. El archivo `.env` usado en ejecución no debe publicarse en el repositorio y debe quedar excluido mediante `.gitignore`.[cite:110][cite:141]

## Endpoints implementados

| Método | Ruta | Función |
|---|---|---|
| `GET` | `/` | Devuelve estado básico del servicio y puerto configurado.[cite:1] |
| `GET` | `/health` | Devuelve confirmación de disponibilidad operativa.[cite:1] |
| `POST` | `/datos` | Recibe telemetría desde TTN y la almacena en MongoDB.[cite:1] |
| `GET` | `/datos` | Retorna las últimas 100 lecturas, protegido con autenticación Basic.[cite:1] |

## Estructura del documento almacenado

Cada lectura persistida por el servicio responde a una estructura lógica similar a la siguiente:

| Campo | Descripción |
|---|---|
| `dispositivo` | Identificador del nodo emisor recibido desde TTN.[cite:1] |
| `timestamp` | Fecha y hora del mensaje, obtenida de `received_at` o generada localmente.[cite:1] |
| `variables` | Objeto con las variables decodificadas del nodo o sensor.[cite:1] |
| `radio_metadata.fcnt` | Contador de trama uplink.[cite:1] |
| `radio_metadata.rssi` | Intensidad de señal recibida.[cite:1] |
| `radio_metadata.snr` | Relación señal-ruido del paquete recibido.[cite:1] |
| `radio_metadata.gateway` | Identificador del gateway asociado al mensaje.[cite:1] |

## Ejecución básica

Con las dependencias instaladas y el archivo `.env` correctamente configurado, el módulo puede ejecutarse localmente mediante Node.js. Los valores de puerto, usuario, contraseña y conexión Mongo deben definirse fuera del archivo fuente para preservar seguridad y mantenibilidad.[cite:110][cite:141]

Ejemplo de ejecución local:

```bash
node api-mongo.js
```

## Valor documental para registro de software

La guía procedimental de registro ante la DNDA señala que el software puede acompañarse de código fuente, descripción del programa y material auxiliar donde se especifiquen funciones, estructura, módulos, finalidad y modo de operación. En ese contexto, este `README.md` cumple la función de describir el módulo backend API de HELMO, su estructura y su finalidad dentro del sistema general.[cite:129]

## Autoría

La autoría del módulo `backend/api` y de la documentación asociada debe reconocerse a:

- Esteban Eduardo Escárraga Túquerres.
- Johan Felipe Ordoñez Jenoy.
- Dayerli Katerine Tamayo Solarte.
