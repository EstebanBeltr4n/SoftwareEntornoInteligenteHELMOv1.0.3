# Especificación funcional - Nodo de turbidez PTAP

Este documento describe el comportamiento funcional del firmware `firmware/nodo_turbidez/src/nodo_turbidez_ptap.ino`, correspondiente al nodo de turbidez del sistema HELMO. Su finalidad es complementar el código fuente con una descripción técnica del principio de adquisición, la lógica de estimación de NTU, los estados definidos y la integración del módulo con la arquitectura LoRaWAN del proyecto.[cite:129][web:224]

## Finalidad del módulo

El nodo de turbidez se diseñó para medir una variable de calidad de agua mediante la lectura analógica de un sensor óptico o equivalente de turbidez. El firmware convierte la señal capturada por el ADC del ESP32-S3 a una estimación de voltaje y luego a un valor aproximado de NTU, generando información compacta para ser transmitida dentro de la red HELMO.[web:257][web:258][web:264]

## Principio de medición

En sensores de turbidez analógicos para ESP32, una práctica común consiste en convertir la lectura ADC a voltaje y luego aplicar una ecuación de calibración, lineal o no lineal, derivada de pruebas del propio sistema. Diversas referencias técnicas muestran justamente este enfoque y recomiendan calibrar con puntos reales del montaje, agua clara y condiciones de mayor turbidez para mejorar la validez del modelo.[web:257][web:258][web:264]

La versión documental de este nodo usa un modelo lineal configurable definido por `NTU_A` y `NTU_B`, junto con un `DIVISOR_FACTOR` para compensar la etapa de acondicionamiento analógico cuando existe divisor resistivo o adaptación de señal entre el sensor y el ADC del microcontrolador.[web:258][web:259]

## Flujo de operación

1. Energizar la etapa de sensado mediante MOSFET.[cite:1]
2. Tomar múltiples lecturas ADC del sensor de turbidez.[web:257][web:263]
3. Promediar las muestras para reducir ruido.[web:258]
4. Convertir el valor ADC a voltaje en el pin del ESP32-S3.[web:259]
5. Reconstruir el voltaje del sensor usando el factor de corrección del divisor.[web:258][web:259]
6. Estimar la turbidez en NTU con la recta de calibración configurada.[web:258]
7. Clasificar el estado de turbidez en categorías discretas.[cite:1]
8. Empaquetar la información en una trama de 6 bytes para envío LoRaWAN.[cite:45]

## Estados definidos

El firmware define cuatro estados lógicos de turbidez:

- `0`: muy_baja.
- `1`: baja.
- `2`: media.
- `3`: alta.

Estos estados son una abstracción operativa que facilita la representación compacta del punto monitoreado y su posterior interpretación en dashboards, alarmas o servicios backend del sistema HELMO.[cite:45]

## Variables de salida

| Variable | Descripción |
|---|---|
| `adcRaw` | Lectura bruta del ADC del ESP32-S3.[web:259] |
| `adcVoltage` | Voltaje equivalente medido en el pin ADC.[web:259] |
| `sensorVoltage` | Voltaje reconstruido del sensor considerando divisor o acondicionamiento.[web:258][web:259] |
| `turbidityNtu` | Estimación de turbidez en unidades NTU.[web:257][web:258] |
| `turbidityState` | Clasificación categórica de la turbidez.[cite:1] |

## Integración con HELMO

El nodo se integra a la arquitectura HELMO como un componente de adquisición distribuida especializado en una variable de calidad del agua. Su salida puede ser consumida por TTN/TTS, backend, base de datos y servicios de visualización, manteniendo la misma lógica de separación por capas que ya existe en los otros nodos del proyecto.[cite:45][cite:1]

## Valor documental para el registro

La presente especificación sirve como material auxiliar descriptivo del firmware y complementa el archivo fuente `.ino`. Su propósito es identificar el nodo de turbidez como una unidad software diferenciada dentro del sistema HELMO para soporte del expediente técnico y del registro de derechos de autor.[cite:129]

## Autoría

La autoría del firmware del nodo de turbidez y de esta especificación técnica debe reconocerse a:

- Esteban Eduardo Escárraga Túquerres.
- Johan Felipe Ordoñez Jenoy.
- Dayerli Katerine Tamayo Solarte.
