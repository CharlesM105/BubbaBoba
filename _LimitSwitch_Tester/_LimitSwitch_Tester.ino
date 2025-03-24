// Limit switch pins
#define X_LIMIT_PIN 9
#define Y_LIMIT_PIN 10
#define Z_LIMIT_PIN 11

void setup() {
  pinMode(X_LIMIT_PIN, INPUT);
  pinMode(Y_LIMIT_PIN, INPUT);
  pinMode(Z_LIMIT_PIN, INPUT);

  Serial.begin(115200);
  delay(500);
  Serial.println("Limit Switch Test Started");
}

void loop() {
  bool xState = digitalRead(X_LIMIT_PIN);
  bool yState = digitalRead(Y_LIMIT_PIN);
  bool zState = digitalRead(Z_LIMIT_PIN);

  Serial.print("X: ");
  Serial.print(xState == LOW ? "Pressed" : "Open");
  Serial.print(" | Y: ");
  Serial.print(yState == LOW ? "Pressed" : "Open");
  Serial.print(" | Z: ");
  Serial.println(zState == LOW ? "Pressed" : "Open");

  delay(500);  // Update twice per second
}