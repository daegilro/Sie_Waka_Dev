// LLamado de librerias a utilizar
#include "secrets.h"
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <ArduinoMqttClient.h>
#include <OneWire.h>                
#include <DallasTemperature.h>

//Configuracion Identificador  modulo
int deviceID = 6;

//Configuracion ubicacion modulo
float latitude = 3.290512; 
float longitude = -76.570426;
char lonchar[10];
char latchar[10];


//Configuración Sensor PH
#define PH_PIN 33
float acidVoltage    = 1876;    //buffer solution 4.0 at 25C
float neutralVoltage = 1414;     //buffer solution 7.0 at 25C
float slopePH,interceptPH, slopeEC,interceptEC = 1;

//Configuración sensor EC
#define EC_PIN 32
#define RES2 820.0
#define ECREF 50
float kValue =0.993;
float highec    = 1390;    //buffer solution 4.0 at 25C
float lowec = 60;     //buffer solution 7.0 at 25C

//Configuración sensor temperatura
OneWire ourWire(15);                //Se establece el pin 4  como bus OneWire para el sensor de temperatura
DallasTemperature sensors(&ourWire); //Se declara una variable u objeto para nuestro sensor

//Configuración variables
bool ok_read = false;
float  voltagePH, voltageEC, phValue, ecValue, temperature = 19;
char phchar[7];
char ecchar[7];
char tempchar[7];
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

 
// The MQTT topics, cargar información y recibir información
#define AWS_IOT_PUBLISH_TOPIC "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/send"

WiFiClientSecure net;
//MQTTClient client = MQTTClient(256);
MqttClient client(net);

void connectAWS(){
  ledcWrite(redChannel, 10);
  ledcWrite(greenChannel, 255);
  ledcWrite(blueChannel, 255);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  ledcWrite(redChannel, 255);
  ledcWrite(greenChannel, 255);
  ledcWrite(blueChannel, 10);

  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);
  client.setId(THINGNAME);

  if (!client.connect(AWS_IOT_ENDPOINT, 8883)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(client.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();

  // set the message receive callback
  //client.onMessage(callback);

  int subscribeQos = 0;

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC,subscribeQos);
  //client.subscribe(AWS_IOT_PUBLISH_TOPIC);

  Serial.println("AWS IoT Connected!");
  ledcWrite(redChannel, 255);
  ledcWrite(greenChannel, 10);
  ledcWrite(blueChannel, 255);
}

void publishMessage(int deviceID, char* latchar, char* lonchar,char* tempchar, char* ecchar, char* phchar){
  StaticJsonDocument<512> doc;
  JsonObject info = doc.to<JsonObject>();
  info["deviceId"] = deviceID;
  info["latitude"] = latchar;
  info["longitude"] = lonchar;
  JsonObject measurementValues = info.createNestedObject("measurementValues");
  measurementValues["Temperatura del Agua [°Celsius]"] = tempchar;
  measurementValues["Conductividad [µs/cm]"] = ecchar;
  measurementValues["pH [Unidades de pH]"] = phchar;
  info["takenAt"]= "2023-11-28T13:30:10Z";
  info["apiKey"]= "a75527eedf103e9d657b2ffe8b1e8a0e";
  
  char jsonBuffer[512];
  
  serializeJson(info, jsonBuffer); // print to client
  Serial.println(jsonBuffer);
  client.beginMessage(AWS_IOT_PUBLISH_TOPIC);
  client.print(jsonBuffer);
  client.endMessage();
  //serializeJson(info, Serial); // print to client
  
  
}

void callback(char* topic, byte* payload, unsigned int length) {
String incommingMessage = " ";
for (int i = 0; i < length; i++) incommingMessage+=(char)payload[i];
Serial.println("Message arrived [" + String(topic) + "] " + incommingMessage);
// check for other commands
/* else if( strcmp(topic,command2_topic) == 0){
if (incommingMessage.equals(“1”)) { } // do something else
}
*/
}
void setup(){
  Serial.begin(115200);
  ledcSetup(redChannel, freq, resolution);
  ledcSetup(greenChannel, freq, resolution);
  ledcSetup(blueChannel, freq, resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(redPin, redChannel);
  ledcAttachPin(greenPin, greenChannel);
  ledcAttachPin(bluePin, blueChannel);
  sensors.begin();
  connectAWS();
}

void loop() {
  
  client.poll();
  static unsigned long timepoint = millis();
  if(millis()-timepoint>4500U){
    //time interval: 1s
    timepoint = millis();
    sensors.requestTemperatures();   //Se envía el comando para leer la temperatura
    temperature = sensors.getTempCByIndex(0);
    dtostrf(temperature,7,3,tempchar);
    Serial.print("Temperatura: ");
    Serial.print(tempchar);
    Serial.println(" ºC");
    
    voltagePH = analogRead(PH_PIN)/4095.0*3300;          // read the ph voltage
    //Serial.print("PHconver: ");
    //Serial.println(voltagePH);
    slopePH = (7.0-4.0)/((neutralVoltage-1500)/3.0 - (acidVoltage-1500)/3.0);  // two point: (_neutralVoltage,7.0),(_acidVoltage,4.0)
    interceptPH =  7.0 - slopePH*(neutralVoltage-1500)/3.0;
    phValue = slopePH*(voltagePH-1500)/3.0+interceptPH;  //y = k*x + b
    dtostrf(phValue,7,3,phchar);
    Serial.print("pH: ");
    Serial.println(phchar);
    voltageEC = analogRead(EC_PIN)/4095.0*3300;
    //Serial.print("ECconver: ");
    //Serial.println(voltageEC);
    slopeEC = (12.88-1.41)/((highec) - (lowec));  
    interceptEC =  (12.88 - slopeEC*highec);
    ecValue = slopeEC*voltageEC + interceptPH-6.44 ;  //y = k*x + b
    //ecValue = rawEC * kValue;             //calculate the EC value after automatic shift
    //ecValue = ecValue / (1.0+0.0185*(temperature-25.0));  //temperature compensation
    dtostrf(ecValue,7,3,ecchar);
    Serial.print("EC: ");
    Serial.print(ecchar);
    Serial.println(" ms/cm");
    
    ok_read=true;
    }
    if(ok_read == true){
      dtostrf(latitude,10,6,latchar);
      dtostrf(longitude,10,6,lonchar);
      ledcWrite(redChannel, 255);
      ledcWrite(greenChannel, 255);
      ledcWrite(blueChannel, 255);
      publishMessage(deviceID,latchar,lonchar,tempchar,ecchar,phchar);
      ok_read=false;
      delay(500);
      ledcWrite(redChannel, 255);
      ledcWrite(greenChannel, 10);
      ledcWrite(blueChannel, 255);
      }
}