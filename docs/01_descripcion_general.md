# HELMO - Descripción general del sistema

HELMO es un sistema inteligente de monitoreo hídrico orientado a la supervisión remota de variables críticas de calidad y operación del agua, con énfasis en contextos de captación, tratamiento, almacenamiento y seguimiento distribuido mediante redes de baja potencia. A nivel funcional, integra nodos de sensado, transmisión LoRaWAN, servicios de backend, base de datos, visualización web y alertamiento para apoyar decisiones operativas y trazabilidad del recurso hídrico.[file:308][cite:45]

La arquitectura conceptual adjunta del proyecto muestra una cadena completa que va desde la captación y los sensores hasta una unidad de proceso local, la comunicación inalámbrica, un gateway/servidor IoT y un servicio web para consulta y monitoreo. En esa arquitectura se identifican explícitamente sensores de turbidez, pH y nivel, una red inalámbrica con alcance estimado de 15 a 20 km, un gateway/servidor IoT, base de datos MongoDB/Compass Atlas, TTN/The Things Stack y un servicio web con dashboard y alertas.[file:308]

## Naturaleza del sistema

HELMO puede describirse como un sistema ciberfísico de monitoreo hídrico con componentes embebidos, red de comunicaciones IoT y servicios digitales de supervisión. Desde la perspectiva de producto y modelo de explotación, también puede evolucionar a un servicio tipo SaaS, en el que la infraestructura web, base de datos, paneles, alertas y analítica se ofrecen como plataforma de monitoreo para operadores, plantas, oficinas o aliados institucionales.[cite:45][file:308]

Bajo ese enfoque, el valor del sistema no se limita al hardware de campo. También incluye la capa software compuesta por firmware de nodos, red LoRa, integración con TTN/TTS, backend, persistencia, dashboards, notificaciones y proyección hacia componentes de aprendizaje automático y control operativo, ya sugeridos en la arquitectura conceptual mediante referencias a ML y lazo de control abierto.[file:308]

## Problema que aborda

El sistema busca responder a necesidades de vigilancia continua sobre variables de calidad del agua y estado operativo en entornos donde el monitoreo manual resulta lento, discontinuo o costoso. La arquitectura adjunta deja ver un caso aplicado a una PTAP en Almaguer, Cauca, donde la supervisión se conecta con captación, sensores locales, enlaces inalámbricos, consulta de variables y eventual interacción operativa desde oficina o entorno institucional.[file:308]

En este contexto, HELMO aporta captura periódica de información, reducción de dependencia del muestreo puramente manual, mayor trazabilidad histórica y posibilidad de generar alertas tempranas. Esto es especialmente relevante cuando se monitorean variables como pH, turbidez, nivel y eventos operativos distribuidos a través de una red de nodos especializados.[file:308][cite:45]

## Componentes principales

A partir del repositorio y de la arquitectura conceptual, HELMO puede describirse mediante los siguientes bloques principales:

- Nodos de sensado dedicados o multisensor, como `nodo_ph`, `nodo_turbidez` y `nodo_nivel`, desarrollados sobre Heltec WiFi LoRa 32 V3 y lógica LoRaWAN OTAA.[cite:45][web:223]
- Subsistema local de proceso y actuación, incluyendo lógica de control, alarmas, válvulas y procesamiento de variables en sitio.[file:308]
- Capa de comunicaciones LoRa/LoRaWAN para telemetría de largo alcance y bajo consumo, asociada a gateway e integración con TTN/The Things Stack.[file:308][web:244]
- Backend, base de datos y servicio web para persistencia, consulta, visualización y alertamiento, con referencias explícitas a MongoDB, Grafana, ngrok y paneles web dentro del proyecto HELMO.[file:308][cite:1]

## Enfoque SaaS

Como servicio SaaS, HELMO puede estructurarse como una plataforma de monitoreo por suscripción o por cliente institucional, donde la infraestructura digital centralizada administra múltiples nodos, ubicaciones, usuarios y métricas. Bajo ese enfoque, los activos del sistema incluyen no solo dispositivos y firmware, sino también la arquitectura lógica, la operación en nube, el modelo de datos, los paneles, las notificaciones y la eventual analítica predictiva aplicada a la gestión del agua.[cite:45][file:308]

Esto permite plantear una oferta tecnológica escalable: una misma base software puede servir a diferentes plantas, acueductos rurales, operadores comunitarios o aliados públicos, mientras cada despliegue conserva sus propios nodos, ubicaciones, credenciales y parámetros de operación. Esa escalabilidad es coherente con la modularidad del repositorio, que ya separa firmware por nodo, documentación técnica y soporte de hardware por subsistema.[cite:1][cite:45]

## Valor documental para el repositorio

Este documento sirve como descripción general del sistema dentro del repositorio HELMO y ayuda a contextualizar el resto de archivos técnicos. Para fines de derechos de autor, complementa el código fuente con una explicación funcional y arquitectónica del sistema, delimitando alcance, propósito y relación entre sus diferentes módulos.[web:293][web:286]

## Autoría

La autoría del sistema HELMO y de la documentación asociada debe reconocerse a:

- Esteban Eduardo Escárraga Túquerres.
- Johan Felipe Ordoñez Jenoy.
- Dayerli Katerine Tamayo Solarte.
