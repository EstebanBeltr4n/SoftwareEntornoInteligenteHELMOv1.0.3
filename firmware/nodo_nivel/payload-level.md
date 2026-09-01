# Especificación de payload - Nodo de nivel HELMO

Este documento describe la estructura del payload generado por `firmware/nodo_nivel/src/nodo_nivel_tanque_ptap.ino` para su transmisión dentro de la red HELMO. La trama fue diseñada para ser compacta, determinística y fácilmente decodificable en TTN/TTS o en cualquier componente backend que reconstruya variables de nivel a partir de un arreglo de bytes recibido por LoRaWAN.[web:252][web:254][cite:45]

## Tamaño y propósito

El payload del nodo de nivel tiene tamaño fijo de 6 bytes. Esta decisión reduce sobrecarga y facilita una interpretación estable del mensaje en el decoder uplink, manteniendo una representación suficiente de la variable hidráulica principal y de su estado operativo.[web:252][web:255]

## Estructura binaria

| Byte | Campo | Tipo | Escalado / codificación | Descripción |
|---|---|---|---|---|
| 0-1 | `distanceScaled` | `uint16` | Distancia x10, big-endian | Distancia sensor-superficie en décimas de centímetro.[web:253] |
| 2-3 | `waterScaled` | `uint16` | Columna de agua x10, big-endian | Altura útil del agua respecto al plano de referencia.[cite:1] |
| 4 | `levelPercentInt` | `uint8` | Entero 0-100 | Porcentaje relativo de nivel.[cite:1] |
| 5 | `statePacked` | `uint8` | nibble alto = `filterVisible`, nibble bajo = `levelState` | Estado lógico resumido del punto monitoreado.[cite:1] |

## Interpretación del byte 5

El byte 5 concentra dos indicadores lógicos en una sola posición:

- nibble alto: `filterVisible`, donde `1` indica que el filtro es visible y `0` indica que no lo es;
- nibble bajo: `levelState`, donde `0 = crítico_bajo`, `1 = bajo`, `2 = normal`, `3 = alto`.[cite:1]

Este tipo de empaquetado es coherente con payloads compactos donde varios estados discretos se representan en un solo byte para conservar ancho de banda y mantener una decodificación controlada por bits o máscaras simples.[web:253][web:255]

## Ejemplo de lectura

Supóngase la siguiente condición:

- distancia medida: 84.5 cm;
- columna de agua: 35.5 cm;
- porcentaje de nivel: 29 %;
- filtro visible: no;
- estado: bajo.

La representación sería:

- `distanceScaled = 845`;
- `waterScaled = 355`;
- `levelPercentInt = 29`;
- `statePacked = 0x01`.

En bytes big-endian, el payload podría verse así:

```text
03 4D 01 63 1D 01
```

Ese resultado representa una condición de nivel bajo, sin exposición visible del filtro, con una columna de agua todavía presente sobre el lecho filtrante.[cite:1]

## Pseudodecodificación sugerida

Una rutina de reconstrucción puede seguir esta lógica:

```js
const distanceScaled = (bytes[0] << 8) | bytes[1];
const waterScaled = (bytes[2] << 8) | bytes[3];
const levelPercent = bytes[4];
const filterVisible = (bytes[5] >> 4) & 0x01;
const levelState = bytes[5] & 0x0F;
```

A partir de ello, el decoder puede reconstruir:

- `distanceCm = distanceScaled / 10.0`
- `waterColumnCm = waterScaled / 10.0`
- `levelPercent`
- `filterVisible`
- `levelState`.[web:254][cite:45]

## Relación con HELMO

Este payload fue diseñado para mantener coherencia con la arquitectura de HELMO, donde cada nodo de campo envía información resumida y estructurada a través de la red LoRaWAN para posterior interpretación en TTN/TTS, almacenamiento en backend y visualización en servicios superiores del sistema. Su forma compacta facilita interoperabilidad con el resto de módulos ya documentados del proyecto.[cite:45][cite:1]

## Autoría

La autoría del payload y del firmware asociado al nodo de nivel debe reconocerse a:

- Esteban Eduardo Escárraga Túquerres.
- Johan Felipe Ordoñez Jenoy.
- Dayerli Katerine Tamayo Solarte.
