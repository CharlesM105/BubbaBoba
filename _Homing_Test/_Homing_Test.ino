// X Axis
#define X_STEP_PIN 2
#define X_DIR_PIN 5
#define X_LIMIT_PIN 9

// Y Axis
#define Y_STEP_PIN 3
#define Y_DIR_PIN 6
#define Y_LIMIT_PIN 10

// Z Axis
#define Z_STEP_PIN 4
#define Z_DIR_PIN 7
#define Z_LIMIT_PIN 11

#define STEP_DELAY 500  // microseconds (controls speed)
#define BACKOFF_STEPS 100

void setup() {
  // Setup stepper pins
  pinMode(X_STEP_PIN, OUTPUT);
  pinMode(X_DIR_PIN, OUTPUT);
  pinMode(Y_STEP_PIN, OUTPUT);
  pinMode(Y_DIR_PIN, OUTPUT);
  pinMode(Z_STEP_PIN, OUTPUT);
  pinMode(Z_DIR_PIN, OUTPUT);

  // Setup limit switch pins (normally open, pulled HIGH)
  pinMode(X_LIMIT_PIN, INPUT_PULLUP);
  pinMode(Y_LIMIT_PIN, INPUT_PULLUP);
  pinMode(Z_LIMIT_PIN, INPUT_PULLUP);

  Serial.begin(9600);
  delay(1000);  // Give some time on power-up

  homeAxis("X", X_STEP_PIN, X_DIR_PIN, X_LIMIT_PIN, false);
  homeAxis("Y", Y_STEP_PIN, Y_DIR_PIN, Y_LIMIT_PIN, false);
  homeAxis("Z", Z_STEP_PIN, Z_DIR_PIN, Z_LIMIT_PIN, true); // Z often homes upward
}

void loop() {
  // Nothing here, everything runs once in setup()
}

// General homing function
void homeAxis(const char* axisName, int stepPin, int dirPin, int limitPin, bool reverseDir) {
  Serial.print("Homing ");
  Serial.print(axisName);
  Serial.println(" axis...");

  // Move toward switch
  digitalWrite(dirPin, reverseDir ? LOW : HIGH);
  while (digitalRead(limitPin) == HIGH) {
    singleStep(stepPin, STEP_DELAY);
  }

  // Back off slightly
  digitalWrite(dirPin, reverseDir ? HIGH : LOW);
  for (int i = 0; i < BACKOFF_STEPS; i++) {
    singleStep(stepPin, STEP_DELAY);
  }

  // Slow approach
  digitalWrite(dirPin, reverseDir ? LOW : HIGH);
  while (digitalRead(limitPin) == HIGH) {
    singleStep(stepPin, STEP_DELAY * 2);  // slower speed
  }

  Serial.print(axisName);
  Serial.println(" axis homed.");
}

// Single step pulse
void singleStep(int stepPin, int delayMicros) {
  digitalWrite(stepPin, HIGH);
  delayMicroseconds(delayMicros);
  digitalWrite(stepPin, LOW);
  delayMicroseconds(delayMicros);
}