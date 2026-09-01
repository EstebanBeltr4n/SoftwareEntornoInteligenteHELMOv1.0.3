# Exposición del MVP mediante ngrok

Este documento describe el mecanismo empleado para exponer el entorno de visualización del MVP HELMO hacia una URL pública temporal mediante ngrok. La documentación oficial de ngrok indica que esta herramienta permite publicar aplicaciones locales en una URL pública de forma inmediata utilizando su CLI, lo cual resulta apropiado para demostraciones, validaciones remotas y pruebas controladas del prototipo.[cite:178]

## Función de ngrok dentro de HELMO

En el contexto de HELMO, ngrok se utiliza como puente entre el entorno local del servidor y los evaluadores o usuarios externos que requieren visualizar el dashboard del MVP. En lugar de desplegar desde el inicio una infraestructura pública permanente, el sistema utiliza un túnel seguro y temporal para exponer la interfaz local de Grafana mediante una dirección accesible desde Internet.[cite:178][cite:177]

Esta decisión técnica es coherente con una fase de validación o demostración de prototipo, ya que reduce complejidad de despliegue, acelera pruebas externas y permite controlar el ciclo de exposición del servicio. No obstante, se trata de un mecanismo apropiado para MVP o validación y no debe asumirse automáticamente como el esquema definitivo de publicación productiva.[cite:178]

## Flujo de exposición

El proceso documentado para HELMO puede describirse así:

1. El servicio Grafana se ejecuta localmente en el servidor del proyecto.[cite:174]
2. ngrok crea un túnel hacia el puerto local donde Grafana está publicado.[cite:178]
3. ngrok genera una URL pública temporal, que se comparte con fines de validación o demostración del MVP.[cite:178]
4. Los usuarios autorizados acceden al dashboard a través de dicha URL y observan el entorno de monitoreo del sistema.[cite:174][cite:178]

## Ejemplo de comando de exposición

El comando exacto depende del puerto local donde se encuentre Grafana. Un ejemplo típico de exposición sería:

```bash
ngrok http 3000
```

Si Grafana se encuentra publicado en otro puerto del servidor, el valor debe ajustarse al puerto real del servicio. La URL generada por ngrok se registra de forma descriptiva en el archivo `evidencias/dashboard_url_example.txt` como ejemplo documental del tipo de enlace utilizado por el MVP.[cite:178]

## Consideraciones de seguridad documental

La documentación del despliegue no debe incluir tokens de ngrok, credenciales activas ni información reservada del entorno local. Si se desea conservar evidencia del enlace, es preferible usar una URL anonimizada o una URL de ejemplo con fines descriptivos, evitando publicar direcciones vigentes si el repositorio será compartido externamente.[cite:178]

## Relación con Grafana

Grafana documenta mecanismos para compartir dashboards mediante enlaces y modalidades de compartición externa. En HELMO, ngrok complementa esa posibilidad al proporcionar la publicación remota del servicio local donde reside la interfaz del dashboard, permitiendo que el MVP pueda observarse desde fuera del entorno del servidor sin realizar un despliegue público completo.[cite:174][cite:180]

## Evidencias sugeridas

En la carpeta `evidencias/capturas/` se recomienda incorporar capturas de:

- terminal con el túnel ngrok activo;
- dashboard abierto desde la URL pública;
- fecha y contexto de la demostración;
- consistencia entre la sesión expuesta y el entorno local de Grafana.

Estas evidencias ayudan a documentar el modo en que el software fue puesto en operación y demostrado durante la fase MVP.[cite:129]

## Autoría

La autoría del esquema de exposición documentado y de la presente descripción técnica debe reconocerse a:

- Esteban Eduardo Escárraga Túquerres.
- Johan Felipe Ordoñez Jenoy.
- Dayerli Katerine Tamayo Solarte.
