// Relay pin definitions
const int relay1 = 0;
const int relay2 = 1;
const int relay3 = 2;
const int relay4 = 3;

void setup() {
  // Set relay pins as OUTPUT
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);

  // Turn all relays OFF (assuming LOW = OFF)
  digitalWrite(relay1, LOW);
  digitalWrite(relay2, LOW);
  digitalWrite(relay3, LOW);
  digitalWrite(relay4, LOW);
}

void loop() {
  // Turn each relay ON one by one
  digitalWrite(relay1, HIGH);
  delay(1000);
  digitalWrite(relay2, HIGH);
  delay(1000);
  digitalWrite(relay3, HIGH);
  delay(1000);
  digitalWrite(relay4, HIGH);
  delay(2000);

  // Turn all relays OFF
  digitalWrite(relay1, LOW);
  digitalWrite(relay2, LOW);
  digitalWrite(relay3, LOW);
  digitalWrite(relay4, LOW);
  delay(2000);
}