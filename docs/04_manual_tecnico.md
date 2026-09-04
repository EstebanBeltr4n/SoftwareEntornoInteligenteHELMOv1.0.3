# HELMO - Manual técnico

Este manual técnico describe de forma resumida la estructura funcional y operativa de HELMO como sistema de monitoreo hídrico distribuido. Su objetivo es servir como documento de apoyo del repositorio, relacionando hardware, firmware, comunicaciones, servicios IoT y capa web bajo una sola descripción técnica del sistema.[file:308][cite:45]

## Alcance del manual

El manual cubre la visión general de los nodos de campo, la lógica de transmisión, la infraestructura de recepción y la capa de servicio web. También incluye una lectura de HELMO como plataforma tecnológica escalable, susceptible de operar como solución SaaS para monitoreo remoto de variables hídricas y eventos operativos.[file:308][cite:45]

## Nodos de campo

HELMO emplea nodos de adquisición dedicados o especializados, entre ellos nodos para pH, turbidez y nivel. Estos módulos, implementados sobre Heltec WiFi LoRa 32 V3, realizan lectura de sensores, procesamiento local, empaquetado de payload y transmisión LoRaWAN hacia la infraestructura superior del sistema.[cite:45][web:223]

Cada nodo mantiene una estructura documental compuesta por archivo fuente `.ino`, README funcional, especificación del nodo y especificación del payload. Esta práctica no solo favorece mantenimiento, sino que también fortalece la trazabilidad del sistema dentro del repositorio para derechos de autor.[cite:45][web:293]

## Flujo de información

El diagrama conceptual de HELMO describe un flujo que parte de la captación, pasa por sensores y unidad de proceso local, cruza una red inalámbrica y llega a servicios IoT, bases de datos y dashboards. Ese flujo desemboca en consultas de variables como pH, turbidez, nivel y eventos, además de alertas, notificaciones y eventual interacción operativa desde oficina o PTAP.[file:308]

En términos funcionales, el flujo puede resumirse así:

```text
Sensor -> Nodo LoRaWAN -> Gateway/TTN/TTS -> Backend -> Base de datos -> Dashboard / Alertas / Operación
```

## Capa SaaS y servicio digital

La dimensión SaaS de HELMO aparece en la capa de servicio web y gestión centralizada. Allí se concentran funcionalidades de frontend, dashboards, consultas históricas, alertas y potencial analítica, lo que permite ofrecer el sistema no solo como conjunto de nodos físicos sino como plataforma digital continua para seguimiento hídrico.[file:308][cite:45]

Desde el punto de vista técnico y de negocio, esta capa centralizada permite administrar múltiples puntos de monitoreo y convertir la arquitectura en una oferta replicable. La modularidad del sistema también facilita personalizar despliegues por cliente, ubicación, planta o caso de uso sin rediseñar la base tecnológica completa.[cite:1][file:308]

## Operación general

La operación técnica del sistema implica, como mínimo, cuatro tareas recurrentes:

- mantenimiento y verificación de nodos de campo;
- supervisión de conectividad LoRa/LoRaWAN y gateway;
- gestión de backend y base de datos;
- seguimiento de dashboards, alertas y eventos operativos.[file:308][cite:45]

Cuando el sistema se proyecta como SaaS, estas tareas pueden convertirse en servicios recurrentes de monitoreo, soporte, configuración, administración de usuarios y mantenimiento de infraestructura digital. Esto fortalece la lectura de HELMO como producto tecnológico más allá del prototipo aislado.[file:308]

## Escalabilidad

La misma base técnica documentada en HELMO permite agregar nuevos nodos de monitoreo en otros lugares de la red LoRa. Esto es posible porque la arquitectura desacopla sensado, transmisión y servicios centrales, y porque los nodos se han organizado en módulos documentados que pueden adaptarse por variable o ubicación.[cite:45][cite:1]

## Valor documental

Este manual sirve como material técnico de apoyo dentro del repositorio y ayuda a explicar de manera integrada cómo funciona HELMO. Para fines de derechos de autor, acompaña el código fuente con una descripción sistemática de la solución desarrollada y del modo en que sus componentes cooperan dentro del sistema completo.[web:293][web:286]

## Autoría

La autoría del manual técnico de HELMO debe reconocerse a:

- Esteban Eduardo Escárraga Túquerres.
- Johan Felipe Ordoñez Jenoy.
- Dayerli Katerine Tamayo Solarte.
