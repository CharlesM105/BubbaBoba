#include <AccelStepper.h>

// Button pins (limit switches)
const int buttonPinX = 2;
const int buttonPinY = 3;
const int buttonPinZ = 4;

// Stepper driver pin definitions
const int stepPinX = 7;
const int dirPinX = 8;
const int enablePinX = A0;

const int stepPinY = 9;
const int dirPinY = 10;
const int enablePinY = A1;

const int stepPinZ = 11;
const int dirPinZ = 12;
const int enablePinZ = A2;

int HomingStepValue1 = 0;
int HomingStepValue2 = 0;
int HomingStepValue3 = 0;

int Homing1 = 0;
int Homing2 = 0;
int Homing3 = 0;

// Create stepper instances in DRIVER mode
AccelStepper stepperX(AccelStepper::DRIVER, stepPinX, dirPinX);
AccelStepper stepperY(AccelStepper::DRIVER, stepPinY, dirPinY);
AccelStepper stepperZ(AccelStepper::DRIVER, stepPinZ, dirPinZ);

void setup() {
  Serial.begin(9600);

  // Limit switches
  pinMode(buttonPinX, INPUT);
  pinMode(buttonPinY, INPUT);
  pinMode(buttonPinZ, INPUT);

  // Motor settings
  stepperX.setMaxSpeed(500);
  stepperX.setAcceleration(300);

  stepperY.setMaxSpeed(500);
  stepperY.setAcceleration(300);

  stepperZ.setMaxSpeed(500);
  stepperZ.setAcceleration(300);

  Serial.println("Homing System");
}

void loop() {


  // HOMING X
  if (Homing1 != 0) {
    Serial.println("Homing X axis...");
    while (digitalRead(buttonPinX) != HIGH) {
stepperX.moveTo(HomingStepValue1);
HomingStepValue1++;
stepperX.run();
delay(5);
  }
  stepperX.setCurrentPosition(0);  

  stepperX.setMaxSpeed(100.0);      
  stepperX.setAcceleration(100.0); 

  HomingStepValue1 = -1;
   Serial.println("Homing Switch 1 Hit");

  stepperX.setCurrentPosition(0);
  Serial.println("Homing 1 Completed");
  
     Homing1 = 0;
 }
}