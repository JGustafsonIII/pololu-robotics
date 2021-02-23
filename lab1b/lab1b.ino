#include <Pololu3piPlus32U4.h>

using namespace Pololu3piPlus32U4;

Encoders encoders;
Buzzer buzzer;
Motors motors;
ButtonA buttonA;
ButtonB buttonB;
LCD lcd;

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

float foot = 30.5F;           // 1ft
float eightteenInches = 45.72F; // 18inch
float threeFeet = (foot * 3); // 3ft

float Sl = 0.0F;
float Sr = 0.0F;

boolean finished = false;
boolean taskComplete = false;
int task = 0;

/*
  Setup is only run once when it comes to the arduino.
*/
void setup()
{
  Serial.begin(57600);
  delay(1000);
  buzzer.play("c32");
  lcd.clear();
  lcd.write("Lab 1b");
  delay(2000);
}

/*
  This is basically the lifecycle loop so looping in here should be avoided.
  You should basically picture the method as a loop itself so every few millseconds
  this will run checkEncoders() as an example.
*/
void loop()
{
  if (finished == false)
  {
    checkEncoders();

    if (task == 0)
    {
      taskComplete = moveForeward(foot);
      if (taskComplete == true)
      {
        task = 1;
        taskComplete = false;
      }
    }
    else if (task == 1)
    {
      taskComplete = moveBackward(-foot);
      if (taskComplete == true)
      {
        taskComplete = false;
        task = 2;
      }
    }
    else if (task == 2)
    {
      taskComplete = moveForeward(eightteenInches);
      if (taskComplete == true)
      {
        taskComplete = false;
        task = 3;
        finished = true;
      }
    }
  }
  else
  {
    endProgram();
    Serial.println("Tasks Completed");
  }
}

void endProgram() {
  buzzer.play("c32");
  while(1); // Easy way of stopping the program
}

/**
 * Used for testing purposed to switch between running and not running.
 * @returns true if finished = false and false if finished = true
 */
boolean onOffSwitch()
{
  if (finished == false)
  {
    motors.setSpeeds(0, 0);
    return true;
  }
  else
  {
    return false;
  }
}

/**
 * Used to reset the robots speed and set a delay 
*/
void finishedTask(int wait)
{
  motors.setSpeeds(0, 0);
  Sl = 0.0F;
  Sr = 0.0F;
  delay(wait);
}

/**
 * Moves the robot forwards based off the distance passed in
 * @param distance The distance you want the robot to travel in metric (Ex. 1ft - 30.5cm)
 * @returns Returns true if succesful and false if something went wrong
*/
boolean moveForeward(float distance)
{
  int wheelSpeed = 300;
  if (Sl < distance && Sr < distance)
  {

    if (Sl > (distance * .75))
    {
      wheelSpeed = 50 * ((distance - Sr) / 10);
      if (wheelSpeed < 30)
        wheelSpeed = 30;
    }
    else if (Sl > (distance * .50))
    {
      wheelSpeed = 100 * ((distance - Sr) / 10);
    }
    else if (Sl > (distance * .25))
    {
      wheelSpeed = 200 * ((distance - Sr) / 10);
    }
    else if (Sl < (distance *.25))
    {
      wheelSpeed = 100 * ((distance - Sr) / 10);
    }

    motors.setSpeeds(wheelSpeed, wheelSpeed);
  }
  else
  {
    finishedTask(2000);
    return true;
  }
  return false;
}

/**
 * Moves the robot backwards based off the distance passed in
 * @param distance The distance you want the robot to travel in metric (Ex. 1ft - 30.5cm)
 * @returns Returns true if succesful and false if something went wrong
*/
boolean moveBackward(float distance)
{
  int wheelSpeed = -300;

  if (Sl > distance && Sr > distance)
  {
    if (Sl < (distance * .75))
    {
      wheelSpeed = 50 * ((distance - Sr) / 10);
      if (wheelSpeed < 30)
        wheelSpeed = -30;
    }
    else if (Sl < (distance * .50))
    {
      wheelSpeed = 100 * ((distance - Sr) / 10);
    }
    else if (Sl < (distance * .25))
    {
      wheelSpeed = 200 * ((distance - Sr) / 10);
    }
    else if (Sl > (distance *.25))
    {
      wheelSpeed = 100 * ((distance - Sr) / 10);
    }

    motors.setSpeeds(wheelSpeed, wheelSpeed);

  }
  else
  {
    finishedTask(2000);
    return true;
  }
  return false;
}

/**
 * Used to calculate the distance on the wheels and print them into the serial monitor/plotter
*/
void checkEncoders()
{
  currentMillis = millis();
  if (currentMillis > prevMillis + PERIOD)
  {
    countsLeft += encoders.getCountsAndResetLeft();
    countsRight += encoders.getCountsAndResetRight();

    Sl += ((countsLeft - prevLeft) / (CLICKS_PER_ROTATION * GEAR_RATIO) * WHEEL_CIRCUMFERENCE);
    Sr += ((countsRight - prevRight) / (CLICKS_PER_ROTATION * GEAR_RATIO) * WHEEL_CIRCUMFERENCE);

    Serial.print("Left: ");
    Serial.print(Sl);
    Serial.print(" Right: ");
    Serial.println(Sr);

    prevLeft = countsLeft;
    prevRight = countsRight;
    prevMillis = currentMillis;
  }
}
