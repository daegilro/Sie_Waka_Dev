#define PH_PIN 33
float acidVoltage    = 2383;    //buffer solution 4.0 at 25C
float neutralVoltage = 1775;     //buffer solution 7.0 at 25C
float slopePH,interceptPH, slopeEC,interceptEC = 1;

#define EC_PIN 32
#define RES2 820.0
#define ECREF 200.0
float kValue =0.966;
bool ok_read = false;
float  voltagePH, voltageEC, phValue, ecValue, temperature = 19;

float readTemperature()
{
  //add your code here to get the temperature from your temperature sensor
}

void setup() {
  Serial.begin(115200);
  
}

void loop() {
  
  static unsigned long timepoint = millis();
  if(millis()-timepoint>1000U){
    //time interval: 1s
    timepoint = millis();
    //temperature = readTemperature();                   // read your temperature sensor to execute temperature compensation
    Serial.print("Raw_PH: ");
    Serial.print(analogRead(PH_PIN)),2;
    Serial.print(" Raw_EC: ");
    Serial.println(analogRead(EC_PIN),2);
    voltagePH = analogRead(PH_PIN)/4095.0*3300;          // read the ph voltage
    Serial.print("PHconver: ");
    Serial.println(voltagePH);
    slopePH = (7.0-4.0)/((neutralVoltage-1500)/3.0 - (acidVoltage-1500)/3.0);  // two point: (_neutralVoltage,7.0),(_acidVoltage,4.0)
    interceptPH =  7.0 - slopePH*(neutralVoltage-1500)/3.0;
    phValue = slopePH*(voltagePH-1500)/3.0+interceptPH;  //y = k*x + b
    Serial.print(" pH: ");
    Serial.println(phValue,2);
    voltageEC = analogRead(EC_PIN)/4095.0*3300;
    Serial.print("ECconver: ");
    Serial.println(voltageEC);
    float rawEC = 1000*voltageEC/RES2/ECREF;
    ecValue = rawEC * kValue;             //calculate the EC value after automatic shift
    //ecValue = ecValue / (1.0+0.0185*(temperature-25.0));  //temperature compensation
    Serial.print(", EC: ");
    Serial.print(ecValue,2);
    Serial.println(" ms/cm");
    ok_read=true;
    }
    //if(ok_read == true){
      //publishMessage(String(temperature),String(ecValue),String(phValue));
      //ok_read=false;
      //}
    //if(readSerial(cmd)){
      //strupr(cmd);
       // if(strstr(cmd,"PH")){
           // ph.calibration(voltagePH,temperature,cmd);       //PH calibration process by Serail CMD
        //}
        //if(strstr(cmd,"EC")){
            //ec.calibration(voltageEC,temperature,cmd);       //EC calibration process by Serail CMD
        //}
}
