#include <AccelStepper.h>

// Define pin connections and motor interface type
#define DIR_PIN 12
#define STEP_PIN 13
#define EN_PIN 5

AccelStepper stepper(1, 13, 12);  // DRIVER = Step/Dir mode

void setup() {
  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, LOW);  // Enable the driver

  stepper.setMaxSpeed(10000);      // Max speed in steps/sec
  stepper.setAcceleration(1000);   // Acceleration in steps/sec^2
}

void loop() {
  // Move 100 steps forward
  stepper.moveTo(100);
  stepper.runToPosition();  // Blocking

  delay(200);  // Optional pause

  // Move 100 steps backward
  stepper.moveTo(-100);
  stepper.runToPosition();  // Blocking

  delay(200);  // Optional pause
}