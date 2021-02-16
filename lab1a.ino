#include <Pololu3piPlus32U4.h>
using namespace Pololu3piPlus32U4;

Encoders encoders; 
Buzzer buzzer;
Motors motors;
ButtonA buttonA;

unsigned long currentMillis;
unsigned long prevMillis;
const unsigned long PERIOD = 50;

void setup() {
  Serial.begin(57600);
  delay(1000);
  buzzer.play("c32");
}

void loop() {
  checkEncoders();
}

void checkEncoders() {
  currentMillis = millis();
  
}