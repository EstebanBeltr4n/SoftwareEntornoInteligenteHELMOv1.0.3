# Especificación funcional - Nodo de nivel PTAP

Este documento describe el comportamiento funcional del firmware `firmware/nodo_nivel/src/nodo_nivel_tanque_ptap.ino`, correspondiente al nodo de nivel del sistema HELMO. Su propósito es complementar el código fuente con una explicación técnica del principio de medición, variables generadas, lógica de clasificación y forma de integración con la red LoRaWAN del proyecto.[cite:129][web:223]

## Finalidad del módulo

El nodo de nivel se diseñó para medir el estado superficial del agua en un tanque o zona operativa de PTAP y traducir esa condición física a una representación digital compacta. El firmware toma una medición de distancia entre el sensor ultrasónico y la superficie del agua, estima la columna útil, calcula un porcentaje relativo y clasifica el estado del nivel para su transmisión dentro de la red HELMO.[web:240][web:243]

El caso de uso considera un punto donde el filtro de arena puede hacerse visible cuando la lámina de agua desciende aproximadamente a 20 cm. Por ello, el software incorpora el umbral `FILTER_VISIBLE_THRESHOLD_CM` como criterio funcional para identificar una condición crítica de nivel.[cite:1]

## Principio de medición

La lógica del nodo usa un sensor tipo HC-SR04 o equivalente, con generación de pulso `TRIG`, captura del tiempo de eco y conversión a distancia mediante la velocidad aproximada del sonido en aire. Este método es el esquema habitual de uso del HC-SR04 con ESP32 para estimar distancias en aplicaciones de nivel y presencia superficial.[web:240][web:243][web:245]

Para mejorar la estabilidad de la medición, el firmware no utiliza una sola lectura aislada, sino varias muestras válidas seguidas de un filtrado por mediana. Este enfoque reduce el impacto de rebotes, ruido y lecturas fuera de rango, lo cual es especialmente útil en superficies no perfectamente estables o con perturbaciones leves.[web:240]

## Flujo de operación

1. Energizar la etapa de sensado mediante un MOSFET de control.[cite:1]
2. Ejecutar varias lecturas ultrasónicas del nivel superficial.[web:243]
3. Filtrar las muestras por mediana para obtener una distancia representativa.[web:240]
4. Calcular la columna de agua útil respecto al plano de referencia configurado.[cite:1]
5. Calcular el porcentaje relativo del nivel.[cite:1]
6. Determinar si el filtro es visible según el umbral de 20 cm.[cite:1]
7. Clasificar el estado del nivel en categorías operativas.[cite:1]
8. Empaquetar la información en una trama binaria de 6 bytes para envío LoRaWAN.[web:252][web:253]

## Estados definidos

El firmware define cuatro estados lógicos de nivel:

- `0`: crítico_bajo.
- `1`: bajo.
- `2`: normal.
- `3`: alto.

La condición `crítico_bajo` se activa cuando el filtro es visible o cuando el porcentaje relativo del nivel cae por debajo del umbral mínimo definido por el algoritmo. Esta lógica fue incorporada para reflejar mejor la situación operativa descrita para el tanque PTAP dentro de HELMO.[cite:1]

## Variables de salida

| Variable | Descripción |
|---|---|
| `distanceCm` | Distancia entre el sensor y la superficie del agua.[web:243] |
| `waterColumnCm` | Altura útil de agua calculada desde la referencia geométrica del punto.[cite:1] |
| `levelPercent` | Nivel relativo expresado como porcentaje del rango configurado.[cite:1] |
| `filterVisible` | Indicador booleano de visibilidad del lecho filtrante.[cite:1] |
| `levelState` | Clasificación categórica del estado del nivel.[cite:1] |

## Integración con HELMO

El nodo se integra a la arquitectura HELMO como un componente de campo que produce una medición puntual lista para transporte LoRaWAN. Su salida puede ser consumida por un decoder uplink en TTN/TTS y posteriormente por el backend, la base de datos y la capa de visualización del sistema, manteniendo la separación por capas entre adquisición, comunicaciones, persistencia y monitoreo.[cite:45][cite:1]

## Estructura del firmware

El archivo fuente principal reside en `src/`, mientras que la carpeta `include/` puede alojar en el futuro archivos como `level_config.h`, `pins.h` o `payload_level.h`. Esta organización coincide con el modelo recomendado para proyectos embebidos estructurados en PlatformIO y facilita mantenibilidad, modularidad y trazabilidad del código fuente.[web:29][web:198][web:256]

## Valor documental para el registro

La presente especificación funcional sirve como material auxiliar descriptivo del programa y complementa el archivo fuente `.ino`. La guía de registro de software ante la DNDA contempla tanto código fuente como documentación técnica de apoyo, por lo que este documento fortalece la identificación del nodo de nivel como unidad diferenciada dentro de la obra software HELMO.[cite:129]

## Autoría

La autoría del firmware del nodo de nivel y de esta especificación técnica debe reconocerse a:

- Esteban Eduardo Escárraga Túquerres.
- Johan Felipe Ordoñez Jenoy.
- Dayerli Katerine Tamayo Solarte.
