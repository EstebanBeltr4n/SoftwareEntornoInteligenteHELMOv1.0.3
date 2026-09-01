# Despliegue del MVP HELMO

La carpeta `backend/deploy` reúne la documentación técnica asociada al despliegue funcional del MVP del sistema HELMO, específicamente en lo relacionado con la visualización de telemetría en dashboards de Grafana y su exposición remota mediante ngrok. Este conjunto documental sirve como material auxiliar del software y complementa el código fuente del backend, describiendo el entorno operativo en el que el sistema fue demostrado y validado.[cite:129]

Dentro de la arquitectura HELMO, la capa de despliegue se ubica después de la adquisición, transporte y persistencia de datos. Los nodos LoRaWAN transmiten información al gateway, TTN procesa y decodifica los uplinks, los módulos backend almacenan la información en MongoDB y finalmente Grafana consume esos datos para generar tableros de monitoreo visibles por el equipo de trabajo o por evaluadores externos del MVP.[cite:1][cite:45]

## Contenido de esta carpeta

```text
backend/
└── deploy/
    ├── README.md
    ├── grafana-deploy.md
    ├── ngrok-exposure.md
    └── evidencias/
        ├── dashboard_url_example.txt
        └── capturas/
```

La estructura separa la explicación del servicio de visualización, el método de exposición remota y la evidencia complementaria del despliegue. Esta forma de organización es pertinente para documentación de software porque permite distinguir la lógica del sistema de su entorno de ejecución y demostración.[cite:129]

## Finalidad del despliegue documentado

El despliegue descrito en esta carpeta corresponde al modo de presentación del MVP de HELMO. Grafana se utiliza como interfaz de visualización de series temporales y variables ambientales, mientras que ngrok se emplea para exponer temporalmente el servicio local mediante una URL pública, facilitando demostraciones remotas, validación funcional y revisión externa del prototipo.[cite:174][cite:178]

Esta modalidad de exposición es coherente con un entorno de prueba o validación temprana, ya que permite acceso externo sin requerir una infraestructura de publicación definitiva. No obstante, debe entenderse como una estrategia de despliegue de MVP y no como una arquitectura final de producción endurecida.[cite:178]

## Relación con el registro de derechos de autor

La guía procedimental de registro de software ante la DNDA contempla el uso de material auxiliar y descripciones técnicas del programa como soporte documental complementario al código fuente. En este contexto, la carpeta `backend/deploy` funciona como evidencia descriptiva del modo en que el software HELMO fue puesto en operación y visualizado durante su fase de demostración y validación.[cite:129]

## Autoría

La autoría del despliegue documentado y de la documentación técnica asociada debe reconocerse a:

- Esteban Eduardo Escárraga Túquerres.
- Johan Felipe Ordoñez Jenoy.
- Dayerli Katerine Tamayo Solarte.
