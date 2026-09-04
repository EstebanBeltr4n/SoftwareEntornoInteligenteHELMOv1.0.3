# HELMO

HELMO es un sistema de monitoreo inteligente de calidad de agua y variables operativas, concebido como una arquitectura integrada de hardware, firmware, comunicaciones LoRaWAN, backend, almacenamiento y servicios web para supervisión remota. El repositorio organiza el contenido técnico del proyecto como una sola obra estructurada, compuesta por módulos de adquisición en campo, servicios de integración y documentación de soporte para trazabilidad técnica y registro de derechos de autor.[file:308][cite:45]

La arquitectura conceptual del sistema muestra una cadena que incluye captación, sensores, unidad de proceso local, red inalámbrica, gateway/servidor IoT, servidor de red TTN/The Things Stack, base de datos y servicio web para consultas, dashboards, alertas y operación. Esa representación coincide con la organización actual del repositorio, donde existen carpetas de `firmware`, `hardware`, `backend`, `integrations/ttn`, `docs` y `legal`, además de archivos raíz como `LICENSE`, `CHANGELOG.md` y este `README.md`.[file:308][file:315][file:316]

## Finalidad del repositorio

Este repositorio tiene una doble finalidad. Por un lado, organiza el desarrollo técnico de HELMO como proyecto de ingeniería; por otro, consolida una base documental y descriptiva útil para un expediente de derechos de autor, donde el código fuente se acompaña de arquitectura, documentación técnica, materiales de soporte y definición de autoría.[web:293][web:286]

El contenido fue estructurado para que cada módulo pueda entenderse individualmente y, al mismo tiempo, como parte de una sola solución integral. Esto permite describir HELMO no solo como conjunto de programas aislados, sino como sistema software-hardware articulado con proyección de servicio SaaS para monitoreo hídrico distribuido.[cite:45][file:308]

## Estructura general

```text
HELMO/
├── backend/
│   ├── api/
│   ├── deploy/
│   └── helmo-bridge/
├── docs/
│   ├── 01_descripcion_general.md
│   ├── 02_arquitectura_del_sistema.md
│   ├── 03_Instalacion_entorno.md
│   ├── 04_manual_tecnico.md
│   └── 05_evidencias_para_registro.md
├── firmware/
├── integrations/
│   └── ttn/
├── legal/
├── .gitignore
├── CHANGELOG.md
├── LICENSE
└── README.md
```

Esta estructura separa claramente las capas de software, documentación, integración de red y soporte legal, lo cual es coherente con prácticas de organización de proyectos embebidos y documentación técnica de software.[web:275][web:291]

## Contenido por carpeta

### `backend/`

Contiene los componentes de servicio digital del sistema, incluyendo API, despliegue e integración intermedia. Esta capa representa parte esencial del enfoque SaaS de HELMO, ya que concentra la lógica de backend, gestión de datos y soporte a la operación remota de la plataforma.[cite:1][cite:45]

### `docs/`

Agrupa los documentos generales del sistema: descripción, arquitectura, instalación del entorno, manual técnico y evidencias para registro. Esta carpeta funciona como columna vertebral documental del repositorio y ayuda a contextualizar el código fuente para fines técnicos y jurídicos.[web:293][file:316]

### `firmware/`

Contiene el software embebido de los nodos de campo, como los módulos dedicados de pH, turbidez y nivel, junto con sus archivos de especificación y payload. Estos nodos implementan la adquisición local, el preprocesamiento y la transmisión LoRaWAN de variables críticas del sistema HELMO.[cite:45][web:223]

### `integrations/ttn/`

Incluye archivos asociados a integración con TTN/The Things Stack, como documentación de payload y decoder uplink. Esta carpeta es importante para la interoperabilidad entre la telemetría LoRaWAN de los nodos y la capa de backend o visualización del sistema.[file:315][cite:45]

### `legal/`

Agrupa documentos jurídicos o de soporte legal del proyecto, tales como autoría, titularidad, alcance documental y notas de versión para registro. Su función es complementar el archivo `LICENSE` ya existente con documentos explicativos orientados a la gestión de derechos de autor del repositorio.[file:314][file:315]

## Naturaleza técnica de HELMO

HELMO combina elementos de sistema embebido, red IoT, arquitectura distribuida y plataforma de monitoreo web. A nivel funcional, integra sensores de pH, turbidez y nivel, transmisión LoRaWAN, almacenamiento de datos, dashboards y alertamiento; a nivel de producto, puede entenderse como base tecnológica para un servicio de monitoreo remoto escalable a múltiples ubicaciones o clientes.[file:308][cite:45]

La arquitectura conceptual también hace referencia a cifrado AES-128, gateway LoRa, servidor IoT, MongoDB/Atlas, Grafana, ngrok y proyección hacia componentes de ML y control operativo. Esto confirma que el alcance del sistema supera el simple envío de datos y abarca una solución digital más amplia de supervisión, análisis y apoyo a operación.[file:308]

## Uso documental para derechos de autor

El presente repositorio fue organizado para que su contenido pueda ser utilizado como soporte técnico y descriptivo del sistema HELMO dentro de un trámite de derechos de autor. Aunque el núcleo de la obra corresponde al software y a su documentación, la inclusión ordenada de arquitectura, hardware, payloads, manuales y material legal mejora la trazabilidad y la delimitación del proyecto como una obra original y estructurada.[web:293][web:286]

## Licencia y restricciones

El archivo `LICENSE` ya define la reserva general de derechos sobre el software, su código fuente, documentación, arquitectura, configuraciones, protocolos y materiales asociados. Este `README.md` no sustituye esa licencia, sino que la complementa describiendo el contenido y el propósito del repositorio dentro de un marco de protección jurídica y técnica.[file:314]

## Autoría

La autoría del sistema HELMO y del contenido técnico-documental del repositorio debe reconocerse a:

- Esteban Eduardo Escárraga Túquerres.
- Johan Felipe Ordoñez Jenoy.
- Dayerli Katerine Tamayo Solarte.
