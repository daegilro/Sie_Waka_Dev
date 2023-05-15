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

void setup() {

  Serial.begin(115200);
  ledcSetup(redChannel, freq, resolution);
  ledcSetup(greenChannel, freq, resolution);
  ledcSetup(blueChannel, freq, resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(redPin, redChannel);
  ledcAttachPin(greenPin, greenChannel);
  ledcAttachPin(bluePin, blueChannel);
  // put your setup code here, to run once:

}

void loop() {
  
  for(int dutyCycleR = 255; dutyCycleR >= 0; dutyCycleR--){
    // changing the LED brightness with PWM
    ledcWrite(redChannel, dutyCycleR);   
    delay(15);
  }
  
  for(int dutyCycleR = 0; dutyCycleR <= 255; dutyCycleR++){   
    // changing the LED brightness with PWM
    ledcWrite(redChannel, dutyCycleR);
    delay(15);
  }
  
  delay(1000);
  for(int dutyCycleG = 255; dutyCycleG >= 0; dutyCycleG--){
    // changing the LED brightness with PWM
    ledcWrite(greenChannel, dutyCycleG);   
    delay(15);
  }
  
  for(int dutyCycleG = 0; dutyCycleG <= 255; dutyCycleG++){   
    // changing the LED brightness with PWM
    ledcWrite(greenChannel, dutyCycleG);
    delay(15);
  }
  
  delay(1000);
  for(int dutyCycleB = 255; dutyCycleB >= 0; dutyCycleB--){
    // changing the LED brightness with PWM
    ledcWrite(blueChannel, dutyCycleB);   
    delay(15);
  }
  
  for(int dutyCycleB = 0; dutyCycleB <= 255; dutyCycleB++){   
    // changing the LED brightness with PWM
    ledcWrite(blueChannel, dutyCycleB);
    delay(15);
  }
  
  delay(1000);
  

}
