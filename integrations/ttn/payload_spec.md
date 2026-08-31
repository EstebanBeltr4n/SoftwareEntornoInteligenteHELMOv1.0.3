# Especificación del payload Uplink - HELMO

Este documento describe la estructura de los mensajes uplink interpretados por el decodificador JavaScript configurado en The Things Stack para el sistema HELMO. La función `decodeUplink(input)` es el mecanismo previsto por The Things Stack para transformar un payload binario recibido desde un dispositivo LoRaWAN en un objeto JSON legible, publicado luego en `decoded_payload` dentro del mensaje uplink.[cite:70][cite:72][cite:84]

La presente especificación documenta los casos actualmente soportados por el módulo `uplink_decoder.js`, los puertos LoRaWAN utilizados, el tamaño esperado del payload, la estructura de bytes y las conversiones aplicadas para obtener magnitudes físicas interpretables por el backend de HELMO.[cite:70][cite:74]

## Alcance

La especificación aplica al flujo de telemetría del sistema HELMO, donde la información de los nodos sensores es transmitida por LoRaWAN, recibida por TTN y posteriormente entregada al backend del proyecto mediante la cadena de integración y procesamiento ya desplegada sobre el servidor Debian 12.[cite:1][cite:45]

## Resumen de formatos soportados

| Caso | fPort | Longitud | Tipo de nodo | Variables decodificadas |
|---|---:|---:|---|---|
| Monolito multisensor | 2 | 6 bytes | Nodo monolítico clásico | `nivel_cm`, `turbidez_ntu`, `ph` [cite:70] |
| Nodo dedicado de pH | 3 | 5 bytes | Nodo individual de pH | `node_id`, `tipo_nodo`, `familia_nodo`, `ph`, `ph_voltage` [cite:70] |
| Compatibilidad heredada | 2 | 4 bytes | Formato legado turbidez+pH | `turbidez_ntu`, `ph`, `nivel_cm=0.0` [cite:70] |

## Caso 1: Nodo monolítico clásico

### Condición de reconocimiento

- `fPort = 2`
- Longitud del payload = 6 bytes

### Estructura del payload

| Byte(s) | Campo bruto | Descripción |
|---|---|---|
| 0-1 | `nivelRaw` | Nivel de agua codificado en entero de 16 bits |
| 2-3 | `turbidezRaw` | Turbidez codificada en entero de 16 bits |
| 4-5 | `phRaw` | pH codificado en entero de 16 bits |

### Decodificación aplicada

| Variable de salida | Fórmula |
|---|---|
| `tipo_nodo` | `"monolito"` |
| `nivel_cm` | `nivelRaw / 10.0` |
| `turbidez_ntu` | `turbidezRaw / 10.0` |
| `ph` | `phRaw / 100.0` |

### Interpretación funcional

Este formato corresponde a un nodo de adquisición integrada que concentra múltiples variables de calidad y estado del agua en un único paquete uplink. La finalidad de este diseño es simplificar la transmisión cuando un mismo nodo mide simultáneamente nivel, turbidez y pH.[cite:91]

## Caso 2: Nodo dedicado de pH con identificador

### Condición de reconocimiento

- `fPort = 3`
- Longitud del payload = 5 bytes

### Estructura del payload

| Byte(s) | Campo bruto | Descripción |
|---|---|---|
| 0 | `nodeId` | Identificador lógico del nodo de pH |
| 1-2 | `phRaw5` | Valor de pH codificado en entero de 16 bits |
| 3-4 | `voltRaw5` | Voltaje del sensor codificado en entero de 16 bits |

### Alias implementados

| node_id | Alias resultante |
|---|---|
| 1 | `phcasa1` |
| 2 | `phcasa2` |
| 3 | `phcasa3` |
| 4 | `phcasa4` |
| 5 | `phcasa5` |
| 6 | `phcasa6` |
| Otro | `phcasa_desconocido_<nodeId>` |

### Decodificación aplicada

| Variable de salida | Fórmula |
|---|---|
| `node_id` | `nodeId` |
| `tipo_nodo` | `getPhNodeAlias(nodeId)` |
| `familia_nodo` | `"ph"` |
| `ph` | `phRaw5 / 100.0` |
| `ph_voltage` | `voltRaw5 / 1000.0` |

### Interpretación funcional

Este formato corresponde a nodos de pH independientes con identificación individual, lo que permite distinguir varios puntos de medición dentro de una misma aplicación TTN. La inclusión del voltaje del sensor añade trazabilidad técnica para procesos de validación, calibración y diagnóstico del comportamiento del sistema de medición.[cite:1][cite:91]

## Caso 3: Formato heredado de compatibilidad

### Condición de reconocimiento

- `fPort = 2`
- Longitud del payload = 4 bytes

### Estructura del payload

| Byte(s) | Campo bruto | Descripción |
|---|---|---|
| 0-1 | `turbidezRaw4` | Turbidez codificada en entero de 16 bits |
| 2-3 | `phRaw4` | pH codificado en entero de 16 bits |

### Decodificación aplicada

| Variable de salida | Fórmula |
|---|---|
| `tipo_nodo` | `"legacy_tp"` |
| `turbidez_ntu` | `turbidezRaw4 / 10.0` |
| `ph` | `phRaw4 / 100.0` |
| `nivel_cm` | `0.0` |

### Interpretación funcional

Este caso se conserva por compatibilidad con estructuras anteriores del proyecto que transmitían únicamente turbidez y pH. Su presencia permite mantener interoperabilidad con implementaciones previas durante procesos de transición, validación y migración del sistema HELMO hacia una arquitectura más modular.[cite:91]

## Manejo de errores

Si el payload recibido no coincide con ninguno de los patrones definidos por combinación de `fPort` y longitud, la función retorna un objeto con la propiedad `errors`, indicando que se recibió un payload no reconocido. Este comportamiento es consistente con el esquema de salida esperado en los payload formatters JavaScript de The Things Stack.[cite:70][cite:72]

Ejemplo conceptual de salida de error:

```javascript
{
  errors: ["Payload no reconocido. fPort=X, bytes=Y"]
}
```

## Relación con TTN y backend

The Things Stack ejecuta el formatter sobre la carga útil binaria del uplink y publica el resultado dentro de `decoded_payload`, lo que permite a las integraciones posteriores consumir telemetría ya interpretada en formato JSON. Esta salida es especialmente útil para los procesos de webhook, puente y almacenamiento desplegados en HELMO, ya que reduce la complejidad del backend al recibir variables físicas ya decodificadas en vez de bytes crudos.[cite:70][cite:84][cite:1]

## Autoría

La presente especificación y el módulo de decodificación asociado forman parte del sistema HELMO. La autoría debe reconocerse a:

- Esteban Eduardo Escárraga Túquerres.
- Johan Felipe Ordoñez Jenoy.
- Dayerli Katerine Tamayo Solarte.

## Recomendaciones de control documental

- Versionar este documento junto con `uplink_decoder.js` en cada cambio funcional del protocolo.
- Actualizar inmediatamente esta especificación cuando cambie un `fPort`, el número de bytes o la escala de decodificación.
- Mantener coherencia entre esta especificación, el código configurado en The Things Stack y la documentación general de arquitectura del sistema.
- Incluir este documento como anexo técnico dentro del expediente de registro de software o documentación institucional del proyecto HELMO.[cite:13]