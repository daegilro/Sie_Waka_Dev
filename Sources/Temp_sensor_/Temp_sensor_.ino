#include <OneWire.h>                
#include <DallasTemperature.h>
 
OneWire ourWire(4);                //Se establece el pin 2  como bus OneWire
 
DallasTemperature sensors(&ourWire); //Se declara una variable u objeto para nuestro sensor

void setup() {
delay(100);
Serial.begin(115200);
sensors.begin();   //Se inicia el sensor
}
 
void loop() {
sensors.requestTemperatures();   //Se envía el comando para leer la temperatura
float temp= sensors.getTempCByIndex(0); //Se obtiene la temperatura en ºC
float temp2 = analogRead(35);
Serial.print("Temperatura= ");
Serial.print(temp);
Serial.println(" C");
Serial.print("Temp RAW= ");
Serial.print(temp2);
Serial.println(" C");

delay(100);                     
}
