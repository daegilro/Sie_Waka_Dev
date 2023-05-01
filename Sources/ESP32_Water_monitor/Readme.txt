Este proyecto esta desarrollado sobre la tarjeta de desarrollo ESP32 
y el ambiente de desarrollo de Arduino.

1. Se establece la conexión via Wi-Fi con la red local donde se encuentre 
el modulo
2. Se establece la comunicación con el Core IoT de AWS, por medio de la 
validación de los certificados.
3. Se realiza la lectura de los sensores de temperatura, electroconductividad
y pH cada segundo, y se publica en el topico "esp32/pub" un solo mensaje en 
formato JSON con la infromación respectiva de cada sensor, esta información 
se publica igual cada segundo.