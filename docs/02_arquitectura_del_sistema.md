# HELMO - Arquitectura del sistema

La arquitectura de HELMO corresponde a un sistema distribuido de adquisición, transporte, almacenamiento y visualización de datos hídricos, compuesto por una capa de sensado en campo, una capa de transmisión inalámbrica, una capa de servicios IoT y una capa de acceso web para consulta y operación. El diagrama conceptual adjunto muestra expresamente esta separación por bloques y ubica el flujo desde captación y sensores hasta servicios web, alertas y operación desde oficina o PTAP.[file:308]

## Vista general por capas

La arquitectura puede entenderse en cinco capas funcionales:

1. Captación y entorno físico.
2. Sensado y procesamiento local.
3. Comunicación inalámbrica LoRa/LoRaWAN.
4. Servicios IoT, backend y persistencia.
5. Visualización, alertas y operación web.[file:308]

Esta estructura por capas también coincide con la forma en que se ha venido organizando el repositorio HELMO, separando firmware, hardware, documentación técnica y evidencias. Desde el punto de vista del diseño de software y de producto, esta modularidad favorece mantenibilidad, escalabilidad y posibilidad de desplegar nuevos nodos o nuevos clientes sobre una misma base tecnológica.[cite:1][cite:45]

## Capa física de captación y sensado

El diagrama conceptual ubica una primera etapa de captación y una segunda etapa de sensores. En esta última se identifican específicamente sensor de turbidez para detección de agua con barro, sensor de pH para calidad del agua en cabecera municipal y sensor de nivel para tanque de almacenamiento con medición de estados alto, medio y bajo.[file:308]

En el repositorio técnico de HELMO, estos elementos se reflejan en la separación por nodos dedicados como `nodo_turbidez`, `nodo_ph` y `nodo_nivel`. Esa correspondencia entre arquitectura conceptual y organización del código ayuda a demostrar que el sistema fue diseñado con modularidad funcional, donde cada nodo ejecuta una tarea de adquisición y serialización específica antes de transmitir telemetría al resto de la red.[cite:45]

## Procesamiento local y actuación

La arquitectura conceptual incorpora una “unidad de proceso local” con varios módulos LoRa y una capa de actuadores, incluyendo válvula de cierre automático y sirena o luz de alarma local. Eso indica que HELMO no se concibe solo como sistema pasivo de visualización, sino como plataforma capaz de apoyar respuesta operativa local ante condiciones observadas en campo.[file:308]

Aunque el lazo de control se presenta como “abierto” y opcional en el diagrama, el modelo arquitectónico deja preparada la posibilidad de que las decisiones derivadas de los datos activen respuestas locales o asistidas. Esta característica incrementa el valor del sistema como solución SaaS/IoT, porque lo acerca a esquemas de monitoreo operativo con alertas, intervención humana y proyección futura hacia analítica o automatización más avanzada.[file:308][cite:45]

## Red inalámbrica y conectividad

La comunicación entre el bloque de campo y los servicios centrales se apoya en una red inalámbrica de largo alcance. El diagrama indica el uso de un módulo LoRaWAN 3, referencia a cifrado digital AES-128 y un alcance estimado de 15 a 20 km, así como un vínculo de Internet 4G para conectividad hacia los servicios superiores.[file:308]

Esa descripción es coherente con el uso de LoRaWAN y con la integración con The Things Stack/TTN que aparece tanto en el diagrama como en los módulos firmware del repositorio. Además, la plataforma Heltec WiFi LoRa 32 V3 documenta soporte LoRaWAN sobre Arduino, lo que respalda la base tecnológica usada en los nodos de HELMO para este tipo de transmisión distribuida. [web:223][web:244][file:308]

## Capa de servicios IoT y datos

El diagrama conceptual incluye explícitamente un “Servidor IoT/Gateway” con gateway LoRa, servidor IoT y base de datos MongoDB/Compass/Atlas. También se muestra un “Servidor de red (TTN - The Things Stack)” y un “Servicio web” asociado a frontend, Grafana y ngrok.[file:308]

Esto define una arquitectura híbrida donde la telemetría capturada en campo puede ser recibida por la red LoRaWAN, procesada por TTN/TTS, derivada a backend y almacenada en una base de datos para consulta y paneles web. Dentro del trabajo previo de HELMO también se ha mencionado el uso de Node.js, MongoDB, Grafana y monitoreo de procesos, lo cual encaja con esta arquitectura SaaS de backend y visualización centralizada.[cite:1][file:308]

## Capa de visualización y servicio SaaS

La capa superior del sistema corresponde al acceso web, dashboards, consultas y alertas para usuarios operadores. El diagrama muestra consultas sobre pH, turbidez, nivel y eventos, así como alertas, actuadores, notificaciones, MongoDB y ML, conectados al servicio web y al operario de PTAP u oficina.[file:308]

Desde una visión SaaS, esta capa constituye el núcleo del valor recurrente del sistema, porque es donde se entregan funcionalidades de plataforma: visualización remota, gestión histórica, alertas, acceso por usuario, analítica y soporte a decisión. Bajo esta lógica, HELMO puede crecer desde una solución piloto o académica hacia una oferta tecnológica con panel centralizado, múltiples nodos por cliente y evolución futura hacia servicios de inteligencia operacional.[cite:45][file:308]

## Escalabilidad de la arquitectura

La arquitectura de HELMO es escalable porque separa nodos de campo, transporte de datos y servicios centrales. Esto permite agregar nuevos nodos de monitoreo en otros lugares de la red LoRa, reutilizando la misma base documental, el mismo patrón de firmware y la misma infraestructura backend, siempre que se administren adecuadamente credenciales, payloads y parametrización por sitio.[cite:45][cite:1]

Esa capacidad de expansión es clave tanto para el diseño técnico como para un eventual modelo de negocio, ya que un mismo servicio central puede atender varios despliegues físicos. Por eso la arquitectura no solo tiene valor técnico, sino también valor estratégico como base de una plataforma de monitoreo hídrico distribuido.[file:308]

## Relación con el repositorio

Dentro del repositorio para derechos de autor, este documento cumple la función de describir la estructura macro del sistema y vincular firmware, hardware, red, backend y servicio web en una sola narrativa técnica. Esto ayuda a que los módulos individuales no se vean como piezas aisladas, sino como partes de una arquitectura software-hardware integrada denominada HELMO.[web:293][web:286]

## Autoría

La autoría de la arquitectura del sistema HELMO y de la documentación asociada debe reconocerse a:

- Esteban Eduardo Escárraga Túquerres.
- Johan Felipe Ordoñez Jenoy.
- Dayerli Katerine Tamayo Solarte.
