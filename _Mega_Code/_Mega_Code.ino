// === Arduino Mega (Menu UI) ===
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_NeoPixel.h>
#include <avr/wdt.h>

#define I2C_ADDR 0x27
#define LCD_COLUMNS 20
#define LCD_LINES 4
LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_LINES);

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);

#define LED_STRIP_PIN 30
#define NUMPIXELS 200
Adafruit_NeoPixel pixels(NUMPIXELS, LED_STRIP_PIN, NEO_GRB + NEO_KHZ800);

#define ENCODER_CLK 49
#define ENCODER_DT 51
#define ENCODER_SW 53
#define RESET_BUTTON_PIN 6

Servo servo_A2;

int milkRatio = 60, flavorRatio = 20, bobaRatio = 20;
int redColor = 0, greenColor = 0, blueColor = 0;
int currentBrightness = 51;

int lastStateCLK;
bool inDrinkMenu = true;
int menuIndex = 0, menuStartIndex = 0;
unsigned long lastButtonPress = 0;
unsigned long lastEncoderMove = 0;
const unsigned long encoderDebounce = 100;

String drinkMenu[] = {"Brown Sugar Tea", "Strawberry Milk", "Taro Milk Tea", "Back to Control"};
int drinkMenuSize = sizeof(drinkMenu) / sizeof(drinkMenu[0]);

String controlMenu[] = {"Run Diagnostics", "RFID Access", "Servo Trigger", "Manual Axis Ctrl", "LED Settings", "Reset System"};
int controlMenuSize = sizeof(controlMenu) / sizeof(controlMenu[0]);

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(3, 1);
  lcd.print("YUMMY BOBA BOT");
  delay(1500);

  pinMode(ENCODER_CLK, INPUT);
  pinMode(ENCODER_DT, INPUT);
  pinMode(ENCODER_SW, INPUT_PULLUP);
  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);

  lastStateCLK = digitalRead(ENCODER_CLK);

  SPI.begin();
  mfrc522.PCD_Init();

  servo_A2.attach(A2, 500, 2500);
  servo_A2.write(0);
  delay(500);
  servo_A2.detach();

  pixels.begin();
  pixels.setBrightness(currentBrightness);
  pixels.clear();
  pixels.show();

  performHoming();
  showDrinkMenu();
}

void loop() {
  handleRotaryEncoder();

  if (digitalRead(ENCODER_SW) == LOW && (millis() - lastButtonPress > 250)) {
    lastButtonPress = millis();
    selectMenuItem();
  }

  if (digitalRead(RESET_BUTTON_PIN) == LOW) {
    lcd.clear();
    lcd.print("System Reset...");
    delay(1000);
    wdt_enable(WDTO_15MS);
    while (1) {}
  }

  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    if (command == "dispense") {
      Serial1.println("DISPENSE");
      Serial.println("Sent DISPENSE to UNO");
    }
  }
}

void performHoming() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Homing in progress");
  for (int i = 10; i > 0; i--) {
    lcd.setCursor(0, 1);
    lcd.print("Wait ");
    lcd.print(i);
    lcd.print(" sec...   ");
    delay(1000);
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Homing complete");
  delay(1000);
}

void handleRotaryEncoder() {
  if (millis() - lastEncoderMove < encoderDebounce) return;

  int currentStateCLK = digitalRead(ENCODER_CLK);
  int currentStateDT = digitalRead(ENCODER_DT);

  if (currentStateCLK != lastStateCLK && currentStateCLK == HIGH) {
    lastEncoderMove = millis();
    int menuSize = inDrinkMenu ? drinkMenuSize : controlMenuSize;

    if (currentStateDT == LOW) {
      menuIndex++;
    } else {
      menuIndex--;
    }

    if (menuIndex < 0) menuIndex = menuSize - 1;
    if (menuIndex >= menuSize) menuIndex = 0;

    showDrinkMenu();
  }

  lastStateCLK = currentStateCLK;
}

void showDrinkMenu() {
  static int lastMenuIndex = -1;
  static int lastMenuStartIndex = -1;
  static bool lastInDrinkMenu = !inDrinkMenu;

  if (menuIndex == lastMenuIndex &&
      menuStartIndex == lastMenuStartIndex &&
      inDrinkMenu == lastInDrinkMenu) return;

  lastMenuIndex = menuIndex;
  lastMenuStartIndex = menuStartIndex;
  lastInDrinkMenu = inDrinkMenu;

  String* menu = inDrinkMenu ? drinkMenu : controlMenu;
  int menuSize = inDrinkMenu ? drinkMenuSize : controlMenuSize;

  if (menuIndex < menuStartIndex) {
    menuStartIndex = menuIndex;
  } else if (menuIndex >= menuStartIndex + LCD_LINES) {
    menuStartIndex = menuIndex - LCD_LINES + 1;
  }

  for (int i = 0; i < LCD_LINES; i++) {
    int itemIndex = menuStartIndex + i;
    lcd.setCursor(0, i);
    if (itemIndex < menuSize) {
      String line = (itemIndex == menuIndex ? "> " : "  ") + menu[itemIndex];
      lcd.print(line);
      int padding = LCD_COLUMNS - line.length();
      while (padding-- > 0) lcd.print(" ");
    } else {
      lcd.print("                    ");
    }
  }
}

void selectMenuItem() {
  lcd.clear();
  String* menu = inDrinkMenu ? drinkMenu : controlMenu;
  String selection = menu[menuIndex];

  lcd.setCursor(0, 0);
  lcd.print("Selected:");
  lcd.setCursor(0, 1);
  lcd.print(selection);
  delay(1000);

  if (inDrinkMenu) {
    if (selection == "Back to Control") {
      inDrinkMenu = false;
      menuIndex = 0;
      menuStartIndex = 0;
    } else {
      lcd.clear();
      lcd.print("Making: ");
      lcd.print(selection);
      Serial1.println("DISPENSE");
      delay(2000);
    }
  } else {
    if (selection == "Reset System") {
      lcd.clear();
      lcd.print("Resetting...");
      delay(1000);
      wdt_enable(WDTO_15MS);
      while (1) {}
    } else if (selection == "RFID Access") {
      lcd.clear();
      lcd.print("Scan your card...");
      delay(2000);
    } else if (selection == "Servo Trigger") {
      servo_A2.attach(A2, 500, 2500);
      servo_A2.write(90);
      delay(1000);
      servo_A2.write(0);
      delay(500);
      servo_A2.detach();
    } else if (selection == "LED Settings") {
      lcd.clear();
      lcd.print("LED Settings TBD");
      delay(1000);
    } else if (selection == "Run Diagnostics") {
      lcd.clear();
      lcd.print("Running tests...");
      Serial1.println("DISPENSE");
      delay(1000);
    } else if (selection == "Manual Axis Ctrl") {
      lcd.clear();
      lcd.print("Manual Ctrl TBD");
      delay(1000);
    }

    if (selection != "Reset System") {
      inDrinkMenu = true;
      menuIndex = 0;
      menuStartIndex = 0;
    }
  }

  menuIndex = -1; // force LCD redraw
}
