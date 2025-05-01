// === Arduino Uno Full Drink Sequencing with Correct Homing + Stepper3 (Boba Dispenser) ===

#include <AccelStepper.h>
#include <Servo.h>

// === Stepper Motors ===
AccelStepper stepperX(1, 13, 12); // X motor (stepper1)
AccelStepper stepperZ(1, 11, 10); // Z motor (stepper2)
AccelStepper stepperY(1, 7, 6);   // Y motor (stepper4)
AccelStepper stepper3(AccelStepper::DRIVER, 9, 8); // Stepper3 = 360° Rotary Boba Dispenser

#define STEPS_PER_REV 200  // Adjust based on your motor

// === Servo ===
Servo coverServo;

// === Limit Switches ===
#define LIMIT_X 4
#define LIMIT_Y 2
#define LIMIT_Z 3

// === Variables ===
int HomingStepValueX = 0;
int HomingStepValueY = 0;
int HomingStepValueZ = 0;

void setup() {
  Serial.begin(9600);

  // Stepper Settings
  stepperX.setMaxSpeed(3000);
  stepperX.setAcceleration(3000);
  stepperY.setMaxSpeed(3000);
  stepperY.setAcceleration(3000);
  stepperZ.setMaxSpeed(3000);
  stepperZ.setAcceleration(3000);
  stepperX.setCurrentPosition(0);
  stepperY.setCurrentPosition(0);
  stepperZ.setCurrentPosition(0);

  stepper3.setMaxSpeed(1000);
  stepper3.setAcceleration(500);
  stepper3.setCurrentPosition(0);

  // Servo Settings
  coverServo.attach(A0);
  coverServo.write(0); // Closed

  // Relay Settings
  pinMode(RELAY_SPINNY_EYES, OUTPUT);
  pinMode(RELAY_MIXER, OUTPUT);
  pinMode(RELAY_BOBA_SHAKER, OUTPUT);
  digitalWrite(RELAY_SPINNY_EYES, LOW);
  digitalWrite(RELAY_MIXER, LOW);
  digitalWrite(RELAY_BOBA_SHAKER, LOW);

  // Limit Switch Settings
  pinMode(LIMIT_X, INPUT_PULLUP);
  pinMode(LIMIT_Y, INPUT_PULLUP);
  pinMode(LIMIT_Z, INPUT_PULLUP);

  Serial.println("Resetting Systems...");
  homeAllMotors();
  waitForRFID();
  selectFlavor();
  selectMilkRatio();
  selectBobaOption();
  waitForStartButton();
  runDrinkSequence();
}

void loop() {
  // Empty
}

// === Homing ===
void homeAllMotors() {
  Serial.println("Starting Homing...");
  HomingStepValueX = 0;
  HomingStepValueY = 0;
  HomingStepValueZ = 0;

  Serial.println("Homing X...");
  while (digitalRead(LIMIT_X) != HIGH) {
    stepperX.moveTo(HomingStepValueX++);
    stepperX.run();
    if (HomingStepValueX > 10000) break;
    delay(2);
  }
  stepperX.setCurrentPosition(0);
  Serial.println("Homing X Complete");

  Serial.println("Homing Y...");
  while (digitalRead(LIMIT_Y) != HIGH) {
    stepperY.moveTo(--HomingStepValueY);
    stepperY.run();
    if (HomingStepValueY < -10000) break;
    delay(2);
  }
  stepperY.setCurrentPosition(0);
  Serial.println("Homing Y Complete");

  Serial.println("Homing Z...");
  while (digitalRead(LIMIT_Z) != HIGH) {
    stepperZ.moveTo(--HomingStepValueZ);
    stepperZ.run();
    if (HomingStepValueZ < -10000) break;
    delay(2);
  }
  stepperZ.setCurrentPosition(0);
  Serial.println("Homing Z Complete");

  Serial.println("All Motors Homed");

  Serial.println("Extending Y and Raising Z...");
  stepperY.setMaxSpeed(1000);
  stepperY.setAcceleration(1000);
  stepperY.moveTo(800);
  stepperY.runToPosition();
  Serial.println("Y Axis Extended Out");

  stepperZ.setMaxSpeed(1000);
  stepperZ.setAcceleration(1000);
  stepperZ.moveTo(500);
  stepperZ.runToPosition();
  Serial.println("Z Axis Raised Up");
}

// === Simulated Menu Inputs ===
void waitForRFID() { delay(2000); Serial.println("RFID Card Swiped"); }
void selectFlavor() { Serial.println("Flavor Selected"); }
void selectMilkRatio() { Serial.println("Milk Ratio Selected"); }
void selectBobaOption() { Serial.println("Boba Option Selected"); }
void waitForStartButton() { delay(1000); Serial.println("Start Pressed"); }

// === Main Drink Sequence ===
void runDrinkSequence() {
  Serial.println("Starting Drink Sequence");

  stepperX.setMaxSpeed(1000);
  stepperX.setAcceleration(1000);
  stepperX.moveTo(-350);
  stepperX.runToPosition();
  Serial.println("X moved over cup");

  stepperY.moveTo(400);
  stepperY.runToPosition();
  Serial.println("Y moved to middle");

  moveZDown();

  coverServo.write(90);
  delay(500);
  Serial.println("Cover Opened");

  digitalWrite(RELAY_SPINNY_EYES, HIGH);
  delay(3000);
  Serial.println("Milk and flavor dispensed");

  coverServo.write(0);
  delay(500);
  digitalWrite(RELAY_SPINNY_EYES, LOW);
  Serial.println("Cover Closed, Spinny Eyes OFF");

  moveZUp();

  stepperX.moveTo(-700);
  stepperX.runToPosition();
  Serial.println("X moved to mixer");

  moveZDown();

  digitalWrite(RELAY_MIXER, HIGH);
  digitalWrite(RELAY_SPINNY_EYES, HIGH);
  delay(3000);
  digitalWrite(RELAY_MIXER, LOW);
  digitalWrite(RELAY_SPINNY_EYES, LOW);
  Serial.println("Mixed drink");

  moveZUp();

  stepperY.moveTo(0);
  stepperY.runToPosition();
  Serial.println("Y moved home");

  // === NEW: Stepper3 Spins to Dispense Boba ===
  Serial.println("Dispensing Boba...");
  stepper3.moveTo(STEPS_PER_REV);  // One full rotation
  stepper3.runToPosition();
  delay(500);
  stepper3.moveTo(0);              // Return
  stepper3.runToPosition();
  delay(500);

  digitalWrite(RELAY_BOBA_SHAKER, HIGH);
  delay(2000);
  digitalWrite(RELAY_BOBA_SHAKER, LOW);
  Serial.println("Boba Shaker Activated");

  stepperY.moveTo(800);
  stepperY.runToPosition();
  Serial.println("Y moved to front");

  delay(10000);

  homeAllMotors();
}

// === Z Movement Helpers ===
void moveZDown() {
  int ZStepValue = 0;
  while (ZStepValue != -500) {
    stepperZ.moveTo(ZStepValue);
    ZStepValue--;
    stepperZ.run();
    delay(2);
  }
  Serial.println("Z moved down");
}

void moveZUp() {
  stepperZ.moveTo(0);
  stepperZ.runToPosition();
  Serial.println("Z moved up");
}