# HELMO - Firmware del nodo central

El módulo `firmware/nodo_central` corresponde al firmware del nodo concentrador multisensor del sistema HELMO. Su función principal es actuar como unidad de proceso local dentro de la arquitectura del proyecto, integrando captura de variables, lógica de procesamiento en borde, gestión de comunicación LoRa/LoRaWAN y coordinación de eventos asociados al monitoreo inteligente de calidad de agua.[cite:1][cite:45]

La arquitectura general compartida para HELMO ubica a la “unidad proceso local” como el punto en el que confluyen sensores, módulos LoRa, almacenamiento o envío de datos, y posibles acciones locales sobre el sistema. En el diagrama aportado se observa que esta unidad recibe información digital desde sensores y se articula con los bloques de actuación, comunicación inalámbrica y backend de monitoreo, por lo que el nodo central constituye una pieza funcional diferenciable dentro de la obra software total.[file:196]

## Ubicación en el repositorio

```text
firmware/
└── nodo_central/
    ├── include/
    ├── src/
    │   └── nodo_concentrador_multisensor.ino
    ├── README.md
    └── central-spec.md
```

La estructura anterior sigue una organización compatible con proyectos Arduino y con entornos más estructurados como PlatformIO, donde `src` se utiliza para archivos fuente y `include` para cabeceras del proyecto. La documentación de PlatformIO explica precisamente ese esquema como base de organización para proyectos embebidos mantenibles.[web:29][web:198][web:200]

## Propósito del nodo central

El nodo central cumple funciones de agregación, control y despacho de información dentro del sistema HELMO. A partir de los antecedentes del proyecto y de la arquitectura aportada, este nodo puede entenderse como el módulo firmware encargado de consolidar lecturas, ejecutar lógica local y preparar la transmisión hacia la capa de comunicaciones inalámbricas o hacia componentes superiores de la plataforma.[cite:1][file:196]

En un sistema de monitoreo de agua como HELMO, este rol es especialmente importante porque permite desacoplar la adquisición local de variables respecto de la capa cloud o del dashboard final. Así, el firmware del nodo central no solo representa código de adquisición, sino también una parte esencial de la lógica operativa del sistema en campo.[cite:45][cite:91]

## Archivo principal

El archivo fuente principal del módulo se encuentra en:

```text
firmware/nodo_central/src/nodo_concentrador_multisensor.ino
```

Este archivo debe considerarse el núcleo ejecutable del nodo central y, para fines de registro, constituye evidencia directa de la lógica implementada en el microcontrolador o placa de procesamiento utilizada por el sistema HELMO.[cite:1]

## Funciones esperadas del firmware

Con base en la arquitectura del proyecto HELMO y en la denominación del archivo fuente, el firmware del nodo central puede documentarse como responsable de:

- inicialización del microcontrolador y periféricos del nodo;
- lectura o recepción de variables provenientes de sensores asociados;
- preprocesamiento o normalización local de datos;
- coordinación del envío inalámbrico mediante módulos LoRa/LoRaWAN;
- consolidación de eventos y estados del entorno monitoreado;
- apoyo a la lógica local del sistema en el punto de captación o unidad de proceso.[file:196][cite:1]

Estas funciones deben afinarse posteriormente conforme al contenido exacto del archivo `.ino`, pero desde la perspectiva documental ya permiten identificar el módulo como una unidad software autónoma dentro del sistema general.[cite:1]

## Relación con la arquitectura HELMO

El diagrama arquitectónico aportado muestra una secuencia funcional que inicia en captación y sensores, pasa por una unidad de proceso local y continúa hacia comunicación inalámbrica, gateway IoT, base de datos y servicios web como Grafana expuesto por URL. En dicha cadena, el nodo central representa la capa firmware que traduce la información del entorno físico a datos procesables y transmisibles para el resto del sistema.[file:196]

Por esta razón, la documentación del nodo central no debe limitarse a describirlo como “código Arduino”, sino como firmware de integración para una unidad de proceso local orientada al monitoreo inteligente del agua. Esa caracterización fortalece la claridad técnica del expediente de autoría del software.[cite:129]

## Recomendaciones documentales para el registro

Para fines de derechos de autor, el módulo `nodo_central` debe acompañarse al menos de:

- el archivo fuente principal `.ino`;
- el presente `README.md`;
- una especificación funcional del nodo;
- opcionalmente, cabeceras en `include/` si el código se modulariza;
- referencias al rol del nodo dentro de la arquitectura general de HELMO.[cite:129]

La guía procedimental de registro de software ante la DNDA contempla el uso de código fuente y material auxiliar descriptivo, por lo que esta documentación complementaria resulta pertinente para explicar el alcance del firmware en el sistema.[cite:129]

## Autoría

La autoría del firmware del nodo central y de la documentación asociada debe reconocerse a:

- Esteban Eduardo Escárraga Túquerres.
- Johan Felipe Ordoñez Jenoy.
- Dayerli Katerine Tamayo Solarte.
