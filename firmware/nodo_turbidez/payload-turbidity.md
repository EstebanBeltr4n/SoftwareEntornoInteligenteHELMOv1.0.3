# Especificación de payload - Nodo de turbidez HELMO

Este documento describe la estructura del payload generado por `firmware/nodo_turbidez/src/nodo_turbidez_ptap.ino` para transmisión sobre la red HELMO. La trama fue diseñada para mantener bajo tamaño, interpretación determinística y compatibilidad con decoders uplink y módulos backend que reconstruyen variables a partir de bytes recibidos por LoRaWAN.[cite:45][web:255]

## Tamaño y propósito

El payload tiene tamaño fijo de 6 bytes. Su diseño busca representar la medición principal de turbidez y algunos elementos auxiliares del estado del sensor sin aumentar innecesariamente el tamaño del mensaje transmitido por la red.[web:255][web:247]

## Estructura binaria

| Byte | Campo | Tipo | Escalado / codificación | Descripción |
|---|---|---|---|---|
| 0-1 | `adcField` | `uint16` | Big-endian | Lectura bruta del ADC.[web:259] |
| 2-3 | `ntuScaled` | `uint16` | NTU x10, big-endian | Turbidez estimada en décimas de NTU.[web:257][web:258] |
| 4 | `voltageScaled` | `uint8` | Voltaje del sensor x100, saturado a 255 | Referencia compacta del nivel de voltaje del sensor.[web:259] |
| 5 | `stateField` | `uint8` | Estado categórico | `0` muy_baja, `1` baja, `2` media, `3` alta.[cite:1] |

## Ejemplo de lectura

Supóngase la siguiente condición:

- `adcRaw = 2380`;
- `sensorVoltage = 1.72 V`;
- `turbidityNtu = 0.7 NTU`;
- `turbidityState = 0`.

La representación sería:

- `adcField = 2380`;
- `ntuScaled = 7`;
- `voltageScaled = 172`;
- `stateField = 0`.

En bytes big-endian, el payload podría verse así:

```text
09 4C 00 07 AC 00
```

Este ejemplo describe una condición de turbidez muy baja o cercana a agua clara, con baja dispersión óptica estimada por el modelo lineal configurado en el firmware.[web:258][cite:1]

## Pseudodecodificación sugerida

```js
const adcRaw = (bytes[0] << 8) | bytes[1];
const ntuScaled = (bytes[2] << 8) | bytes[3];
const voltageScaled = bytes[4];
const turbidityState = bytes[5];

const turbidityNtu = ntuScaled / 10.0;
const sensorVoltage = voltageScaled / 100.0;
```

Esta reconstrucción permite que el backend o el formatter uplink presenten tanto la variable principal de NTU como indicadores auxiliares útiles para trazabilidad de la medición.[web:255][cite:45]

## Relación con HELMO

El payload fue diseñado para integrarse con la arquitectura HELMO existente, en la que los nodos de campo generan datos compactos y estructurados para su transporte LoRaWAN y posterior interpretación en TTN/TTS, backend, almacenamiento y dashboards. Esta forma de serialización mantiene consistencia con los otros nodos del proyecto y fortalece la interoperabilidad del repositorio técnico.[cite:45][cite:1]

## Autoría

La autoría del payload y del firmware asociado al nodo de turbidez debe reconocerse a:

- Esteban Eduardo Escárraga Túquerres.
- Johan Felipe Ordoñez Jenoy.
- Dayerli Katerine Tamayo Solarte.
