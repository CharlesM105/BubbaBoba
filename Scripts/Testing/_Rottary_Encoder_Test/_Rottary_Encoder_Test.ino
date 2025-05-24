#define ENCODER_SW 21  // Try pin 10 instead of 21 or 20

void setup() {
  Serial.begin(9600);
  pinMode(ENCODER_SW, INPUT_PULLUP);
  Serial.println("Pushbutton Test");
}

void loop() {
  int state = digitalRead(ENCODER_SW);
  Serial.print("Button State: ");
  Serial.println(state);
  delay(200);
}