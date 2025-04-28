// === Arduino Uno Full Drink Sequencing with Correct Homing ===

#include <AccelStepper.h>
#include <Servo.h>

// === Stepper Motors ===
AccelStepper stepperX(1, 13, 12); // X motor (stepper1)
AccelStepper stepperZ(1, 11, 10); // Z motor (stepper2)
AccelStepper stepperY(1, 7, 6);   // Y motor (stepper4)

// === Servo ===
Servo coverServo;

// === Relays ===
#define RELAY_SPINNY_EYES 2
#define RELAY_MIXER 3
#define RELAY_BOBA_SHAKER 4

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

  // Servo Settings
  coverServo.attach(A0);
  coverServo.write(0); // Closed position

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

  // === ALL SYSTEMS RESET ===
  Serial.println("Resetting Systems...");

  // === X, Y, Z Motors Home ===
  homeAllMotors();

  // === CUSTOMER SWIPES RFID CARD ===
  waitForRFID();

  // === CUSTOMER SELECTS FLAVOR ===
  selectFlavor();

  // === CUSTOMER SELECTS FLAVOR TO MILK RATIO ===
  selectMilkRatio();

  // === CUSTOMER SELECTS IF THEY WANT BOBA OR NOT ===
  selectBobaOption();

  // === CUSTOMER PRESSES START ===
  waitForStartButton();

  // === SEQUENCED MOVEMENTS ===
  runDrinkSequence();
}

void loop() {
  // Empty
}

// === Functions ===

void homeAllMotors() {
  Serial.println("Starting Homing...");

  // Reset homing step counters
  HomingStepValueX = 0;
  HomingStepValueY = 0;
  HomingStepValueZ = 0;

  // Home X
  Serial.println("Homing X...");
  while (digitalRead(LIMIT_X) != HIGH) {
    stepperX.moveTo(HomingStepValueX++);
    stepperX.run();
    if (HomingStepValueX > 10000) { Serial.println("X Homing Fail-Safety"); break; }
    delay(2);
  }
  stepperX.setCurrentPosition(0);
  Serial.println("Homing X Complete");

  // Home Y
  Serial.println("Homing Y...");
  while (digitalRead(LIMIT_Y) != HIGH) {
    stepperY.moveTo(--HomingStepValueY);
    stepperY.run();
    if (HomingStepValueY < -10000) { Serial.println("Y Homing Fail-Safety"); break; }
    delay(2);
  }
  stepperY.setCurrentPosition(0);
  Serial.println("Homing Y Complete");

  // Home Z
  Serial.println("Homing Z...");
  while (digitalRead(LIMIT_Z) != HIGH) {
    stepperZ.moveTo(--HomingStepValueZ);
    stepperZ.run();
    if (HomingStepValueZ < -10000) { Serial.println("Z Homing Fail-Safety"); break; }
    delay(2);
  }
  stepperZ.setCurrentPosition(0);
  Serial.println("Homing Z Complete");

  Serial.println("All Motors Homed");

  // === AFTER HOMING: Extend Y and Raise Z ===
  Serial.println("Extending Y and Raising Z...");

  // Move Y axis out (example: 800 steps forward)
  stepperY.setMaxSpeed(1000);
  stepperY.setAcceleration(1000);
  stepperY.moveTo(800);
  stepperY.runToPosition();
  Serial.println("Y Axis Extended Out");

  // Move Z axis up (example: 500 steps up)
  stepperZ.setMaxSpeed(1000);
  stepperZ.setAcceleration(1000);
  stepperZ.moveTo(500);
  stepperZ.runToPosition();
  Serial.println("Z Axis Raised Up");
}

void waitForRFID() {
  delay(2000); // Simulate RFID scan
  Serial.println("RFID Card Swiped");
}

void selectFlavor() {
  Serial.println("Flavor Selected");
}

void selectMilkRatio() {
  Serial.println("Milk Ratio Selected");
}

void selectBobaOption() {
  Serial.println("Boba Option Selected");
}

void waitForStartButton() {
  delay(1000); // Simulate start button
  Serial.println("Start Pressed");
}

void runDrinkSequence() {
  Serial.println("Starting Drink Sequence");

  // Stepper X move to have nozzle over cup
  stepperX.setMaxSpeed(1000);
  stepperX.setAcceleration(1000);
  stepperX.moveTo(-350);
  stepperX.runToPosition();
  Serial.println("X moved over cup");

  // Stepper Y move middle
  stepperY.setMaxSpeed(1000);
  stepperY.setAcceleration(1000);
  stepperY.moveTo(400);
  stepperY.runToPosition();
  Serial.println("Y moved to middle");

  // Stepper Z move down
  moveZDown();

  // Servo for cover rotate
  coverServo.write(90);
  delay(500);
  Serial.println("Cover Opened");

  // Enable spinny eyes
  digitalWrite(RELAY_SPINNY_EYES, HIGH);

  // Dispense milk and flavor
  delay(3000);
  Serial.println("Milk and flavor dispensed");

  // Cover tubes again / disable spinny eyes
  coverServo.write(0);
  delay(500);
  digitalWrite(RELAY_SPINNY_EYES, LOW);
  Serial.println("Cover Closed, Spinny Eyes OFF");

  // Stepper Z move up
  moveZUp();

  // Stepper X move to have mixer over cup
  stepperX.moveTo(-700);
  stepperX.runToPosition();
  Serial.println("X moved to mixer");

  // Stepper Z move down
  moveZDown();

  // Enable relay for mixer and spinny eyes
  digitalWrite(RELAY_MIXER, HIGH);
  digitalWrite(RELAY_SPINNY_EYES, HIGH);
  delay(3000);
  digitalWrite(RELAY_MIXER, LOW);
  digitalWrite(RELAY_SPINNY_EYES, LOW);
  Serial.println("Mixed drink");

  // Stepper Z move up
  moveZUp();

  // Stepper Y home
  stepperY.moveTo(0);
  stepperY.runToPosition();
  Serial.println("Y moved home");

  // Enable stepper for boba dispenser (simulate step)
  Serial.println("Dispensing Boba...");
  delay(2000);

  // Enable relay for boba dispenser shaker
  digitalWrite(RELAY_BOBA_SHAKER, HIGH);
  delay(2000);
  digitalWrite(RELAY_BOBA_SHAKER, LOW);
  Serial.println("Boba Shaker Activated");

  // Stepper Y move to front
  stepperY.moveTo(800);
  stepperY.runToPosition();
  Serial.println("Y moved to front");

  // Wait 10 seconds
  delay(10000);

  // Home X, Y, Z motors
  homeAllMotors();
}

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

/*#include <AccelStepper.h>

AccelStepper stepperX(1, 13, 12); // Stepper1 = X

#define LIMIT_X 4 // Your limit switch for X

int HomingStepValueX = 0;

void setup() {
  Serial.begin(9600);

  stepperX.setMaxSpeed(3000);
  stepperX.setAcceleration(3000);
  stepperX.setCurrentPosition(0);

  pinMode(LIMIT_X, INPUT_PULLUP);

  Serial.println("=== HOMING X START ===");
}

void loop() {
  if (digitalRead(LIMIT_X) == HIGH) {
    Serial.println("Limit switch already HIGH (pressed) at start!");
    delay(2000);
  } 
  else {
    // Start homing
    while (digitalRead(LIMIT_X) != HIGH) {
      stepperX.moveTo(HomingStepValueX++);
      stepperX.run();
      Serial.print("Position: ");
      Serial.println(stepperX.currentPosition());
      delay(2);

      // safety break
      if (HomingStepValueX > 10000) {
        Serial.println("!! FAIL-SAFE: MOVED TOO FAR WITHOUT SWITCH HIT");
        while (true) {}
      }
    }

    Serial.println("=== X HOMED ===");
    stepperX.setCurrentPosition(0);

    while (true) {
      // Stop everything
    }
  }
}*/