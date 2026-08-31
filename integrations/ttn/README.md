# Integración TTN - Decodificador Uplink HELMO

Este módulo contiene el decodificador de carga útil (*uplink payload formatter*) utilizado en The Things Stack para interpretar los paquetes LoRaWAN transmitidos por los nodos del sistema HELMO. La función principal del módulo es transformar la carga útil binaria recibida desde los dispositivos finales en una estructura JSON legible y consistente, la cual posteriormente queda disponible en el campo `decoded_payload` del mensaje de uplink dentro de The Things Stack.[cite:70][cite:84]

La implementación utiliza un *Custom JavaScript formatter*, mecanismo soportado por The Things Stack para definir una función `decodeUplink(input)` ejecutada cuando se recibe un mensaje de datos desde un dispositivo LoRaWAN. Esta función procesa el arreglo de bytes (`input.bytes`) y el puerto de aplicación (`input.fPort`) para producir datos estructurados, advertencias y errores según el formato del payload recibido.[cite:70][cite:72][cite:74]

## Propósito del módulo

El decodificador uplink forma parte de la capa de integración entre la red LoRaWAN y el backend del sistema HELMO. Su propósito es estandarizar la telemetría recibida antes de que sea consumida por los servicios posteriores de ingestión, almacenamiento y visualización desplegados en el servidor Debian 12 del proyecto, donde los datos son procesados, almacenados en MongoDB local y en la nube, y visualizados mediante Grafana expuesto por ngrok.[cite:1][cite:45]

Este componente permite distinguir entre diferentes tipos de nodos y formatos de mensaje, incluyendo un nodo monolítico multisensor, nodos dedicados de pH y un formato heredado de compatibilidad. Gracias a ello, la salida decodificada mantiene coherencia semántica dentro de la aplicación y facilita el tratamiento uniforme de las variables de monitoreo.[cite:70][cite:71]

## Ubicación recomendada en el repositorio

La ubicación recomendada para este módulo dentro del repositorio es la siguiente:

```text
integrations/
└── ttn/
    ├── README.md
    ├── payload_spec.md
    └── uplink_decoder.js
```

Esta organización separa adecuadamente el código de integración LoRaWAN del firmware embebido y del backend del servidor, lo que mejora la trazabilidad técnica, la mantenibilidad del sistema y la preparación del repositorio para fines académicos, de transferencia de conocimiento y de registro de software.[cite:13][cite:72]

## Funcionamiento general

En The Things Stack, el formatter uplink se configura desde la sección **Applications > Payload Formatters > Uplink**, seleccionando el tipo **Custom JavaScript formatter** e incorporando una función `decodeUplink(input)`. La plataforma ejecuta esta función sobre la carga útil binaria (`frm_payload`) y publica el resultado decodificado en el campo `decoded_payload` del mensaje de uplink.[cite:72][cite:70][cite:84]

Dentro del proyecto HELMO, esta lógica de decodificación identifica el puerto LoRaWAN y la longitud del payload para determinar el tipo de mensaje recibido. A partir de ello, el módulo convierte los datos binarios en variables físicas como nivel de agua, turbidez, pH y voltaje de lectura del sensor de pH, aplicando factores de escala definidos por el diseño del protocolo de transmisión del sistema.[cite:70][cite:74]

## Casos soportados

El decodificador uplink implementado en HELMO contempla los siguientes casos:

- Nodo monolítico clásico: mensaje multisensor enviado por un único nodo que agrupa nivel, turbidez y pH.
- Nodo dedicado de pH: mensaje con identificador de nodo, valor de pH y voltaje de lectura.
- Formato heredado: mensaje compatible con una estructura previa de turbidez y pH sin variable de nivel.
- Detección de error: generación de error explícito cuando el `fPort` y la longitud del payload no coinciden con ningún formato reconocido.[cite:70][cite:72]

## Valor documental para registro

La Dirección Nacional de Derecho de Autor de Colombia indica que, para el registro de software, pueden aportarse el código fuente, una descripción suficientemente detallada de los procedimientos del programa y material auxiliar como manuales de usuario. Por ello, conservar este módulo como archivo fuente versionado junto con su documentación técnica fortalece la evidencia de autoría, diseño funcional y alcance del sistema HELMO.[cite:13]

El archivo `README.md` cumple la función de describir la finalidad del módulo, su contexto dentro de la arquitectura general del sistema y su relación con el procesamiento de telemetría. Complementariamente, el archivo `payload_spec.md` documenta de forma precisa la estructura de los payloads, los puertos utilizados, la longitud esperada y las reglas de interpretación aplicadas durante la decodificación.[cite:13][cite:70]

## Autoría

Este módulo forma parte del sistema HELMO y su autoría debe reconocerse a:

- Esteban Eduardo Escárraga Túquerres.
- Johan Felipe Ordoñez Jenoy.
- Dayerli Katerine Tamayo Solarte.

## Recomendaciones de mantenimiento

- Mantener en sincronía el contenido de `uplink_decoder.js` con la versión activa configurada en The Things Stack.[cite:72]
- Registrar en el historial de cambios cualquier modificación en puertos, tamaño de payload, factores de escala o alias de nodos.
- Evitar cambios no documentados en la estructura del payload, ya que afectarían la compatibilidad entre nodos, TTN y backend.
- Referenciar este módulo en la documentación de arquitectura general del sistema para demostrar su papel dentro de la cadena completa de telemetría HELMO.[cite:1][cite:45]
