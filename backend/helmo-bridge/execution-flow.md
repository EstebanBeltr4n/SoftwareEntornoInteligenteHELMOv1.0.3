# Flujo de ejecución - HELMO Bridge

Este documento describe el flujo operativo del módulo `backend/helmo-bridge/index.js`, encargado de integrar la mensajería MQTT de The Things Stack con la capa de persistencia de MongoDB utilizada en el sistema HELMO. El bridge forma parte del backend desplegado sobre servidor Debian 12 y se articula con la arquitectura telemétrica ya operativa del proyecto.[cite:1][cite:45]

## Visión general del flujo

El bridge se ejecuta como un servicio en segundo plano que consume telemetría decodificada desde TTN. Su función consiste en convertir cada mensaje recibido en un documento estructurado y persistirlo en una base local, con réplica opcional hacia MongoDB Atlas cuando el servicio cloud está disponible.[cite:1][cite:48]

## Secuencia de ejecución

### 1. Inicialización del servicio

Al iniciar el proceso, el script carga configuración desde variables de entorno y prepara los clientes de conexión a MongoDB local, MongoDB Atlas y MQTT. Node.js documenta el uso de `process.env` para este tipo de configuración sensible, y el driver oficial de MongoDB permite construir clientes `MongoClient` con opciones de timeout, conexión y recuperación adecuadas para procesos backend persistentes.[cite:110][cite:152][cite:162]

### 2. Conexión a MongoDB local

El bridge establece primero conexión con la base de datos local, la cual constituye el almacenamiento principal de operación del sistema. Esta base permite que HELMO continúe registrando telemetría aun cuando existan fallos temporales de internet o indisponibilidad de la base cloud.[cite:1]

### 3. Conexión inicial a MongoDB Atlas

Posteriormente, el servicio intenta abrir conexión con MongoDB Atlas. Si esta conexión se establece correctamente, queda habilitada la replicación de documentos en la nube; si falla, el sistema conserva la operación local y marca Atlas como no disponible sin detener el proceso completo.[cite:1]

### 4. Reintento periódico de Atlas

Cuando Atlas no se encuentra disponible, el módulo activa un mecanismo de reintento periódico. Esta estrategia permite recuperar automáticamente la conectividad cloud sin intervención manual, manteniendo el sistema en operación continua con almacenamiento local mientras se restablece la conectividad externa.[cite:1]

### 5. Conexión al broker MQTT de TTN

Con la persistencia local lista, el bridge se conecta al broker MQTT de The Things Stack usando las credenciales definidas para la aplicación HELMO. A continuación, se suscribe al tópico de uplinks correspondiente, quedando a la espera de nueva telemetría proveniente de los nodos del sistema.[cite:47]

### 6. Recepción de mensajes MQTT

Cada vez que llega un mensaje al tópico suscrito, el bridge convierte el payload MQTT a texto y lo interpreta como JSON. Luego extrae el objeto `uplink_message.decoded_payload`, generado previamente en TTN por el formatter uplink configurado para la aplicación.[cite:48][cite:70]

### 7. Validación del contenido

Si el mensaje recibido no contiene `decoded_payload`, el bridge genera un aviso en consola y descarta el evento sin persistencia. Si el campo sí existe, continúa la construcción del documento telemétrico a partir de los datos recibidos.[cite:48]

### 8. Construcción del documento

El documento generado por el bridge contiene:

- `dispositivo`: identificador del nodo emisor.
- `timestamp`: fecha/hora del evento.
- `variables`: conjunto de variables decodificadas del sensor o nodo.
- `radio_metadata`: metadatos del enlace inalámbrico, como `fcnt`, `rssi`, `snr` y `gateway`.[cite:48]

Esta estructura normaliza la telemetría y la deja lista para su uso por la API local, consultas en MongoDB, sincronización cloud y visualización posterior en Grafana.[cite:1][cite:53]

### 9. Persistencia dual

El bridge intenta guardar primero el documento en MongoDB local. Después, si MongoDB Atlas está disponible, replica el mismo documento en la base cloud. Si el guardado cloud falla, el módulo desactiva temporalmente Atlas y continúa trabajando solo con el almacenamiento local, reanudando la replicación cuando un reintento futuro restablezca la conexión.[cite:1][cite:48]

### 10. Registro de eventos y trazabilidad

Durante la ejecución, el módulo imprime en consola información operativa sobre la conexión a MongoDB, estado del broker MQTT, resultados de inserción y valores asociados a cada mensaje procesado. Esto facilita la verificación funcional, el diagnóstico de errores y la trazabilidad de la telemetría en tiempo real.[cite:47][cite:48]

## Importancia dentro de HELMO

El bridge constituye una pieza central del backend HELMO porque une la capa de comunicaciones IoT con la capa de almacenamiento persistente. Sin este módulo, la telemetría ya decodificada por TTN no podría integrarse de forma estructurada con el entorno local de operación del sistema ni con la capa de respaldo cloud asociada al proyecto.[cite:1][cite:45]

Además, su diseño con almacenamiento dual y operación degradada fortalece la robustez del sistema en escenarios reales de monitoreo de calidad de agua, donde la continuidad del registro local es prioritaria. Esta lógica convierte al bridge en un módulo funcionalmente diferenciable y claramente identificable dentro de la obra software HELMO.[cite:1][cite:91]

## Valor para registro de software

La documentación del flujo de ejecución complementa el código fuente del bridge y permite describir de manera suficientemente detallada los procedimientos internos del programa, lo que resulta útil para expedientes de registro de software. La DNDA acepta como soporte tanto el código fuente como la descripción técnica del funcionamiento del programa y material auxiliar de apoyo.[cite:13][cite:14]

## Autoría

La autoría del módulo `helmo-bridge` y de la presente descripción técnica debe reconocerse a:

- Esteban Eduardo Escárraga Túquerres.
- Johan Felipe Ordoñez Jenoy.
- Dayerli Katerine Tamayo Solarte.
