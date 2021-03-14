#include <Pololu3piPlus32U4.h>
using namespace Pololu3piPlus32U4;

Buzzer buzzer;
Motors motors;

// Init Ultrasonic
const int ECHO_PIN = 30;
const int TRIG_PIN = 17;

// Ultrasonic Max
const float MAX_DISTANCE = 60.96;

// Determine the nomalization factor based on MAX_DISTANCE
const float DISTANCE_FACTOR = MAX_DISTANCE / 100;
const float STOP_DISTANCE = 5;

// Motor Constants
const float MOTOR_BASE_SPEED = 200.0;
const int MOTOR_MIN_SPEED = 30;

// Motor Compensation
const float L_MOTOR_FACTOR = 1.0;
const float R_MOTOR_FACTOR = 1.0;
const float L_MOTOR_FACTOR_THRESHOLD = 60;
const float R_MOTOR_FACTOR_THRESHOLD = 60;


// Determine the nominalization factor based on MOTOR_BASE_SPEED
const float MOTOR_FACTOR = MOTOR_BASE_SPEED / 100;

// Ultrasonic Timing
//unsigned long currentMillis;
//unsigned long previousMillis;
unsigned long usCM;
unsigned long usPM;
const unsigned long US_PERIOD = 50; // Time to wait for 1st US to activate

// Motor Timing
unsigned long motorCM;
unsigned long motorPM;
const unsigned long MOTOR_PERIOD = 20;
// Current US Distance Reading
float distance = 0;
boolean move = false;
void setup() {
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  delay(1000);
  buzzer.play("c32");
}

void loop() {
  // put your main code here, to run repeatedly:
  usReadCM();

  if (distance == MAX_DISTANCE || distance == 0) {
    stopAndLook();
  } else {
    setMotors();
  }
}

void usReadCM() {
  usCM = millis();
  if (usCM > usPM + US_PERIOD) {

    // Clears the TRIG_PIN (set to low)
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);

    // Sets the TRIG_PIN HIGH (ACTIVE) for 10 microseconds
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    // Reads the ECHO_PIN, returns the sound wave travel time in microseconds
    long duration = pulseIn(ECHO_PIN, HIGH, 38000);

    // Calculate Distance
    distance = duration * 0.034 / 2; // Time of Flight Equation

    // Apply Limits
    if (distance > MAX_DISTANCE) distance = MAX_DISTANCE;
    //if (distance == 0) distance = MAX_DISTANCE;

    // Display the distance on the Serial Monitor
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    // Update previousMillis
    usPM = usCM;
  }
}



boolean stopAndLook() {
  float leftSpeed1 = 50;
  float rightSpeed1 = -50;
  motors.setSpeeds(leftSpeed1, rightSpeed1);
  delay(100);
  while (distance > 0 && distance < MAX_DISTANCE) {
    usReadCM();

    if (distance == STOP_DISTANCE) return;
    if (distance < MAX_DISTANCE) {
      motors.setSpeeds(0, 0);
      leftSpeed1 = 0;
      rightSpeed1 = 0;
    }
  }
  return;
}

void setMotors() {
  motorCM = millis();
  if (motorCM > motorPM + MOTOR_PERIOD) {
    float leftSpeed = MOTOR_BASE_SPEED;
    float rightSpeed = MOTOR_BASE_SPEED;

    if (distance < MAX_DISTANCE) {
      float magnitude = (float)(MAX_DISTANCE - distance) / DISTANCE_FACTOR;

      leftSpeed = MOTOR_BASE_SPEED - (magnitude * MOTOR_FACTOR);
      rightSpeed = MOTOR_BASE_SPEED - (magnitude * MOTOR_FACTOR);
    }

    if (leftSpeed < MOTOR_MIN_SPEED) leftSpeed = MOTOR_MIN_SPEED;
    if (rightSpeed < MOTOR_MIN_SPEED) rightSpeed = MOTOR_MIN_SPEED;

    if (leftSpeed <= L_MOTOR_FACTOR_THRESHOLD) {
      leftSpeed *= L_MOTOR_FACTOR;
    }

    if (rightSpeed < + R_MOTOR_FACTOR_THRESHOLD) {
      rightSpeed *= R_MOTOR_FACTOR;
    }

    if (distance <= STOP_DISTANCE) leftSpeed = 0;
    if (distance <= STOP_DISTANCE) rightSpeed = 0;


    Serial.print("Left: ");
    Serial.print(leftSpeed);
    Serial.print(" Right: ");
    Serial.println(rightSpeed);

    motors.setSpeeds(leftSpeed, rightSpeed);
    // Update previousMillis
    motorPM = motorCM;

  }
}
