// Yummy Boba + Menu System + NeoPixel + Stepper Homing + Manual Axis Control

#include <AccelStepper.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_NeoPixel.h>
#include <avr/wdt.h>  // For software reset

#define RESET_BUTTON_PIN 6
#define LIMIT_SWITCH_PIN A7

// LCD Display
#define I2C_ADDR 0x27
#define LCD_COLUMNS 20
#define LCD_LINES 4
LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_LINES);

// RFID Module
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);

// LED
#define LED_PIN 7

// Rotary Encoder
#define ENCODER_CLK 49
#define ENCODER_DT 51
#define ENCODER_SW 53

// NeoPixel
#define LED_STRIP_PIN 30
#define NUMPIXELS 200
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LED_STRIP_PIN, NEO_GRB + NEO_KHZ800);

// Servo
Servo servo_A0;

// Stepper Motors
AccelStepper stepper1(1, 13, 12);
AccelStepper stepper2(1, 11, 10);
AccelStepper stepper3(1, 9, 8);
AccelStepper stepper4(1, 7, 6);

// Homing Variables
int Homing1 = 1, Homing2 = 1, Homing3 = 1;
int HomingStepValue1 = 0, HomingStepValue2 = 0, HomingStepValue3 = 0;
bool isAllowed = false;

// Other Variables
volatile int menuIndex = 0;
int lastStateCLK;
bool buttonPressed = false;
int menuStartIndex = 0;
int redColor = 0, greenColor = 0, blueColor = 0;
int currentBrightness = 51;
int lastLimitValue = -1;

String menuItems[] = {
  "Run Diagnostics",
  "RFID Access",
  "Servo Trigger",
  "Manual Axis Ctrl",
  "LED Settings",
  "Reset System"
};
int menuSize = sizeof(menuItems) / sizeof(menuItems[0]);

void setup() {
  Serial.begin(9600);
  lcd.init(); lcd.backlight();
  lcd.setCursor(3, 1); lcd.print("YUMMY BOBA BOOT");
  delay(1500);

  for (int i = 1; i <= 4; i++) {
    AccelStepper* s = (i == 1) ? &stepper1 : (i == 2) ? &stepper2 : (i == 3) ? &stepper3 : &stepper4;
    s->setMaxSpeed(1000); s->setAcceleration(1000); s->setCurrentPosition(0);
  }

  pinMode(ENCODER_CLK, INPUT); pinMode(ENCODER_DT, INPUT); pinMode(ENCODER_SW, INPUT_PULLUP);
  lastStateCLK = digitalRead(ENCODER_CLK);

  SPI.begin(); mfrc522.PCD_Init();
  pinMode(LED_PIN, OUTPUT); digitalWrite(LED_PIN, HIGH);

  servo_A0.attach(A0, 500, 2500);
  servo_A0.write(0);
  delay(500);
  servo_A0.detach();

  pixels.begin(); pixels.setBrightness(currentBrightness);
  pixels.clear(); pixels.show();
  for (int i = 0; i < NUMPIXELS; i++) pixels.setPixelColor(i, pixels.Color(255, 255, 255));
  pixels.show();

  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);

  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Homing in progress");
  performHoming();
  lcd.setCursor(0, 1); lcd.print("Homing Complete");
  delay(1500);
  updateMenuDisplay();
}

void loop() {
  handleRotaryEncoder();
  if (digitalRead(ENCODER_SW) == LOW && !buttonPressed) {
    selectMenuItem();
    buttonPressed = true;
  } else if (digitalRead(ENCODER_SW) == HIGH) {
    buttonPressed = false;
  }
  if (digitalRead(RESET_BUTTON_PIN) == LOW) {
    lcd.clear(); lcd.print("System Reset...");
    delay(1000);
    wdt_enable(WDTO_15MS); while (1) {}
  }

  // Read and print analog value from limit switch
  int currentLimitValue = analogRead(LIMIT_SWITCH_PIN);
  if (currentLimitValue != lastLimitValue) {
    Serial.print("Analog Read A7 = ");
    Serial.println(currentLimitValue);
    lastLimitValue = currentLimitValue;
  }
}

void handleRotaryEncoder() {
  int currentStateCLK = digitalRead(ENCODER_CLK);
  if (currentStateCLK != lastStateCLK) {
    if (digitalRead(ENCODER_DT) != currentStateCLK) menuIndex++;
    else menuIndex--;
    if (menuIndex >= menuSize) menuIndex = 0;
    if (menuIndex < 0) menuIndex = menuSize - 1;
    if (menuIndex >= menuStartIndex + 3) menuStartIndex++;
    else if (menuIndex < menuStartIndex) menuStartIndex--;
    updateMenuDisplay();
  }
  lastStateCLK = currentStateCLK;
}

void updateMenuDisplay() {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Main Menu:");
  for (int i = 0; i < 3; i++) {
    int itemIndex = menuStartIndex + i;
    if (itemIndex < menuSize) {
      lcd.setCursor(1, i + 1);
      lcd.print((itemIndex == menuIndex) ? "> " : "  ");
      lcd.print(menuItems[itemIndex]);
    }
  }
}

void selectMenuItem() {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Chosen:");
  lcd.setCursor(2, 2); lcd.print(menuItems[menuIndex]);
  delay(1000);
  switch (menuIndex) {
    case 0: runDiagnostics(); break;
    case 1: checkRFID(); break;
    case 2: servoControl(); break;
    case 3: manualAxisControl(); break;
    case 4: ledControl(); break;
    case 5:
      lcd.clear(); lcd.print("System Resetting...");
      delay(1000);
      wdt_enable(WDTO_15MS); while (1) {}
      break;
  }
  updateMenuDisplay();
}

void runDiagnostics() {
  lcd.clear(); lcd.print("Running Tests...");
  Serial.println("Stepper Homing Starting");
  performHoming();
  Serial.println("Diagnostics Complete");
  lcd.setCursor(0, 1); lcd.print("Diagnostics OK");
  delay(1500);
}

void performHoming() {
  if (Homing1) {
    HomingStepValue1 = 1;
    while (true) {
      stepper1.moveTo(HomingStepValue1++); stepper1.run(); delay(5);
      if (digitalRead(3) == HIGH) {
        stepper1.stop();
        Serial.println("X Axis Homed (Limit Switch 3 Triggered)");
        break;
      }
    }
    stepper1.setCurrentPosition(0); Homing1 = 0;
  }
  if (Homing2) {
    HomingStepValue2 = 1;
    while (true) {
      stepper2.moveTo(HomingStepValue2++); stepper2.run(); delay(5);
      if (digitalRead(4) == HIGH) {
        stepper2.stop();
        Serial.println("Y Axis Homed (Limit Switch 4 Triggered)");
        break;
      }
    }
    stepper2.setCurrentPosition(0); Homing2 = 0;
  }
  if (Homing3) {
    HomingStepValue3 = 1;
    while (true) {
      stepper3.moveTo(HomingStepValue3++); stepper3.run(); delay(5);
      if (digitalRead(5) == HIGH) {
        stepper3.stop();
        Serial.println("Z Axis Homed (Limit Switch 5 Triggered)");
        break;
      }
    }
    stepper3.setCurrentPosition(0); Homing3 = 0;
  }
}

void manualAxisControl() {
  int axisIndex = 0;
  bool selecting = true;
  while (selecting) {
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("Manual Axis Ctrl:");
    lcd.setCursor(1, 1); lcd.print(axisIndex == 0 ? "> X Axis" : "  X Axis");
    lcd.setCursor(1, 2); lcd.print(axisIndex == 1 ? "> Y Axis" : "  Y Axis");
    lcd.setCursor(1, 3); lcd.print(axisIndex == 2 ? "> Z Axis" : "  Z Axis");

    int currentStateCLK = digitalRead(ENCODER_CLK);
    if (currentStateCLK != lastStateCLK) {
      axisIndex += (digitalRead(ENCODER_DT) != currentStateCLK) ? 1 : -1;
      if (axisIndex > 2) axisIndex = 0;
      if (axisIndex < 0) axisIndex = 2;
    }
    lastStateCLK = currentStateCLK;
    if (digitalRead(ENCODER_SW) == LOW) {
      delay(300);
      controlAxis(axisIndex);
      selecting = false;
    }
  }
}

void controlAxis(int axis) {
  AccelStepper* selected = (axis == 0) ? &stepper1 : (axis == 1) ? &stepper2 : &stepper3;
  long position = 0;
  lcd.clear(); lcd.print("Move Axis:");
  while (true) {
    int currentStateCLK = digitalRead(ENCODER_CLK);
    if (currentStateCLK != lastStateCLK) {
      position += (digitalRead(ENCODER_DT) != currentStateCLK) ? 10 : -10;
      selected->moveTo(position);
    }
    selected->run();
    lastStateCLK = currentStateCLK;
    if (digitalRead(ENCODER_SW) == LOW) break;
  }
  lcd.clear(); lcd.print("Axis Movement Done");
  delay(1000);
}

void servoControl() {
  lcd.clear(); lcd.print("Servo Triggering");
  servo_A0.attach(A0, 500, 2500);
  servo_A0.write(90); delay(1000);
  servo_A0.write(0); delay(500);
  servo_A0.detach();
  lcd.setCursor(0, 1); lcd.print("Reset Complete");
  delay(1000);
}

void checkRFID() {
  lcd.clear(); lcd.print("Scan Card...");
  while (true) {
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) continue;
    String content = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) content.concat(String(mfrc522.uid.uidByte[i], HEX));
    content.toUpperCase(); lcd.clear();
    if (content == "87E5BEA7" || content == "EAFA9C82") {
      lcd.print("Access Granted"); isAllowed = true; flashColor(0, 255, 0);
    } else {
      lcd.print("Access Denied"); isAllowed = false; flashColor(255, 0, 0);
    }
    delay(2000);
    for (int i = 0; i < NUMPIXELS; i++) pixels.setPixelColor(i, pixels.Color(255, 255, 255));
    pixels.show(); break;
  }
}

void flashColor(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < NUMPIXELS; j++) pixels.setPixelColor(j, pixels.Color(r, g, b));
    pixels.show(); delay(300);
    for (int j = 0; j < NUMPIXELS; j++) pixels.setPixelColor(j, 0);
    pixels.show(); delay(300);
  }
}

void ledControl() {
  int choice = 0;
  while (true) {
    lcd.clear(); lcd.setCursor(0, 0); lcd.print("LED Control:");
    lcd.setCursor(1, 1); lcd.print(choice == 0 ? "> Brightness" : "  Brightness");
    lcd.setCursor(1, 2); lcd.print(choice == 1 ? "> Color" : "  Color");
    int currentStateCLK = digitalRead(ENCODER_CLK);
    if (currentStateCLK != lastStateCLK) {
      choice += (digitalRead(ENCODER_DT) != currentStateCLK) ? 1 : -1;
      if (choice > 1) choice = 0; if (choice < 0) choice = 1;
    }
    lastStateCLK = currentStateCLK;
    if (digitalRead(ENCODER_SW) == LOW) {
      delay(300);
      if (choice == 0) adjustBrightness();
      else changeColor();
      break;
    }
  }
}

void adjustBrightness() {
  int brightnessValue = currentBrightness;
  while (true) {
    int currentStateCLK = digitalRead(ENCODER_CLK);
    if (currentStateCLK != lastStateCLK) {
      brightnessValue += (digitalRead(ENCODER_DT) != currentStateCLK) ? 5 : -5;
      brightnessValue = constrain(brightnessValue, 0, 255);
      currentBrightness = brightnessValue;
      pixels.setBrightness(currentBrightness);
      pixels.show();
      lcd.clear(); lcd.print("Brightness: "); lcd.print(currentBrightness);
    }
    lastStateCLK = currentStateCLK;
    if (digitalRead(ENCODER_SW) == LOW) break;
  }
  delay(300);
}

void changeColor() {
  redColor = random(0, 255);
  greenColor = random(0, 255);
  blueColor = random(0, 255);
  for (int i = 0; i < NUMPIXELS; i++) pixels.setPixelColor(i, pixels.Color(redColor, greenColor, blueColor));
  pixels.setBrightness(currentBrightness);
  pixels.show();
  lcd.clear(); lcd.print("New Color Set"); delay(1500);
}