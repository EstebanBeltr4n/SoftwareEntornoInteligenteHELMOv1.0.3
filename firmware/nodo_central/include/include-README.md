# Uso de la carpeta include - Nodo central HELMO

La carpeta `include/` del módulo `firmware/nodo_central` está destinada a archivos de cabecera del proyecto, tales como definiciones de pines, constantes globales, prototipos de funciones y estructuras compartidas entre varios archivos fuente. La documentación de PlatformIO y su estructura de proyecto recomienda precisamente este directorio para los encabezados del proyecto embebido.[web:198][web:200]

Si el firmware del nodo central evoluciona hacia una organización más modular, esta carpeta puede contener archivos como:

- `pins.h`
- `config.h`
- `sensors.h`
- `lora_payload.h`
- `system_state.h`

Aunque el archivo principal del nodo central pueda compilar inicialmente como un único `.ino`, documentar el propósito de `include/` resulta útil para la mantenibilidad del proyecto y para dejar constancia de una estructura profesional del firmware dentro del repositorio HELMO.[web:29][web:38]

## Autoría

La autoría de la estructura documental del nodo central debe reconocerse a:

- Esteban Eduardo Escárraga Túquerres.
- Johan Felipe Ordoñez Jenoy.
- Dayerli Katerine Tamayo Solarte.
