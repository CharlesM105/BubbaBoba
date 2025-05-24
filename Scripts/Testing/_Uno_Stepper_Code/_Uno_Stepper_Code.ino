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

// === Relays (Updated Pins) ===
#define RELAY_MIXER A2
#define RELAY_BOBA_SHAKER A3

Servo coverServo;

void setup() {
  Serial.begin(9600);

  // Slower and smoother speeds
  stepperX.setMaxSpeed(2000);
  stepperX.setAcceleration(2000);
  stepperY.setMaxSpeed(2000);
  stepperY.setAcceleration(800);
  stepperZ.setMaxSpeed(2000);
  stepperZ.setAcceleration(2000);
  stepper3.setMaxSpeed(800);
  stepper3.setAcceleration(300);

  stepperX.setCurrentPosition(0);
  stepperY.setCurrentPosition(0);
  stepperZ.setCurrentPosition(0);
  stepper3.setCurrentPosition(0);

  pinMode(LIMIT_X, INPUT_PULLUP);
  pinMode(LIMIT_Y, INPUT_PULLUP);
  pinMode(LIMIT_Z, INPUT_PULLUP);

  pinMode(RELAY_MIXER, OUTPUT);
  pinMode(RELAY_BOBA_SHAKER, OUTPUT);
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
  delay(3000);
  Serial.println("Fast homing X...");
  stepperX.setSpeed(400);
  while (digitalRead(LIMIT_X) != HIGH) {
    stepperX.runSpeed();
  }
  stepperX.setCurrentPosition(0);
  Serial.println("X homed");

  Serial.println("Fast homing Y...");
  stepperY.setSpeed(-400);
  while (digitalRead(LIMIT_Y) != HIGH) {
    stepperY.runSpeed();
  }
  stepperY.setCurrentPosition(0);
  Serial.println("Y homed");

  Serial.println("Fast homing Z...");
  stepperZ.setSpeed(-400);
  while (digitalRead(LIMIT_Z) != HIGH) {
    stepperZ.runSpeed();
  }
  stepperZ.setCurrentPosition(0);
  Serial.println("Z homed");

  delay(100);
}

// === Full Drink Routine ===
void runFullSequence() {
  homeAllMotors();

  stepperZ.setMaxSpeed(1000);  
  stepperZ.setAcceleration(1000);   

  // Y slides out
  stepperY.moveTo(1100);
  stepperY.runToPosition();
  Serial.println("Y extended for cup");

  // Wait for photoresistor signal (light)
  waitForSignal();

  // Y slides in
  stepperY.moveTo(520);
  stepperY.runToPosition();
  Serial.println("Y pulled cup in");
  
  // X moves to tubes
  stepperX.moveTo(-200);
  stepperX.runToPosition();
  Serial.println("X moved to tubes");
  
  // Z lowers
  stepperZ.moveTo(2000);
  stepperZ.runToPosition();
  Serial.println("Z lowered");

  // Move servo to 45 degrees
  coverServo.write(45);
  delay(5000);  // Dispensing flavor
  coverServo.write(0);  // Back to 0

  // Z raises
  stepperZ.setSpeed(-400);
  while (digitalRead(LIMIT_Z) != HIGH) {
    stepperZ.runSpeed();
  }

  // X moves to mixer
  stepperX.moveTo(-670);
  stepperX.runToPosition();
  Serial.println("X moved to mixer");
  
  // Z lowers
  stepperZ.moveTo(2000);
  stepperZ.runToPosition();
  Serial.println("Z lowered");

  // Turn on mixer
  digitalWrite(RELAY_MIXER, HIGH);
  delay(4000);  // mixing time
  digitalWrite(RELAY_MIXER, LOW);

  // Z raises
  stepperZ.setSpeed(-400);
  while (digitalRead(LIMIT_Z) != HIGH) {
    stepperZ.runSpeed();
  }

  // X returns home
  stepperX.setSpeed(400);
  while (digitalRead(LIMIT_X) != HIGH) {
    stepperX.runSpeed();
  }

  // Y returns to boba position
  stepperY.setSpeed(-400);
  while (digitalRead(LIMIT_Y) != HIGH) {
    stepperY.runSpeed();
  }

  // Dispense boba
  Serial.println("Dispensing boba...");
  digitalWrite(RELAY_BOBA_SHAKER, HIGH);  // Start shaking
  stepper3.moveTo(200);
  stepper3.runToPosition();
  stepper3.setCurrentPosition(0);
  digitalWrite(RELAY_BOBA_SHAKER, LOW);   // Stop shaking
  delay(1000);

  // Present cup
  stepperY.moveTo(1100);
  stepperY.runToPosition();
  Serial.println("Y fully extended to present cup");

  delay(10000); // Wait for customer to pick up cup

  Serial.println("Resetting...");
  homeAllMotors();
}

// === Wait for Signal from Photoresistor on A0 ===
void waitForSignal() {
  int lightThreshold = 20; // Adjust based on your lighting and photoresistor setup

  Serial.println("Waiting for light signal from photoresistor...");

  while (true) {
    int lightLevel = analogRead(A0);
    if (lightLevel > lightThreshold) {
      Serial.print("Light detected! Value: ");
      Serial.println(lightLevel);
      break;
    }
    delay(50);
  }

  Serial.println("Light signal confirmed, continuing...");
}
