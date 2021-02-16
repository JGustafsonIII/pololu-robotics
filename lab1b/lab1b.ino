#include <Pololu3piPlus32U4.h>

using namespace Pololu3piPlus32U4;

Encoders encoders; 
Buzzer buzzer;
Motors motors;
ButtonA buttonA;

unsigned long currentMillis; 
unsigned long prevMillis;

// Represents the amount of time we want to wait in milliseconds
const unsigned long PERIOD = 20;

long countsLeft = 0;
long countsRight = 0;
long prevLeft = 0;
long prevRight = 0;

const int CLICKS_PER_ROTATION = 12;
const float GEAR_RATIO = 75.81F;
const float WHEEL_DIAMETER = 3.2;
const float WHEEL_CIRCUMFERENCE = 10.0531;

float Sl = 0.0F;
float Sr = 0.0F;

// GEAR RATIO FOR TURTLE EDITION IS 75.81

/*
  Setup is only run once when it comes to the arduino.
*/
void setup() {
  Serial.begin(57600);
  delay(1000);
  buzzer.play("c32");
}

/*
  This is basically the lifecycle loop so looping in here should be avoided.
  You should basically picture the method as a loop itself so every few millseconds
  this will run checkEncoders() as an example.
*/
void loop() {
  checkEncoders();
}

void checkEncoders() {
  currentMillis = millis();
  if (currentMillis > prevMillis + PERIOD) {
    countsLeft += encoders.getCountsAndResetLeft();
    countsRight += encoders.getCountsAndResetRight();

    Sl += ((countsLeft - prevLeft) / (CLICKS_PER_ROTATION * GEAR_RATIO) * WHEEL_CIRCUMFERENCE);
    Sr += ((countsRight - prevRight) / (CLICKS_PER_ROTATION * GEAR_RATIO) * WHEEL_CIRCUMFERENCE);

    int wheelSpeed = 75;
    
    if(Sr < 30.1) {

      if(Sl > 20) {
        wheelSpeed = 75 * ((30.1 - Sr) / 10);
        if(wheelSpeed < 20) {
          wheelSpeed = 20;
        }
      }
      
      motors.setSpeeds(wheelSpeed, wheelSpeed);
    } else {
      motors.setSpeeds(0, 0);
    }
    
    Serial.print("Left: ");
    Serial.print(Sl);
    Serial.print(" Right: ");
    Serial.println(Sr);

    prevLeft = countsLeft;
    prevRight = countsRight;
    prevMillis = currentMillis;
  }
}
