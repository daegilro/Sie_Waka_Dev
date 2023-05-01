//------ Include  libraries
#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <DFRobot_EC.h>
#include <DFRobot_PH.h>
#include <EEPROM.h>

//---- WiFi settings
const char* ssid = "Familia_Isaacs";
const char* password = "39684436";
//---- MQTT Broker settings
const char* mqtt_server = "d008f2372f934a7ab0adb29438c3ef56.s2.eu.hivemq.cloud";
const char* mqtt_username = "daegilro";
const char* mqtt_password = "DavidMQTT123";
const int mqtt_port = 8883;

WiFiClientSecure espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;

#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
//------- Topoics MQTT Broker
const char* Sen_Temp_Topic = "Sen_Temp_Topic";
const char* Sen_Cond_Topic = "Sen_Cond_Topic";
const char* Sen_Ph_Topic = "Sen_Ph_Topic";
//const char* Sen_TDS_Topic= "Sensor TDS";

static const char *root_ca PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)EOF";

//------- Config Sens Conductividad ----------------------
#define EC_PIN 25
DFRobot_EC ec;
float ecvoltage,ecValue = 1;

//------ Config Sens PH------------------------------------
#define PH_PIN 26
DFRobot_PH ph;
float phvoltage,phValue = 1;

//------ Config Sens Temp------------------------------------
#define TM_PIN 27
float temperature = 19;
void setup() {

Serial.begin(115200);
//--- Setup Conduct and Temp sens

ec.begin();
ph.begin();
Serial.print("\nConnecting to ");
Serial.println(ssid);

WiFi.mode(WIFI_STA);
WiFi.begin(ssid, password);

while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
randomSeed(micros());
Serial.println("\nWiFi connected\nIP address: ");
Serial.println(WiFi.localIP());

while (!Serial) delay(1);

espClient.setCACert(root_ca);
client.setServer(mqtt_server, mqtt_port);
client.setCallback(callback);

}

void loop() {

  if (!client.connected()) reconnect();
  client.loop();

  char cmd[10];

  static unsigned long timepoint = millis();
  if(millis()-timepoint>1000U)  //time interval: 1s
  {
    timepoint = millis();
    //temperature = readTemperature();          // read your temperature sensor to execute temperature compensation
    phvoltage = analogRead(PH_PIN)/4095.0*5000;
    Serial.print("PH_Raw: ");
    Serial.println(phvoltage,1);
    phValue = ph.readPH(phvoltage,temperature);  // convert voltage to pH with temperature compensation  // convert voltage to EC with temperature compensation
    ecvoltage = analogRead(EC_PIN)/4095.0*5000;   // read the voltage
    Serial.print("EC_Raw: ");
    Serial.print(ecvoltage,2);
    ecValue =  ec.readEC(ecvoltage,temperature);
    Serial.print("temperature:");
    Serial.print(temperature,3);
    Serial.print("^C  EC:");
    Serial.print(ecValue,4);
    Serial.print("ms/cm  pH:");
    Serial.println(phValue,5);

    delay(20);
  
    publishMessage(Sen_Temp_Topic,String(temperature),true);  
    publishMessage(Sen_Cond_Topic,String(ecValue),true);  
    publishMessage(Sen_Ph_Topic,String(phValue),true);  
    //publishMessage(Sen_TDS_Topic,String(tds_sens),true);   
    }
    
    if(readSerial(cmd)){
        strupr(cmd);
        if(strstr(cmd,"PH")){
            ph. calibration(phvoltage,temperature,cmd);       //PH calibration process by Serail CMD
        }
        if(strstr(cmd,"EC")){
            ec.calibration(ecvoltage,temperature,cmd);       //EC calibration process by Serail CMD
        }
    }
  
  
}



//=======================================================================Function=================================================================================
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

void reconnect() {
// Loop until we’re reconnected
while (!client.connected()) {
Serial.print("Attempting MQTT connection…");
String clientId = "ESP32Client-Sie_Waka"; // Create a random client ID
clientId += String(random(0xffff), HEX);
// Attempt to connect
if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
Serial.println("connected");

  client.subscribe(Sen_Temp_Topic);   // subscribe the topics 
  client.subscribe(Sen_Cond_Topic);   // subscribe the topics 
  client.subscribe(Sen_Ph_Topic);   // subscribe the topics 
  //client.subscribe(Sen_TDS_Topic);   // subscribe the topics
  client.subscribe("esp32/mess_Sg1");
  client.subscribe("esp32/mess_Sg2");
  client.subscribe("Engel200/mess_Sg1");
  client.subscribe("Engel200/mess_Sg2");
} else {
  Serial.print("failed, rc=");
  Serial.print(client.state());
  Serial.println(" try again in 5 seconds");   // Wait 5 seconds before retrying
  delay(5000);
}
}
}

//=======================================
// This void is called every time we have a message from the broker

void callback(char* topic, byte* payload, unsigned int length) {
String incommingMessage = " ";
for (int i = 0; i < length; i++) incommingMessage+=(char)payload[i];
//Serial.println("Message arrived [" + String(topic) + "] " + incommingMessage);
// check for other commands
/* else if( strcmp(topic,command2_topic) == 0){
if (incommingMessage.equals(“1”)) { } // do something else
}
*/
}

//======================================= publising as string
void publishMessage(const char* topic, String payload , boolean retained){
if (client.publish(topic, payload.c_str(), true)){
  Serial.println("Message publised [" + String(topic)+ "]: "+payload);
}
}
