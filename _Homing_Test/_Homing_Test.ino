// Libraries
#include <AccelStepper.h>

// Variables
int Homing1 = 0;
int Homing2 = 0;
int Homing3 = 0;
int HomingStepValue1 = 0;
int HomingStepValue2 = 0;
int HomingStepValue3 = 0;

const int LIMIT_THRESHOLD = 500;  // Analog trigger threshold

AccelStepper stepper1(1, 13, 12);
AccelStepper stepper2(1, 11, 10);
AccelStepper stepper3(1, 9, 8);
AccelStepper stepper4(1, 7, 6);

void setup() {
  Serial.begin(115200);
  Serial.println("Power On");

  Homing1 = 1;
  Homing2 = 1;
  Homing3 = 1;
  HomingStepValue1 = 0;
  HomingStepValue2 = 0;
  HomingStepValue3 = 0;

  pinMode(A7, INPUT);  // Stepper 1 analog switch
  pinMode(4, INPUT);   // Stepper 2 analog switch
  pinMode(5, INPUT);   // Stepper 3 analog switch

  stepper1.setMaxSpeed(1000);
  stepper2.setMaxSpeed(1000);
  stepper3.setMaxSpeed(1000);
  stepper4.setMaxSpeed(1000);

  stepper1.setAcceleration(1000);
  stepper2.setAcceleration(1000);
  stepper3.setAcceleration(1000);
  stepper4.setAcceleration(1000);

  stepper1.setCurrentPosition(0);
  stepper2.setCurrentPosition(0);
  stepper3.setCurrentPosition(0);
  stepper4.setCurrentPosition(0);
}

void loop() {
  // Full homing sequence
  if (Homing1 || Homing2 || Homing3) {
    
    // Homing stepper 1
    if (Homing1) {
      stepper1.setMaxSpeed(100.0);
      stepper1.setAcceleration(100.0);
      Serial.println("Stepper 1 is Homing");

      HomingStepValue1 = 1;

      while (analogRead(A7) < LIMIT_THRESHOLD) {
        stepper1.moveTo(HomingStepValue1++);
        stepper1.run();
        delay(5);
      }

      stepper1.setCurrentPosition(0);
      Serial.println("Homing Switch 1 Hit");
      Serial.println("Homing 1 Completed");
      Homing1 = 0;
    }

    // Homing stepper 2
    if (Homing2) {
      stepper2.setMaxSpeed(100.0);
      stepper2.setAcceleration(100.0);
      Serial.println("Stepper 2 is Homing");

      HomingStepValue2 = 1;

      while (analogRead(4) < LIMIT_THRESHOLD) {
        stepper2.moveTo(HomingStepValue2++);
        stepper2.run();
        delay(5);
      }

      stepper2.setCurrentPosition(0);
      Serial.println("Homing Switch 2 Hit");
      Serial.println("Homing 2 Completed");
      Homing2 = 0;
    }

    // Homing stepper 3
    if (Homing3) {
      stepper3.setMaxSpeed(100.0);
      stepper3.setAcceleration(100.0);
      Serial.println("Stepper 3 is Homing");

      HomingStepValue3 = 1;

      while (analogRead(5) < LIMIT_THRESHOLD) {
        stepper3.moveTo(HomingStepValue3++);
        stepper3.run();
        delay(5);
      }

      stepper3.setCurrentPosition(0);
      Serial.println("Homing Switch 3 Hit");
      Serial.println("Homing 3 Completed");
      Homing3 = 0;
    }

    if (!Homing1 && !Homing2 && !Homing3) {
      Serial.println("Full Homing Sequence Completed");
    }
  }
}