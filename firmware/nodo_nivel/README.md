# HELMO - Firmware del nodo de nivel

El módulo `firmware/nodo_nivel` corresponde al firmware embebido del nodo de nivel del sistema HELMO, diseñado para integrarse a la arquitectura LoRaWAN del proyecto como una unidad de campo especializada en monitoreo superficial del agua dentro de un tanque o compartimiento de la PTAP. Su función es medir la distancia entre el sensor y la lámina de agua, estimar la columna útil, clasificar el estado del nivel y transmitir una carga útil compacta hacia la red HELMO para su posterior decodificación, almacenamiento y visualización.[web:223][web:254]

La versión actual del nodo fue ajustada para mantener coherencia con la arquitectura general de HELMO y con la lógica del nodo multisensor, reutilizando la misma filosofía de diseño: adquisición local, preprocesamiento embebido, empaquetado binario compacto y transmisión LoRaWAN mediante la base de software Heltec para la WiFi LoRa 32 V3. Heltec documenta soporte LoRaWAN para esta placa sobre Arduino, y la estructura `src/` más `include/` coincide con las prácticas habituales de organización de firmware embebido en PlatformIO y entornos equivalentes.[web:223][web:29][web:198][web:200]

## Estructura del módulo

```text
firmware/
└── nodo_nivel/
    ├── include/
    ├── src/
    │   └── nodo_nivel_tanque_ptap.ino
    ├── README.md
    ├── level-spec.md
    └── payload-level.md
```

La carpeta `src/` contiene el archivo fuente principal del nodo, mientras que `include/` queda reservada para encabezados del proyecto y futuras definiciones compartidas. Esta distribución es consistente con la documentación de PlatformIO, que ubica el código principal en `src` y los archivos de cabecera en `include`.[web:29][web:38][web:256]

## Función del nodo dentro de HELMO

El nodo de nivel forma parte de la capa firmware de adquisición distribuida del sistema HELMO. Su papel es capturar una variable hidráulica específica del punto monitoreado y convertirla en un payload binario que pueda circular por la red LoRaWAN hacia TTN/TTS, backend, base de datos y servicios de visualización como Grafana dentro de la arquitectura general del proyecto.[cite:45][cite:1]

En el caso descrito para PTAP, el nodo se instala sobre un punto donde el nivel del agua puede disminuir hasta dejar visible el lecho de arena a aproximadamente 20 cm. Por ello, además de medir distancia y porcentaje relativo, el firmware incorpora una lógica de clasificación para detectar condiciones críticas de operación asociadas a visibilidad del filtro.[cite:1]

## Archivo principal

El archivo fuente principal del módulo es:

```text
firmware/nodo_nivel/src/nodo_nivel_tanque_ptap.ino
```

Este archivo contiene la lógica completa del nodo: inicialización del hardware, lectura ultrasónica, filtrado por mediana, cálculo de variables derivadas, generación del payload y control del ciclo LoRaWAN. Como pieza de software embebido, constituye la evidencia principal del módulo para fines de documentación técnica y registro de derechos de autor.[web:240][web:243][cite:129]

## Variables funcionales del firmware

La versión actual del nodo genera y trabaja con las siguientes variables lógicas:

- `distanceCm`: distancia entre el sensor y la superficie del agua;
- `waterColumnCm`: columna de agua estimada respecto al plano de referencia configurado;
- `levelPercent`: porcentaje relativo de nivel;
- `filterVisible`: bandera lógica de visibilidad del filtro;
- `levelState`: clasificación categórica del estado del nivel.[cite:1]

## Integración con la red

La trama generada por el nodo tiene tamaño fijo de 6 bytes y está diseñada para facilitar su decodificación en TTN/TTS o en el backend del proyecto. El uso de serialización compacta y orden explícito de bytes es una práctica coherente con payloads LoRaWAN de telemetría, donde se prioriza tamaño reducido y reconstrucción determinística de los campos en el decoder.[web:252][web:253][web:255]

## Autoría

La autoría del firmware del nodo de nivel y de la documentación asociada debe reconocerse a:

- Esteban Eduardo Escárraga Túquerres.
- Johan Felipe Ordoñez Jenoy.
- Dayerli Katerine Tamayo Solarte.
