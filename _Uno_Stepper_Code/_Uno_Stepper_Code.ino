// === Arduino Uno (Stepper Controller) ===
#include <AccelStepper.h>
#include <SoftwareSerial.h>

int Homing1 = 1;
int Homing2 = 1;
int Homing3 = 1;
int HomingStepValue1 = 0;
int HomingStepValue2 = 0;
int HomingStepValue3 = 0;

int Dispense = 0;
int ZStepValue = 0;

#define VIBRATION_RELAY_PIN A2
#define STEPS_PER_REV 200  // Adjust if your motor has a different steps/rev

AccelStepper stepper1(1, 13, 12); // X
AccelStepper stepper2(1, 11, 10); // Z
AccelStepper stepper3(1, 9, 8);   // ROTARY (360°)
AccelStepper stepper4(1, 7, 6);   // Slide

void setup() {
  pinMode(A0, OUTPUT);    // Not currently used
  pinMode(A1, INPUT);     // Simulated photoresistor
  pinMode(VIBRATION_RELAY_PIN, OUTPUT);
  digitalWrite(VIBRATION_RELAY_PIN, LOW);

  Serial.begin(9600);
  Serial.println("Power On");

  // Stepper settings
  for (int i = 1; i <= 4; i++) {
    AccelStepper* s = (i == 1) ? &stepper1 :
                      (i == 2) ? &stepper2 :
                      (i == 3) ? &stepper3 :
                                 &stepper4;
    s->setMaxSpeed(1000);
    s->setAcceleration(1000);
    s->setCurrentPosition(0);
  }
}

void loop() {
  if (Homing1 != 0 && Homing2 != 0 && Homing3 != 0) {
    // Homing Stepper 1 (X)
    stepper1.setMaxSpeed(3000);
    stepper1.setAcceleration(3000);
    Serial.println("Stepper 1 is Homing");
    while (digitalRead(4) != HIGH) {
      stepper1.moveTo(HomingStepValue1++);
      stepper1.run();
      delay(2);
    }
    stepper1.setCurrentPosition(0);
    Serial.println("Homing 1 Completed");
    Homing1 = 0;

    // Homing Stepper 4 (Slide)
    stepper4.setMaxSpeed(3000);
    stepper4.setAcceleration(3000);
    Serial.println("Stepper 4 is Homing");
    while (digitalRead(2) != HIGH) {
      stepper4.moveTo(--HomingStepValue2);
      stepper4.run();
      delay(2);
    }
    stepper4.setCurrentPosition(0);
    Serial.println("Homing 2 Completed");
    Homing2 = 0;

    // Homing Stepper 2 (Z)
    stepper2.setMaxSpeed(3000);
    stepper2.setAcceleration(3000);
    Serial.println("Stepper 2 is Homing");
    while (digitalRead(3) != HIGH) {
      stepper2.moveTo(--HomingStepValue3);
      stepper2.run();
      delay(2);
    }
    stepper2.setCurrentPosition(0);
    Serial.println("Homing 3 Completed");
    Homing3 = 0;

    Serial.println("HOMED");

    // After homing, rotate Stepper 3 (360° spin)
    Serial.println("Rotating Stepper 3 (360 degrees)");
    stepper3.moveTo(STEPS_PER_REV);
    stepper3.runToPosition();
    stepper3.setCurrentPosition(0);
    Serial.println("Rotation complete");
  }

  // Simulated photoresistor check
  if (analogRead(A1) < 500) {
    Dispense = 1;
  }

  if (Dispense != 0) {
    delay(1000);

    // Move X Axis
    stepper1.moveTo(-350);
    stepper1.runToPosition();

    // Activate vibration motor
    Serial.println("Activating vibration relay...");
    digitalWrite(VIBRATION_RELAY_PIN, HIGH);
    delay(1000);
    digitalWrite(VIBRATION_RELAY_PIN, LOW);
    Serial.println("Vibration done");

    // Z axis moves down
    ZStepValue = 0;
    while (ZStepValue != -500) {
      stepper2.moveTo(ZStepValue);
      ZStepValue--;
      stepper2.run();
      delay(2);
    }

    Dispense = 0;
  }
}