// === Arduino Uno (Stepper Controller) ===
#include <AccelStepper.h>
#include <SoftwareSerial.h>

SoftwareSerial serialInput(4, 5); // RX = 4, TX = 5 (TX unused)

int Homing1 = 1;
int Homing2 = 1;
int Homing3 = 1;
int HomingStepValue1 = 0;
int HomingStepValue2 = 0;
int HomingStepValue3 = 0;

int Dispense = 0;
int ZStepValue = 0;

AccelStepper stepper1(1, 13, 12);
AccelStepper stepper2(1, 11, 10);
AccelStepper stepper3(1, 9, 8);
AccelStepper stepper4(1, 7, 6);

void setup() {
  pinMode(A0, OUTPUT);
  pinMode(A1, INPUT);

  Serial.begin(9600);
  serialInput.begin(9600);
  Serial.println("Power On");

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
  if (Homing1 != 0 && Homing2 != 0 && Homing3 != 0) {
    // Homing Stepper 1
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

    // Homing Stepper 2
    stepper4.setMaxSpeed(3000);
    stepper4.setAcceleration(3000);
    Serial.println("Stepper 2 is Homing");
    while (digitalRead(2) != HIGH) {
      stepper4.moveTo(--HomingStepValue2);
      stepper4.run();
      delay(2);
    }
    stepper4.setCurrentPosition(0);
    Serial.println("Homing 2 Completed");
    Homing2 = 0;

    // Homing Stepper 3
    stepper2.setMaxSpeed(3000);
    stepper2.setAcceleration(3000);
    Serial.println("Stepper 3 is Homing");
    while (digitalRead(3) != HIGH) {
      stepper2.moveTo(--HomingStepValue3);
      stepper2.run();
      delay(2);
    }
    stepper2.setCurrentPosition(0);
    Serial.println("Homing 3 Completed");
    Homing3 = 0;

    Serial.println("HOMED");
  }

  if (serialInput.available()) {
    String command = serialInput.readStringUntil('\n');
    command.trim();
    if (command == "DISPENSE") {
      Dispense = 1;
    }
  }

  if (Dispense != 0) {
    delay(1000);

    stepper1.setMaxSpeed(1000);
    stepper1.setAcceleration(1000);
    stepper1.moveTo(-350);
    stepper1.runToPosition();

    stepper4.setMaxSpeed(1000);
    stepper4.setAcceleration(1000);
    stepper4.moveTo(400);
    stepper4.runToPosition();

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