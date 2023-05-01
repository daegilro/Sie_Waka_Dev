#include "secrets.h"
#include <WiFiClientSecure.h>
//#include <MQTTClient.h>
#include <ArduinoJson.h>
//#include <Arduino_JSON.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "DFRobot_PH.h"
#include "DFRobot_EC.h"
#include <EEPROM.h>

#define PH_PIN 33
#define EC_PIN 32
bool ok_read = false;
float  voltagePH, voltageEC, phValue, ecValue, temperature = 19;
DFRobot_PH ph;
DFRobot_EC ec;

// The MQTT topics that this device should publish/subscribe
#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

WiFiClientSecure net;
//MQTTClient client = MQTTClient(256);
PubSubClient client(net);

void connectAWS()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.setServer(AWS_IOT_ENDPOINT, 8883);

  // Create a message handler
  client.setCallback(callback);

  Serial.print("Connecting to AWS IOT");

  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(100);
  }

  if(!client.connected()){
    Serial.println("AWS IoT Timeout!");
    return;
  }

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  Serial.println("AWS IoT Connected!");
}

void publishMessage(String temp,String ec,String ph){
  StaticJsonDocument<200> doc;
  doc["time"] = millis();
  doc["Temperatura"] = temp;
  doc["ElectroCond"] = ec;
  doc["pH"] = ph;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client

  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
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

//void messageHandler(String &topic, String &payload) {
//  Serial.println("incoming: " + topic + " - " + payload);
//
////  StaticJsonDocument<200> doc;
////  deserializeJson(doc, payload);
////  const char* message = doc["message"];
//}

int i = 0;
bool readSerial(char result[]){
    while(Serial.available() > 0){
        char inChar = Serial.read();
        if(inChar == '\n'){
             result[i] = '\0';
             Serial.flush();
             i=0;
             return true;
        }
        if(inChar != '\r'){
             result[i] = inChar;
             i++;
        }
        delay(1);
    }
    return false;
}

float readTemperature()
{
  //add your code here to get the temperature from your temperature sensor
}

void setup() {
  Serial.begin(115200);
  ph.begin();
  ec.begin();
  connectAWS();
}

void loop() {
  client.loop();
  char cmd[10];
  static unsigned long timepoint = millis();
  if(millis()-timepoint>1000U){
    //time interval: 1s
    timepoint = millis();
    //temperature = readTemperature();                   // read your temperature sensor to execute temperature compensation
    Serial.print("Raw_PH: ");
    Serial.print(analogRead(PH_PIN));
    Serial.print(" Raw_EC: ");
    Serial.println(analogRead(EC_PIN));
    voltagePH = analogRead(PH_PIN);//4095.0*5000;          // read the ph voltage
    phValue    = ph.readPH(voltagePH,temperature);       // convert voltage to pH with temperature compensation
    Serial.print("pH: ");
    Serial.print(phValue,2);
    voltageEC = analogRead(EC_PIN)/4095.0*5000;
    ecValue    = ec.readEC(voltageEC,temperature);       // convert voltage to EC with temperature compensation
    Serial.print(", EC: ");
    Serial.print(ecValue,2);
    Serial.println(" ms/cm");
    ok_read=true;
    }
    if(ok_read == true){
      publishMessage(String(temperature),String(ecValue),String(phValue));
      ok_read=false;
      }
    if(readSerial(cmd)){
        strupr(cmd);
        if(strstr(cmd,"PH")){
            ph.calibration(voltagePH,temperature,cmd);       //PH calibration process by Serail CMD
        }
        if(strstr(cmd,"EC")){
            ec.calibration(voltageEC,temperature,cmd);       //EC calibration process by Serail CMD
        }
     }
 
}
