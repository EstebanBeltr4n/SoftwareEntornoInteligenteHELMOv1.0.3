# HELMO - Evidencias para registro

Este documento resume los tipos de evidencia técnica que acompañan el repositorio HELMO y que pueden servir como soporte del expediente de derechos de autor. Su función es identificar qué materiales existen, qué rol cumplen dentro de la descripción de la obra software y cómo se relacionan con la arquitectura general del sistema.[web:293][web:286]

## Código fuente

El repositorio contiene código fuente de firmware organizado por nodos, incluyendo módulos dedicados como `nodo_ph`, `nodo_turbidez` y `nodo_nivel`, junto con sus especificaciones técnicas y de payload. Este conjunto constituye el núcleo principal de la obra software en su componente embebido y demuestra diseño modular, estructura de telemetría y lógica LoRaWAN.[cite:45]

## Documentación técnica

La carpeta `docs/` contiene descripciones generales, arquitectura, instalación de entorno, manual técnico y soporte descriptivo del sistema. Estos documentos ayudan a interpretar el funcionamiento global de HELMO y a vincular los módulos firmware, hardware y servicio web dentro de una sola narrativa técnica.[web:293][cite:45]

## Arquitectura conceptual

Se adjunta un diagrama conceptual de arquitectura que muestra la relación entre captación, sensores, unidad de proceso local, red inalámbrica, servidor IoT/gateway, base de datos, servidor de red TTN/TTS, servicio web y operación en oficina o PTAP. Ese diagrama aporta evidencia visual de la estructura funcional del sistema y de su alcance como solución integral de monitoreo hídrico.[file:308]

## Valor del enfoque SaaS

El componente SaaS de HELMO se expresa en la existencia de una capa de servicio web, dashboards, consultas, alertas, backend y base de datos. Esto significa que la obra no se agota en un firmware aislado, sino que se proyecta como una plataforma digital de supervisión y gestión remota, con posibilidad de escalar a múltiples puntos de monitoreo y clientes o usuarios institucionales.[file:308][cite:45]

## Autoría

La autoría del sistema HELMO y de la documentación asociada debe reconocerse a:

- Esteban Eduardo Escárraga Túquerres.
- Johan Felipe Ordoñez Jenoy.
- Dayerli Katerine Tamayo Solarte.
