# Uso de la carpeta include - Nodo de pH HELMO

La carpeta `include/` del módulo `firmware/nodo_ph` está destinada a encabezados del proyecto, como constantes de calibración, configuración de pines, alias de despliegue, prototipos de funciones y estructuras auxiliares. Esta organización es adecuada para una futura modularización del nodo y ayuda a mantener una presentación ordenada del repositorio técnico del proyecto HELMO.[cite:1]

En futuras iteraciones, esta carpeta puede contener archivos como:

- `ph_config.h`
- `pins.h`
- `payload_ph.h`
- `calibration_ph.h`

Documentar desde ahora su propósito fortalece la consistencia del bloque `firmware/` y hace más sencillo replicar esta misma base para crear otros nodos de monitoreo desplegados en diferentes lugares de la red LoRa HELMO.[cite:1][cite:45]

## Autoría

La autoría de la estructura documental del nodo de pH debe reconocerse a:

- Esteban Eduardo Escárraga Túquerres.
- Johan Felipe Ordoñez Jenoy.
- Dayerli Katerine Tamayo Solarte.
