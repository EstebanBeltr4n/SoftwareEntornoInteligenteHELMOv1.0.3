# Especificación funcional - Nodo central HELMO

Este documento describe el rol técnico del firmware correspondiente al nodo central del sistema HELMO, ubicado en `firmware/nodo_central/src/nodo_concentrador_multisensor.ino`. Su finalidad es complementar el código fuente con una descripción suficientemente detallada de las funciones, propósito y relación arquitectónica del módulo dentro del sistema de monitoreo inteligente de agua.[cite:129][cite:1]

## Identificación del módulo

- **Módulo:** Nodo central.
- **Ubicación:** `firmware/nodo_central/`.
- **Archivo fuente principal:** `src/nodo_concentrador_multisensor.ino`.
- **Tipo de componente:** Firmware embebido para unidad de proceso local multisensor.[cite:1]

## Función general

El nodo central implementa la lógica embebida de la unidad concentradora del sistema HELMO. Su papel es recibir, consolidar y procesar información proveniente del entorno de captación y de los sensores asociados, organizando dichos datos para su transmisión inalámbrica y su integración con las capas superiores del sistema.[file:196][cite:45]

Este módulo actúa como intermediario entre el entorno físico de monitoreo y la infraestructura digital del proyecto. En consecuencia, representa la capa firmware que transforma señales, lecturas o eventos de campo en información estructurada utilizable por la red LoRaWAN, el backend y el tablero de visualización.[file:196]

## Papel dentro de la arquitectura

Según el diagrama aportado, la unidad de proceso local se relaciona con sensores de turbidez, pH y nivel, así como con módulos LoRa, actuadores y la capa de comunicación inalámbrica que enlaza con el gateway IoT, TTN, MongoDB y Grafana. El nodo central debe entenderse como la implementación software embebida de esa unidad de proceso local descrita en la arquitectura HELMO.[file:196]

Esta posición dentro del sistema le otorga relevancia funcional, ya que coordina la transición entre adquisición sensórica local y transmisión hacia el resto de la plataforma. Por tanto, su documentación resulta clave dentro del expediente técnico del firmware del proyecto.[cite:1][cite:45]

## Responsabilidades funcionales

Con base en la arquitectura general y en el propósito declarado del proyecto, las responsabilidades del nodo central pueden describirse así:

1. Inicializar el microcontrolador, buses, pines, temporizadores y módulos necesarios para el funcionamiento del nodo.[cite:1]
2. Leer o integrar datos provenientes de los sensores del sistema o de módulos secundarios asociados al monitoreo.[file:196]
3. Aplicar transformaciones, validaciones o agregaciones básicas previas a la transmisión.[cite:1]
4. Preparar tramas o estructuras de datos para su envío por LoRa/LoRaWAN.[cite:45]
5. Coordinar eventos locales, alarmas o interacciones con actuadores, cuando la lógica del sistema así lo requiera.[file:196]
6. Sostener el enlace entre la operación en campo y la infraestructura digital remota del proyecto HELMO.[cite:1]

## Entradas y salidas esperadas

### Entradas

Las entradas funcionales del nodo central incluyen información procedente de sensores y señales internas del sistema. En la arquitectura suministrada aparecen como referencias principales la captación y sensores de turbidez, pH y nivel, además de la interacción con módulos LoRa y posibles actuadores.[file:196]

### Salidas

Las salidas del nodo central corresponden a datos digitales estructurados, eventos de transmisión inalámbrica y, eventualmente, señales de control local. Estas salidas se articulan con la red inalámbrica, el gateway IoT y las capas backend del sistema HELMO.[file:196][cite:45]

## Entorno de desarrollo

El archivo principal del módulo usa extensión `.ino`, lo que lo sitúa dentro del ecosistema Arduino. No obstante, la organización del firmware con carpetas `src` e `include` también es compatible con prácticas de estructuración propias de PlatformIO y otros entornos de desarrollo embebido más formales.[web:29][web:198]

Esta compatibilidad es conveniente desde la perspectiva de mantenimiento y trazabilidad del software, ya que permite evolucionar el proyecto hacia estructuras más modulares sin perder la claridad de su organización actual.[web:200]

## Valor para registro de software

La presente especificación funcional tiene como propósito describir el módulo firmware del nodo central más allá del código fuente bruto. La guía de registro de software ante la DNDA contempla tanto el código como el material auxiliar descriptivo, lo que hace pertinente anexar este documento como complemento explicativo del archivo `.ino` principal.[cite:129]

## Autoría

La autoría del firmware del nodo central y de la presente especificación técnica debe reconocerse a:

- Esteban Eduardo Escárraga Túquerres.
- Johan Felipe Ordoñez Jenoy.
- Dayerli Katerine Tamayo Solarte.
