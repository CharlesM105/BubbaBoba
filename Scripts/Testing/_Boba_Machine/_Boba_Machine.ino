#include <AccelStepper.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_NeoPixel.h>
#include <avr/wdt.h>

// ==== Constants and Pins ====
#define I2C_ADDR 0x27
#define LCD_COLUMNS 20
#define LCD_LINES 4
LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_LINES);

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);

#define LED_STRIP_PIN 30
#define NUMPIXELS 200
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LED_STRIP_PIN, NEO_GRB + NEO_KHZ800);

#define ENCODER_CLK 49
#define ENCODER_DT 51
#define ENCODER_SW 53
#define RESET_BUTTON_PIN 6
#define LIMIT_SWITCH_PIN A7

Servo servo_A0;

// Stepper Motors
AccelStepper stepper1(1, 13, 12);
AccelStepper stepper2(1, 11, 10);
AccelStepper stepper3(1, 9, 8);
AccelStepper stepper4(1, 7, 6);

// ==== Global Variables ====
int milkRatio = 60, flavorRatio = 20, bobaRatio = 20;
int redColor = 0, greenColor = 0, blueColor = 0;
int currentBrightness = 51;
bool isAllowed = false;
int lastStateCLK, lastLimitValue = -1;
bool buttonPressed = false;
bool inDrinkMenu = true;
int menuIndex = 0, menuStartIndex = 0;

String drinkMenu[] = { "Brown Sugar Tea", "Strawberry Milk", "Taro Milk Tea", "Back to Control" };
int drinkMenuSize = sizeof(drinkMenu) / sizeof(drinkMenu[0]);

String controlMenu[] = { "Run Diagnostics", "RFID Access", "Servo Trigger", "Manual Axis Ctrl", "LED Settings", "Reset System" };
int controlMenuSize = sizeof(controlMenu) / sizeof(controlMenu[0]);

void setup() {
  Serial.begin(9600);
  lcd.init(); lcd.backlight();
  lcd.setCursor(3, 1); lcd.print("YUMMY BOBA BOT");
  delay(1500);

  for (int i = 1; i <= 4; i++) {
    AccelStepper* s = (i == 1) ? &stepper1 : (i == 2) ? &stepper2 : (i == 3) ? &stepper3 : &stepper4;
    s->setMaxSpeed(1000); s->setAcceleration(1000); s->setCurrentPosition(0);
  }

  pinMode(ENCODER_CLK, INPUT);
  pinMode(ENCODER_DT, INPUT);
  pinMode(ENCODER_SW, INPUT_PULLUP);
  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
  lastStateCLK = digitalRead(ENCODER_CLK);

  SPI.begin();
  mfrc522.PCD_Init();

  servo_A0.attach(A0, 500, 2500); servo_A0.write(0); delay(500); servo_A0.detach();

  pixels.begin();
  pixels.setBrightness(currentBrightness);
  pixels.clear(); pixels.show();

  performHoming();
  showDrinkMenu();
}

void loop() {
  handleRotaryEncoder();

  if (digitalRead(ENCODER_SW) == LOW && !buttonPressed) {
    buttonPressed = true;
    selectMenuItem();
  } else if (digitalRead(ENCODER_SW) == HIGH) {
    buttonPressed = false;
  }

  if (digitalRead(RESET_BUTTON_PIN) == LOW) {
    lcd.clear(); lcd.print("System Reset...");
    delay(1000);
    wdt_enable(WDTO_15MS); while (1) {}
  }

  int currentLimitValue = analogRead(LIMIT_SWITCH_PIN);
  if (currentLimitValue != lastLimitValue) {
    Serial.print("Analog Read A7 = "); Serial.println(currentLimitValue);
    lastLimitValue = currentLimitValue;
  }
}

void performHoming() {
  lcd.clear(); lcd.print("Homing in progress");

  // HOMING STEPPER 1 (X AXIS)
  stepper1.setMaxSpeed(5000.0);
  stepper1.setAcceleration(1000.0);
  Serial.println("Stepper 1 is Homing");
  while (digitalRead(4) != HIGH) { // Using Pin 4 for Stepper 1 Limit Switch
    stepper1.moveTo(stepper1.currentPosition() + 1);
    stepper1.run();
    delay(5);
  }
  stepper1.setCurrentPosition(0);
  Serial.println("Homing 1 Completed");

  // HOMING STEPPER 4 (CUP SLIDE AXIS)
  stepper4.setMaxSpeed(5000.0);
  stepper4.setAcceleration(1000.0);
  Serial.println("Stepper 2 is Homing");
  while (digitalRead(2) != HIGH) { // Using Pin 2 for Stepper 4 Limit Switch
    stepper4.moveTo(stepper4.currentPosition() - 1);
    stepper4.run();
    delay(5);
  }
  stepper4.setCurrentPosition(0);
  Serial.println("Homing 2 Completed");

  // HOMING STEPPER 3 (Z AXIS)
  stepper3.setMaxSpeed(5000.0);
  stepper3.setAcceleration(1000.0);
  Serial.println("Stepper 3 is Homing");
  while (digitalRead(3) != HIGH) { // Using Pin 3 for Stepper 3 Limit Switch
    stepper3.moveTo(stepper3.currentPosition() - 1);
    stepper3.run();
    delay(5);
  }
  stepper3.setCurrentPosition(0);
  Serial.println("Homing 3 Completed");

  // HOMING COMPLETE
  lcd.setCursor(0, 1); lcd.print("Homing Complete");
  Serial.println("Full Homing Sequence Completed");
  delay(1500);
}

void handleRotaryEncoder() {
  int currentStateCLK = digitalRead(ENCODER_CLK);
  if (currentStateCLK != lastStateCLK) {
    if (digitalRead(ENCODER_DT) != currentStateCLK) menuIndex++;
    else menuIndex--;

    int currentSize = inDrinkMenu ? drinkMenuSize : controlMenuSize;
    if (menuIndex >= currentSize) menuIndex = 0;
    if (menuIndex < 0) menuIndex = currentSize - 1;

    if (menuIndex >= menuStartIndex + 3) menuStartIndex++;
    else if (menuIndex < menuStartIndex) menuStartIndex--;

    updateMenuDisplay();
  }
  lastStateCLK = currentStateCLK;
}

void updateMenuDisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(inDrinkMenu ? "Drinks Menu:" : "Control Menu:");
  for (int i = 0; i < 3; i++) {
    int itemIndex = menuStartIndex + i;
    if (itemIndex < (inDrinkMenu ? drinkMenuSize : controlMenuSize)) {
      lcd.setCursor(1, i + 1);
      lcd.print((itemIndex == menuIndex) ? "> " : "  ");
      lcd.print(inDrinkMenu ? drinkMenu[itemIndex] : controlMenu[itemIndex]);
    }
  }
}

void showDrinkMenu() {
  inDrinkMenu = true;
  menuIndex = 0;
  menuStartIndex = 0;
  updateMenuDisplay();
}

void showControlMenu() {
  inDrinkMenu = false;
  menuIndex = 0;
  menuStartIndex = 0;
  updateMenuDisplay();
}
void selectMenuItem() {
  lcd.clear(); lcd.setCursor(0, 0); lcd.print("Selected:");
  lcd.setCursor(2, 2); lcd.print(inDrinkMenu ? drinkMenu[menuIndex] : controlMenu[menuIndex]);
  delay(1000);

  if (inDrinkMenu) {
    if (drinkMenu[menuIndex] == "Back to Control") {
      showControlMenu();
    } else {
      askBobaPreference();
    }
  } else {
    String selected = controlMenu[menuIndex];
    if (selected == "Reset System") {
      performHoming();
      showDrinkMenu();
    } else if (selected == "Manual Axis Ctrl") {
      manualAxisControl();
    } else if (selected == "LED Settings") {
      ledControl();
    } else if (selected == "RFID Access") {
      checkRFID();
    } else if (selected == "Servo Trigger") {
      servoControl();
    } else if (selected == "Run Diagnostics") {
      runDiagnostics();
    }
  }

  updateMenuDisplay();
}
void askBobaPreference() {
  bool addBoba = true, addSyrup = false;
  bool confirmed = false;

  while (!confirmed) {
    lcd.clear(); lcd.setCursor(0, 0); lcd.print("Add Boba?");
    lcd.setCursor(2, 2); lcd.print(addBoba ? "Yes" : "No");
    int clk = digitalRead(ENCODER_CLK);
    if (clk != lastStateCLK) addBoba = !addBoba;
    lastStateCLK = clk;
    if (digitalRead(ENCODER_SW) == LOW) { confirmed = true; delay(300); }
  }

  if (addBoba) adjustBobaPercentage();
  else bobaRatio = 0;

  confirmed = false;
  while (!confirmed) {
    lcd.clear(); lcd.setCursor(0, 0); lcd.print("Add Syrup Ring?");
    lcd.setCursor(2, 2); lcd.print(addSyrup ? "Yes" : "No");
    int clk = digitalRead(ENCODER_CLK);
    if (clk != lastStateCLK) addSyrup = !addSyrup;
    lastStateCLK = clk;
    if (digitalRead(ENCODER_SW) == LOW) { confirmed = true; delay(300); }
  }

  lcd.clear(); lcd.print("Creating drink...");
  delay(1500);
  startDrinkMakingProcess(addSyrup);
}

void adjustBobaPercentage() {
  int percentage = bobaRatio;
  while (true) {
    lcd.clear(); lcd.setCursor(0, 0); lcd.print("Set Boba Amount");
    lcd.setCursor(2, 2); lcd.print(percentage); lcd.print("%");

    int clk = digitalRead(ENCODER_CLK);
    if (clk != lastStateCLK) {
      percentage += (digitalRead(ENCODER_DT) != clk) ? 5 : -5;
      percentage = constrain(percentage, 0, 100);
    }
    lastStateCLK = clk;
    if (digitalRead(ENCODER_SW) == LOW) {
      bobaRatio = percentage;
      delay(300);
      break;
    }
  }
}
void startDrinkMakingProcess(bool addSyrup) {
  slideCupOut();
  waitForCupGrab();
  returnCupToCenter();
  if (addSyrup) applySyrupRing();
  dispenseMilk();
  dispenseFlavor();
  mixDrink();
  if (bobaRatio > 0) { moveToBoba(); dispenseBoba(); }
  liftCup();
  presentCupToUser();
  waitToStartNewOrder();
}

void slideCupOut() {
  lcd.clear(); lcd.print("Sliding cup out...");
  stepper4.moveTo(1000); // Slide out
  while (stepper4.distanceToGo() != 0) stepper4.run();
  delay(500);
}

void returnCupToCenter() {
  lcd.clear(); lcd.print("Returning cup...");
  stepper4.moveTo(0); // Return to center
  while (stepper4.distanceToGo() != 0) stepper4.run();
  delay(500);
}

void applySyrupRing() {
  lcd.clear(); lcd.print("Applying syrup...");
  stepper1.moveTo(500); // Move to syrup zone
  while (stepper1.distanceToGo() != 0) stepper1.run();
  delay(500);
  stepper1.moveTo(0); // Return
  while (stepper1.distanceToGo() != 0) stepper1.run();
  delay(500);
}

void dispenseMilk() {
  lcd.clear(); lcd.print("Dispensing Milk");
  lcd.setCursor(0, 2); lcd.print(milkRatio); lcd.print("%");
  stepper2.moveTo(milkRatio * 10); // Simulate pump
  while (stepper2.distanceToGo() != 0) stepper2.run();
  delay(500);
  stepper2.moveTo(0);
  while (stepper2.distanceToGo() != 0) stepper2.run();
  delay(300);
}

void dispenseFlavor() {
  lcd.clear(); lcd.print("Adding Flavor");
  lcd.setCursor(0, 2); lcd.print(flavorRatio); lcd.print("%");
  stepper2.moveTo(flavorRatio * 10);
  while (stepper2.distanceToGo() != 0) stepper2.run();
  delay(500);
  stepper2.moveTo(0);
  while (stepper2.distanceToGo() != 0) stepper2.run();
  delay(300);
}

void mixDrink() {
  lcd.clear(); lcd.print("Mixing drink...");
  stepper3.moveTo(500); // Lower + spin
  while (stepper3.distanceToGo() != 0) stepper3.run();
  delay(500);
  stepper3.moveTo(0); // Return
  while (stepper3.distanceToGo() != 0) stepper3.run();
  delay(500);
}

void moveToBoba() {
  lcd.clear(); lcd.print("Positioning Boba");
  stepper1.moveTo(1000); // Move under boba
  while (stepper1.distanceToGo() != 0) stepper1.run();
  delay(500);
}

void dispenseBoba() {
  lcd.clear(); lcd.print("Dispensing Boba");
  stepper2.moveTo(bobaRatio * 10);
  while (stepper2.distanceToGo() != 0) stepper2.run();
  delay(500);
  stepper2.moveTo(0);
  while (stepper2.distanceToGo() != 0) stepper2.run();
  delay(500);
}

void liftCup() {
  lcd.clear(); lcd.print("Raising mixer...");
  stepper3.moveTo(300); // Raise
  while (stepper3.distanceToGo() != 0) stepper3.run();
  delay(500);
  stepper3.moveTo(0);
  while (stepper3.distanceToGo() != 0) stepper3.run();
  delay(500);
}

void presentCupToUser() {
  lcd.clear(); lcd.print("Slide out cup");
  lcd.setCursor(0, 1); lcd.print("Grab & Press Btn");
  stepper4.moveTo(1000); // Slide cup out again
  while (stepper4.distanceToGo() != 0) stepper4.run();
  while (digitalRead(ENCODER_SW) == HIGH) delay(10);
  lcd.clear(); lcd.print("Cup retrieved!");
  delay(1000);
  stepper4.moveTo(0); // Pull tray back in
  while (stepper4.distanceToGo() != 0) stepper4.run();
  delay(500);
}

void waitForCupGrab() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Press knob when");
  lcd.setCursor(0, 1);
  lcd.print("you've grabbed cup");

  // Wait for encoder button press
  while (digitalRead(ENCODER_SW) == HIGH) {
    delay(10);
  }

  // Optional confirmation
  lcd.clear();
  lcd.print("Cup grabbed!");
  delay(1000);
}

void waitToStartNewOrder() {
  lcd.clear(); lcd.print("Start another?");
  lcd.setCursor(0, 2); lcd.print("Press button...");
  while (digitalRead(ENCODER_SW) == HIGH) delay(10);
  lcd.clear(); lcd.print("Resetting system...");
  delay(1000);
  performHoming();
  showDrinkMenu();
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

void ledControl() {
  int choice = 0;
  while (true) {
    lcd.clear(); lcd.setCursor(0, 0); lcd.print("LED Control:");
    lcd.setCursor(1, 1); lcd.print(choice == 0 ? "> Brightness" : "  Brightness");
    lcd.setCursor(1, 2); lcd.print(choice == 1 ? "> Color" : "  Color");

    int clk = digitalRead(ENCODER_CLK);
    if (clk != lastStateCLK) {
      choice += (digitalRead(ENCODER_DT) != clk) ? 1 : -1;
      if (choice > 1) choice = 0;
      if (choice < 0) choice = 1;
    }
    lastStateCLK = clk;

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
    int clk = digitalRead(ENCODER_CLK);
    if (clk != lastStateCLK) {
      brightnessValue += (digitalRead(ENCODER_DT) != clk) ? 5 : -5;
      brightnessValue = constrain(brightnessValue, 0, 255);
      currentBrightness = brightnessValue;
      pixels.setBrightness(currentBrightness);
      pixels.show();
      lcd.clear(); lcd.print("Brightness: "); lcd.print(currentBrightness);
    }
    lastStateCLK = clk;
    if (digitalRead(ENCODER_SW) == LOW) break;
  }
  delay(300);
}

void changeColor() {
  redColor = random(0, 255);
  greenColor = random(0, 255);
  blueColor = random(0, 255);
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(redColor, greenColor, blueColor));
  }
  pixels.setBrightness(currentBrightness);
  pixels.show();
  lcd.clear(); lcd.print("New Color Set");
  delay(1500);
}

void checkRFID() {
  lcd.clear(); lcd.print("Scan Card...");
  while (true) {
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) continue;
    String content = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    content.toUpperCase();
    lcd.clear();

    if (content == "87E5BEA7" || content == "EAFA9C82") {
      lcd.print("Access Granted");
      isAllowed = true;
      flashColor(0, 255, 0);
    } else {
      lcd.print("Access Denied");
      isAllowed = false;
      flashColor(255, 0, 0);
    }

    delay(2000);
    for (int i = 0; i < NUMPIXELS; i++) pixels.setPixelColor(i, pixels.Color(255, 255, 255));
    pixels.show();
    break;
  }
}

void flashColor(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < NUMPIXELS; j++) {
      pixels.setPixelColor(j, pixels.Color(r, g, b));
    }
    pixels.show();
    delay(300);
    for (int j = 0; j < NUMPIXELS; j++) {
      pixels.setPixelColor(j, 0);
    }
    pixels.show();
    delay(300);
  }
}

void servoControl() {
  lcd.clear(); lcd.print("Servo Triggering");
  servo_A0.attach(A0, 500, 2500);
  servo_A0.write(90);
  delay(1000);
  servo_A0.write(0);
  delay(500);
  servo_A0.detach();
  lcd.setCursor(0, 1); lcd.print("Reset Complete");
  delay(1000);
}

void runDiagnostics() {
  lcd.clear(); lcd.print("Running Tests...");
  Serial.println("Stepper Homing Starting");
  performHoming();
  Serial.println("Diagnostics Complete");
  lcd.setCursor(0, 1); lcd.print("Diagnostics OK");
  delay(1500);
}