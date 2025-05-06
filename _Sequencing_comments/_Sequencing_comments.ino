#include <AccelStepper.h>
#include <Servo.h>

// === Stepper Motors ===
AccelStepper stepperX(1, 13, 12); // X axis
AccelStepper stepperZ(1, 11, 10); // Z axis
AccelStepper stepperY(1, 7, 6);   // Y axis
AccelStepper stepper3(AccelStepper::DRIVER, 9, 8); // Boba Dispenser

#define STEPS_PER_REV 200

// === Limit Switches ===
#define LIMIT_X 4
#define LIMIT_Y 2
#define LIMIT_Z 3

// === Relays (optional) ===
#define RELAY_SPINNY_EYES 5
#define RELAY_MIXER 6
#define RELAY_BOBA_SHAKER 7

Servo coverServo;

void setup() {
  Serial.begin(9600);

  stepperX.setMaxSpeed(3000);
  stepperX.setAcceleration(4000);
  stepperY.setMaxSpeed(3000);
  stepperY.setAcceleration(1000);
  stepperZ.setMaxSpeed(3000);       // ← Faster Z
  stepperZ.setAcceleration(4000);   // ← Faster Z accel
  stepper3.setMaxSpeed(1000);
  stepper3.setAcceleration(500);

  stepperX.setCurrentPosition(0);
  stepperY.setCurrentPosition(0);
  stepperZ.setCurrentPosition(0);
  stepper3.setCurrentPosition(0);

  pinMode(LIMIT_X, INPUT_PULLUP);
  pinMode(LIMIT_Y, INPUT_PULLUP);
  pinMode(LIMIT_Z, INPUT_PULLUP);

  pinMode(RELAY_SPINNY_EYES, OUTPUT);
  pinMode(RELAY_MIXER, OUTPUT);
  pinMode(RELAY_BOBA_SHAKER, OUTPUT);
  digitalWrite(RELAY_SPINNY_EYES, LOW);
  digitalWrite(RELAY_MIXER, LOW);
  digitalWrite(RELAY_BOBA_SHAKER, LOW);

  coverServo.attach(A1);
  coverServo.write(0);

  Serial.println("== SYSTEM STARTING ==");
}

void loop() {
  runFullSequence();
}

// === HOMING ===
void homeAllMotors() {
  Serial.println("Fast homing X...");
  stepperX.setSpeed(500);
  while (digitalRead(LIMIT_X) != HIGH) {
    stepperX.runSpeed();
  }
  stepperX.setCurrentPosition(0);
  Serial.println("X homed");

  Serial.println("Fast homing Y...");
  stepperY.setSpeed(-500);
  while (digitalRead(LIMIT_Y) != HIGH) {
    stepperY.runSpeed();
  }
  stepperY.setCurrentPosition(0);
  Serial.println("Y homed");

  Serial.println("Fast homing Z...");
  stepperZ.setSpeed(-500);
  while (digitalRead(LIMIT_Z) != HIGH) {
    stepperZ.runSpeed();
  }
  stepperZ.setCurrentPosition(0);
  Serial.println("Z homed");

  delay(10);
}

// === Z Axis Control ===
void moveZDown() {
  stepperZ.moveTo(-500);
  stepperZ.runToPosition();
  Serial.println("Z lowered");
  delay(1000);
}

void moveZUp() {
  stepperZ.moveTo(0);
  stepperZ.runToPosition();
  Serial.println("Z raised");
  delay(1000);
}

// === Full Drink Routine ===
void runFullSequence() {
  homeAllMotors();

  // Y slides out
  stepperY.moveTo(800);
  stepperY.runToPosition();
  Serial.println("Y extended for cup");
  delay(1000);

  // Y slides in
  stepperY.moveTo(400);
  stepperY.runToPosition();
  Serial.println("Y pulled cup in");
  delay(1000);

  // Z lowers
  moveZDown();

  // Z raises
  moveZUp();

  // X moves to mixer
  stepperX.moveTo(-350);
  stepperX.runToPosition();
  Serial.println("X moved to mixer");
  delay(1000);

  // Z lowers to mix
  moveZDown();

  // Z raises again
  moveZUp();

  // Y returns to boba
  stepperY.moveTo(0);
  stepperY.runToPosition();
  Serial.println("Y returned for boba");
  delay(1000);

  // Boba spin
  Serial.println("Dispensing boba...");
  stepper3.moveTo(STEPS_PER_REV);
  stepper3.runToPosition();
  delay(1000);
  stepper3.moveTo(0);
  stepper3.runToPosition();
  delay(1000);

  // Y slides forward to deliver
  stepperY.moveTo(800);
  stepperY.runToPosition();
  Serial.println("Y fully extended to present cup");
  delay(1000);

  Serial.println("Resetting...");
  homeAllMotors();
}