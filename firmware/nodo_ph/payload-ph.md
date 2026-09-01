# Especificación de payload - Nodo de pH HELMO

Este documento describe la estructura del payload generado por `firmware/nodo_ph/src/nodo_ph_casa_01.ino` para transmisión sobre la red HELMO. La trama fue diseñada para ser compacta, interpretable y adecuada para decodificación en TTN/TTS o en módulos backend encargados de reconstruir el valor de pH y datos asociados del sensor.[web:247][cite:45]

## Tamaño y propósito

El payload del nodo de pH tiene un tamaño fijo de 5 bytes. Esta decisión permite transportar la variable principal de pH, junto con el identificador del nodo y el voltaje auxiliar de la medición, manteniendo una trama corta y eficiente para LoRaWAN.[web:247][web:244]

## Estructura binaria

| Byte | Campo | Tipo | Escalado / codificación | Descripción |
|---|---|---|---|---|
| 0 | `NODE_ID` | `uint8` | Valor directo | Identificador del nodo dentro de la red HELMO.[cite:1] |
| 1-2 | `phInt` | `uint16` | pH x100, big-endian | Valor de pH con dos decimales.[cite:1] |
| 3-4 | `voltInt` | `uint16` | Voltaje x1000, big-endian | Voltaje del sensor en milivoltios aproximados.[cite:1] |

## Ejemplo de lectura

Supóngase la siguiente condición:

- `NODE_ID = 1`;
- `pH = 6.87`;
- `voltajeSensor = 2.321 V`.

La representación sería:

- `NODE_ID = 0x01`;
- `phInt = 687`;
- `voltInt = 2321`.

En bytes big-endian, el payload podría verse así:

```text
01 02 AF 09 11
```

Este ejemplo representa un nodo identificado como `1`, con pH cercano a neutro ácido y voltaje consistente con el modelo de calibración configurado en el firmware.[cite:1]

## Pseudodecodificación sugerida

```js
const nodeId = bytes[0];
const phInt = (bytes[1] << 8) | bytes[2];
const voltInt = (bytes[3] << 8) | bytes[4];

const ph = phInt / 100.0;
const voltage = voltInt / 1000.0;
```

Esta reconstrucción permite que el formatter uplink o el backend presenten tanto el valor principal de pH como una referencia del voltaje leído por el sensor, útil para depuración, validación y trazabilidad de la medición.[web:247][cite:45]

## Relación con la arquitectura HELMO

Este payload mantiene coherencia con la arquitectura de HELMO, en la que cada nodo transmite telemetría compacta, con estructura bien definida y desacoplada del backend. El uso de un identificador de nodo en el byte inicial también facilita construir nuevos nodos derivados de esta base para desplegarlos en otros lugares de la red LoRa del sistema.[cite:45][cite:1]

## Autoría

La autoría del payload y del firmware asociado al nodo de pH debe reconocerse a:

- Esteban Eduardo Escárraga Túquerres.
- Johan Felipe Ordoñez Jenoy.
- Dayerli Katerine Tamayo Solarte.
