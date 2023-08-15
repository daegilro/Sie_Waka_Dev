#include <WiFiClientSecure.h>
#include <OneWire.h>                
#include <DallasTemperature.h>

#define PH_PIN 33
float acidVoltage    = 1876;    //buffer solution 4.0 at 25C
float neutralVoltage = 1414;     //buffer solution 7.0 at 25C
float slopePH,interceptPH, slopeEC,interceptEC = 1;
float highec    = 1390;    //buffer solution 4.0 at 25C
float lowec = 60;     //buffer solution 7.0 at 25C


#define EC_PIN 32
#define RES2 820.0
#define ECREF 50
float kValue =0.993;
bool ok_read = false;
float  voltagePH, voltageEC, phValue, ecValue, temperature = 19;
OneWire ourWire(4);                //Se establece el pin 2  como bus OneWire
// Red, green, and blue pins for PWM control
const int redPin = 13;     // 13 corresponds to GPIO13
const int greenPin = 12;   // 12 corresponds to GPIO12
const int bluePin = 14;    // 14 corresponds to GPIO14

// Setting PWM frequency, channels and bit resolution
const int freq = 5000;
const int redChannel = 0;
const int greenChannel = 1;
const int blueChannel = 2;
// Bit resolution 2^8 = 256
const int resolution = 8;
 
DallasTemperature sensors(&ourWire); //Se declara una variable u objeto para nuestro sensor




void setup() {
  Serial.begin(115200);
  ledcSetup(redChannel, freq, resolution);
  ledcSetup(greenChannel, freq, resolution);
  ledcSetup(blueChannel, freq, resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(redPin, redChannel);
  ledcAttachPin(greenPin, greenChannel);
  ledcAttachPin(bluePin, blueChannel);
  sensors.begin();
  
}

void loop() {
  static unsigned long timepoint = millis();
  if(millis()-timepoint>1000U){
    //time interval: 1s
    timepoint = millis();
    sensors.requestTemperatures();   //Se envía el comando para leer la temperatura
    temperature = sensors.getTempCByIndex(0);
    Serial.print("Temperatura: ");
    Serial.print(temperature,2);
    Serial.println(" ºC");
    voltagePH = analogRead(PH_PIN)/4095.0*3300;          // read the ph voltage
    Serial.print("PHconver: ");
    Serial.println(voltagePH);
    voltageEC = analogRead(EC_PIN)/4095.0*3300;
    Serial.print("ECconver: ");
    Serial.println(voltageEC);
    delay(500);
    }
    
}
