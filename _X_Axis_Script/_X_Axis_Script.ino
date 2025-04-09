#include <AccelStepper.h>

// Define pin connections and motor interface type
#define DIR_PIN 12
#define STEP_PIN 13
#define EN_PIN 5

AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);  // DRIVER = Step/Dir mode

int stepsToMove = 100;

void setup() {
  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, LOW);  // Enable the driver

  stepper.setMaxSpeed(1000);      // Max speed in steps/sec
  stepper.setAcceleration(500);   // Acceleration in steps/sec^2
}

void loop() {
  // Move 100 steps forward
  stepper.moveTo(stepsToMove);
  stepper.runToPosition();  // Blocking

  delay(200);  // Optional pause

  // Move 100 steps backward
  stepper.moveTo(-stepsToMove);
  stepper.runToPosition();  // Blocking

  delay(200);  // Optional pause
}