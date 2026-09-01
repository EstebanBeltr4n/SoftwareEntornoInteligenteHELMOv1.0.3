# Despliegue de Grafana en HELMO

Este documento describe el rol de Grafana dentro del MVP del sistema HELMO y la forma en que se utiliza para presentar visualmente la telemetría obtenida desde la red LoRaWAN, procesada por TTN y persistida en la infraestructura backend del proyecto. Grafana documenta que los dashboards pueden compartirse mediante enlaces, snapshots, exportaciones y otras modalidades, lo cual respalda su uso como herramienta de visualización y demostración dentro del entorno HELMO.[cite:174][cite:183]

## Función de Grafana en el sistema

En HELMO, Grafana cumple la función de capa de visualización del sistema. Sobre esta interfaz se representan variables adquiridas por los nodos de monitoreo de agua, así como tendencias temporales y estados operativos derivados del backend de ingestión y persistencia de datos.[cite:1][cite:45]

Dentro del flujo técnico del proyecto, Grafana se ubica como consumidor de la información ya almacenada por los módulos backend. Esto permite convertir datos telemétricos en paneles interpretables, útiles para seguimiento de variables, validación del prototipo y demostración del comportamiento del sistema frente a usuarios, asesores o evaluadores.[cite:1][cite:53]

## Alcance del dashboard del MVP

El dashboard del MVP HELMO se concibe como un tablero de monitoreo técnico y funcional. Su finalidad principal es presentar de forma ordenada las lecturas provenientes de los nodos desplegados, permitiendo revisar series temporales, valores recientes, comportamiento de sensores y estado general del entorno monitoreado.[cite:1]

Para efectos documentales y de registro, el dashboard debe entenderse como parte del entorno de ejecución observable del software, no como un simple elemento gráfico aislado. En otras palabras, constituye la manifestación visible del procesamiento, almacenamiento y consulta de datos implementado en el backend del sistema HELMO.[cite:129]

## Elementos recomendados en el dashboard

Con base en la naturaleza del proyecto HELMO, el dashboard MVP puede incluir, entre otros, los siguientes paneles:

- lecturas recientes por dispositivo;
- series de tiempo de variables de calidad de agua;
- paneles de estado o disponibilidad de nodos;
- valores agregados o resúmenes por periodo;
- paneles de seguimiento de variables críticas del sistema.[cite:1][cite:91]

La finalidad del presente documento no es fijar una única estructura visual obligatoria, sino dejar descrito el papel funcional de Grafana como componente de visualización dentro del prototipo desplegado.[cite:174]

## Compartición y validación del dashboard

Grafana documenta mecanismos de compartición por enlace interno, compartición externa, snapshots, exportes PDF, exportes JSON e imágenes. Para el contexto del MVP HELMO, estas capacidades refuerzan la posibilidad de presentar el tablero tanto en entornos locales como en escenarios de validación externa controlada.[cite:174][cite:180][cite:185]

En el caso específico de HELMO, la validación externa del MVP se apoya además en un túnel temporal mediante ngrok, el cual expone la instancia local del dashboard a través de una URL pública. Esta estrategia permite presentar el comportamiento del sistema sin desplegar una infraestructura pública permanente durante la fase de demostración.[cite:178]

## Evidencias sugeridas

En la subcarpeta `evidencias/capturas/` se recomienda adjuntar capturas del dashboard que muestren al menos:

- vista general del tablero;
- paneles principales del monitoreo;
- fecha de la demostración o validación;
- consistencia entre variables visualizadas y datos reales del sistema.

Estas capturas fortalecen el expediente técnico porque muestran la materialización visual del software funcionando sobre datos reales o de prueba controlada.[cite:129]

## Autoría

La autoría del despliegue documentado de Grafana y de la presente descripción técnica debe reconocerse a:

- Esteban Eduardo Escárraga Túquerres.
- Johan Felipe Ordoñez Jenoy.
- Dayerli Katerine Tamayo Solarte.
