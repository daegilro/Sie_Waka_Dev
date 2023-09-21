// LLamado de librerias a utilizar
#include "secrets.h"
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>                
#include <DallasTemperature.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Configuracion Identificador  modulo
float deviceID = 1;

//Configuracion ubicacion modulo
float latitude = -34.62994536; 
float longitude = -58.39187918;

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

//Configuración pantalla
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define down_pin 5
#define up_pin 4
#define enter_pin 16
#define back_pin 17
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

 
// The MQTT topics, cargar información y recibir información
#define AWS_IOT_PUBLISH_TOPIC   "samples/upload"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

WiFiClientSecure net;
//MQTTClient client = MQTTClient(256);
PubSubClient client(net);

static const uint8_t image_data_Siewaka[1024] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xcf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0x87, 0xff, 0xff, 0xf0, 0x3e, 0x7f, 0xc7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0x03, 0xff, 0xff, 0xc0, 0x0c, 0x3f, 0x8f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xfe, 0x09, 0xff, 0xff, 0x80, 0x06, 0x7f, 0x9f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xfc, 0x0c, 0xff, 0xff, 0x8f, 0x87, 0xff, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xf8, 0x0e, 0xff, 0xff, 0x0f, 0xef, 0xfe, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xf8, 0x0f, 0x7f, 0xff, 0x1f, 0xfe, 0x78, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xf0, 0x0f, 0x3f, 0xff, 0x8f, 0xfc, 0x70, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xe0, 0x0f, 0x9f, 0xff, 0x81, 0xfc, 0x71, 0xe1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xc0, 0x0e, 0x0f, 0xff, 0xc0, 0x3c, 0x63, 0xf1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xc6, 0x08, 0x4f, 0xff, 0xe0, 0x0c, 0x63, 0xf1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0x8e, 0x01, 0xe7, 0xff, 0xfe, 0x04, 0x60, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0x0c, 0x07, 0xf7, 0xff, 0xff, 0x84, 0x60, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0x08, 0x0f, 0xf3, 0xff, 0xff, 0xc4, 0x63, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xfe, 0x38, 0x0f, 0xfb, 0xff, 0xbf, 0xc4, 0x63, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xfe, 0x30, 0x0f, 0xf9, 0xff, 0x0f, 0x84, 0x61, 0xf3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xfc, 0x70, 0x0f, 0xf9, 0xff, 0x00, 0x0c, 0x70, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xfc, 0x60, 0x0f, 0xe0, 0xff, 0x80, 0x1c, 0x78, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xfc, 0x60, 0x0f, 0x80, 0xff, 0xe0, 0x3e, 0x7e, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xfc, 0xe0, 0x0c, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xf8, 0xc0, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xf8, 0xc0, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xf8, 0xc0, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xf9, 0xc0, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xf9, 0xc0, 0x00, 0x00, 0x7f, 0x9f, 0xff, 0xf1, 0xff, 0xfe, 0x3f, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xf8, 0x80, 0x00, 0x00, 0x7f, 0x0f, 0xff, 0xf1, 0xff, 0xfe, 0x3f, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xf8, 0x00, 0x00, 0x00, 0x7f, 0x0f, 0xff, 0xe1, 0xff, 0xfe, 0x3f, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xf8, 0x00, 0x00, 0x00, 0x7f, 0x8f, 0xff, 0xe3, 0xff, 0xfe, 0x3f, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xf8, 0x00, 0x00, 0x00, 0x7f, 0x8f, 0xc7, 0xe3, 0xe0, 0xce, 0x3e, 0x7c, 0x19, 0xff, 0xff, 
    0xff, 0xf8, 0x00, 0x00, 0x00, 0xff, 0x87, 0xc3, 0xc3, 0x80, 0x0e, 0x3c, 0x78, 0x00, 0xff, 0xff, 
    0xff, 0xfc, 0x00, 0x00, 0x00, 0xff, 0xc7, 0x83, 0xc7, 0x00, 0x0e, 0x38, 0x70, 0x00, 0xff, 0xff, 
    0xff, 0xfc, 0x00, 0x00, 0x00, 0xff, 0xc7, 0x83, 0xc7, 0x1f, 0x0e, 0x30, 0xe1, 0xe0, 0xff, 0xff, 
    0xff, 0xfc, 0x00, 0x00, 0x01, 0xff, 0xc3, 0x81, 0x86, 0x1f, 0x8e, 0x21, 0xe3, 0xf0, 0xff, 0xff, 
    0xff, 0xfe, 0x00, 0x00, 0x01, 0xff, 0xe3, 0x11, 0x8e, 0x3f, 0x8e, 0x03, 0xe3, 0xf8, 0xff, 0xff, 
    0xff, 0xff, 0x00, 0x00, 0x03, 0xff, 0xe3, 0x18, 0x8e, 0x3f, 0x8e, 0x07, 0xe3, 0xf8, 0xff, 0xff, 
    0xff, 0xff, 0x00, 0x00, 0x03, 0xff, 0xe0, 0x38, 0x0e, 0x3f, 0x8e, 0x03, 0xe3, 0xf8, 0xff, 0xff, 
    0xff, 0xff, 0x80, 0x00, 0x07, 0xff, 0xf0, 0x38, 0x1e, 0x3f, 0x8e, 0x01, 0xe3, 0xf8, 0xff, 0xff, 
    0xff, 0xff, 0xc0, 0x00, 0x0f, 0xff, 0xf0, 0x7c, 0x1e, 0x1f, 0x8e, 0x30, 0xe3, 0xf0, 0xff, 0xff, 
    0xff, 0xff, 0xe0, 0x00, 0x1f, 0xff, 0xf8, 0x7c, 0x1f, 0x0e, 0x0e, 0x38, 0x71, 0xe0, 0xff, 0xff, 
    0xff, 0xff, 0xf0, 0x00, 0x7f, 0xff, 0xf8, 0x7e, 0x3f, 0x80, 0x0e, 0x3c, 0x30, 0x00, 0xff, 0xff, 
    0xff, 0xff, 0xfc, 0x00, 0xff, 0xff, 0xf8, 0xfe, 0x3f, 0xc0, 0x0e, 0x7e, 0x38, 0x08, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xcf, 0xff, 0xff, 0xfd, 0xff, 0x7f, 0xe1, 0xdf, 0xff, 0xfe, 0x1d, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};


int selected = 0;
int entered = -1;

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
    ledcWrite(redChannel, 10);
    ledcWrite(greenChannel, 255);
    ledcWrite(blueChannel, 255);
    return;
  }

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  Serial.println("AWS IoT Connected!");
  ledcWrite(redChannel, 255);
  ledcWrite(greenChannel, 10);
  ledcWrite(blueChannel, 255);
}

void publishMessage(float deviceID, float latitude, float longitude,float temperature, float ecValue, float phValue){
  
  StaticJsonDocument<256> doc;
  JsonObject info = doc.to<JsonObject>();
  info["deviceId"] = deviceID;
  info["latitude"] = latitude;
  info["longitude"] = longitude;
  JsonObject measurementValues = info.createNestedObject("measurementValues");
  measurementValues["Temperatura del Agua [°Celsius]"] = temperature;
  measurementValues["Conductividad [µs/cm]"] = ecValue;
  measurementValues["pH [Unidades de pH]"] = phValue;
  info["takenAt"]= "2023-08-24T13:10:10Z";
  info["apiKey"]= "589be0f62f4502b8490eccaf748cf194";
  
  char jsonBuffer[512];
  
  serializeJson(info, jsonBuffer); // print to client

  serializeJson(info, Serial); // print to client
  

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

void displaymenu(void) {

  int down = analogRead(down_pin);
  int up = analogRead(up_pin);
  int enter = digitalRead(enter_pin);
  int back = digitalRead(back_pin);
  Serial.print(down);
  Serial.print('\t');
  Serial.print(up);
  Serial.print('\t');
  Serial.print(enter);
  Serial.print('\t');
  Serial.println(back);

  if (up == 0 && down == 0) {
  };
  if (up == 0) {
    selected = selected + 1;
    delay(200);
  };
  if (down == 0) {
    selected = selected - 1;
    delay(200);
  };
  if (enter == 0) {
    entered = selected;
  };
  if (back == 0) {
    entered = -1;
  };
  const char *options[4] = {
    " Medicion OFF LINE ",
    " Medicion ON LINE ",
    " Calibrar ",
    " Info Device ",
  };

  if (entered == -1) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("Menu Principal"));
    display.println("");
    for (int i = 0; i < 4; i++) {
      if (i == selected) {
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
        display.println(options[i]);
      } else if (i != selected) {
        display.setTextColor(SSD1306_WHITE);
        display.println(options[i]);
      }
    }
  } else if (entered == 0) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("MODULO SIE WAKA"));
    display.println("Modo OFF LINE");
    display.setTextColor(SSD1306_WHITE);
    display.print("Temperatura: ");// demas info
    display.print(temperature);
    display.println(" ºC");
    display.print("Electrocond: ");// demas info
    display.print(ecValue);
    display.println(" ms/cm");
    display.print("ph: ");// demas info
    display.println(phValue);    
  } else if (entered == 1) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("MODULO SIE WAKA"));
    display.println("Modo ON LINE");
    display.setTextColor(SSD1306_WHITE);
    display.print("Temperatura: ");// demas info
    display.print(temperature);
    display.println(" ºC");
    display.print("Electrocond: ");// demas info
    display.print(ecValue);
    display.println(" ms/cm");
    display.print("ph: ");// demas info
    display.println(phValue);    
  }
  else if (entered == 2) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("MODULO SIE WAKA"));
    display.println("Calibracion");
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(2);
    display.println("Calibration");
  } else if (entered == 3) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("MODULO SIE WAKA"));
    display.println("Info. del Sistema");
    display.print("Device ID: ");// demas info
    display.print(deviceID);
    display.println("Latitud: ");// demas info
    display.print(latitude);
    display.println("Longitud: ");// demas info
    display.print(longitude);    
  }

  display.display();
}