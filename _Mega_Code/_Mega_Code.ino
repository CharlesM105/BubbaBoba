#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <Adafruit_NeoPixel.h>

// === LCD ===
#define I2C_ADDR 0x27
#define LCD_COLUMNS 20
#define LCD_LINES 4
LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_LINES);

// === LEDs ===
#define LED_STRIP_PIN 10
#define NUMPIXELS 40
Adafruit_NeoPixel pixels(NUMPIXELS, LED_STRIP_PIN, NEO_GRB + NEO_KHZ800);

// === Rotary Encoder ===
#define ENCODER_CLK 18
#define ENCODER_DT 19
#define ENCODER_SW 4

// === Relays ===
#define RELAY_MILK 5
#define RELAY_FLAVOR_1 6
#define RELAY_FLAVOR_2 7
#define RELAY_FLAVOR_3 8

#define UNO_CONNECTION A0

int milkRatio = 60, flavorRatio = 20, bobaRatio = 20;
int redColor = 0, greenColor = 0, blueColor = 0;
int currentBrightness = 51;

bool inDrinkMenu = true;
int menuIndex = 0, menuStartIndex = 0;
unsigned long startupIgnoreTime = 0;

String drinkMenu[] = {"Classic Tea", "Strawberry Milk", "Taro Milk Tea", "Back to Control"};
int drinkMenuSize = sizeof(drinkMenu) / sizeof(drinkMenu[0]);

String controlMenu[] = {"Run Diagnostics", "Manual Axis Ctrl", "LED Settings"};
int controlMenuSize = sizeof(controlMenu) / sizeof(controlMenu[0]);

// === DRINK-MAKING STATE ===
bool makingDrink = false;
unsigned long drinkStartTime = 0;
int drinkStep = 0;
String currentDrink = "";

// === Encoder State ===
int lastClk = HIGH;
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  startupIgnoreTime = millis();

  pinMode(RELAY_FLAVOR_1, OUTPUT);
  pinMode(RELAY_FLAVOR_2, OUTPUT);
  pinMode(RELAY_FLAVOR_3, OUTPUT);
  pinMode(RELAY_MILK, OUTPUT);
  pinMode(A0, OUTPUT);
  digitalWrite(RELAY_FLAVOR_1, LOW);
  digitalWrite(RELAY_FLAVOR_2, LOW);
  digitalWrite(RELAY_FLAVOR_3, LOW);
  digitalWrite(RELAY_MILK, LOW);
  digitalWrite(A0, LOW);

  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  pinMode(ENCODER_SW, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(3, 1);
  lcd.print("BUBBA BOBA 3000");
  delay(1500);

  pixels.begin();
  pixels.setBrightness(currentBrightness);
  pixels.clear();
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 100));  // Blue
  }
  pixels.show();

  performHoming();
  showMenu();
}

void loop() {
  handleEncoder();
  handleDrinkMaking();
}

void performHoming() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Homing in progress");
  lcd.setCursor(0, 1);
  lcd.print("Wait 1 sec...");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Homing complete");
  delay(1000);
}

void showMenu() {
  lcd.clear();
  String* menu = inDrinkMenu ? drinkMenu : controlMenu;
  int size = inDrinkMenu ? drinkMenuSize : controlMenuSize;

  // Adjust visible window
  if (menuIndex < menuStartIndex) {
    menuStartIndex = menuIndex;
  } else if (menuIndex >= menuStartIndex + 4) {
    menuStartIndex = menuIndex - 3;
  }

  // Draw up to 4 lines
  for (int i = 0; i < 4; i++) {
    int itemIdx = menuStartIndex + i;
    if (itemIdx >= size) break;

    lcd.setCursor(0, i);
    if (itemIdx == menuIndex) {
      lcd.print("> ");
    } else {
      lcd.print("  ");
    }
    lcd.print(menu[itemIdx]);
  }
}

void selectMenuItem() {
  if (makingDrink) return;

  String* menu = inDrinkMenu ? drinkMenu : controlMenu;
  String selection = menu[menuIndex];

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Selected:");
  lcd.setCursor(0, 1);
  lcd.print(selection);
  delay(1000);

  if (inDrinkMenu) {
    if (selection == "Back to Control") {
      inDrinkMenu = false;
      menuIndex = 0;
    } else {
      currentDrink = selection;
      makingDrink = true;
      drinkStartTime = millis();
      drinkStep = 0;
    }
  } else {
    if (selection == "LED Settings") {
      lcd.clear();
      lcd.print("LED Settings TBD");
      delay(1000);
    } else if (selection == "Run Diagnostics") {
      lcd.clear();
      lcd.print("Running tests...");
      delay(1000);
    } else if (selection == "Manual Axis Ctrl") {
      lcd.clear();
      lcd.print("Manual Ctrl TBD");
      delay(1000);
    }

    inDrinkMenu = true;
    menuIndex = 0;
  }

  showMenu();
}

void handleEncoder() {
  int currentClk = digitalRead(ENCODER_CLK);
  int currentDt = digitalRead(ENCODER_DT);
  bool buttonState = digitalRead(ENCODER_SW);

  // Rotation
  if (currentClk != lastClk && currentClk == LOW) {
    int size = inDrinkMenu ? drinkMenuSize : controlMenuSize;

    if (currentDt != currentClk) {
      menuIndex++;
      if (menuIndex >= size) menuIndex = 0;
    } else {
      menuIndex--;
      if (menuIndex < 0) menuIndex = size - 1;
    }
    showMenu();
  }
  lastClk = currentClk;

  // Button Press
  if (buttonState == LOW && lastButtonState == HIGH && (millis() - lastDebounceTime > 250)) {
    selectMenuItem();
    lastDebounceTime = millis();
  }
  lastButtonState = buttonState;
}

void handleDrinkMaking() {
  if (!makingDrink) return;

  unsigned long now = millis();

  switch (drinkStep) {
    case 0:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Starting drink...");
      lcd.setCursor(0, 1);
      lcd.print(currentDrink);
      // Opposite: keep all pumps ON initially
      digitalWrite(RELAY_MILK, HIGH);
      digitalWrite(RELAY_FLAVOR_1, HIGH);
      digitalWrite(RELAY_FLAVOR_2, HIGH);
      digitalWrite(RELAY_FLAVOR_3, HIGH);
      drinkStep++;
      drinkStartTime = now;
      break;

    case 1:
      if (now - drinkStartTime >= 1000) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Moving cup...");
        digitalWrite(UNO_CONNECTION, LOW); // Opposite: pull LOW to start
        drinkStep++;
        drinkStartTime = now;
      }
      break;

    case 2:
      if (now - drinkStartTime >= 1000) {
        digitalWrite(UNO_CONNECTION, HIGH); // Opposite: pull HIGH to stop
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Dispensing milk...");
        digitalWrite(RELAY_MILK, LOW); // Opposite: LOW means ON now
        drinkStep++;
        drinkStartTime = now;
      }
      break;

    case 3:
      if (now - drinkStartTime >= 2000) {
        digitalWrite(RELAY_MILK, HIGH); // Opposite: HIGH means OFF now
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Adding flavor...");

        if (currentDrink == "Classic Tea") {
          digitalWrite(RELAY_FLAVOR_1, LOW);
        } else if (currentDrink == "Strawberry Milk") {
          digitalWrite(RELAY_FLAVOR_2, LOW);
        } else if (currentDrink == "Taro Milk Tea") {
          digitalWrite(RELAY_FLAVOR_3, LOW);
        }

        drinkStep++;
        drinkStartTime = now;
      }
      break;

    case 4:
      if (now - drinkStartTime >= 2000) {
        // Opposite: turn all flavor relays back to HIGH (off)
        digitalWrite(RELAY_FLAVOR_1, HIGH);
        digitalWrite(RELAY_FLAVOR_2, HIGH);
        digitalWrite(RELAY_FLAVOR_3, HIGH);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Finishing up...");
        drinkStep++;
        drinkStartTime = now;
      }
      break;

    case 5:
      if (now - drinkStartTime >= 1000) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Enjoy your drink!");
        drinkStep++;
        drinkStartTime = now;
      }
      break;

    case 6:
      if (now - drinkStartTime >= 2000) {
        makingDrink = false;
        showMenu();
      }
      break;
  }
}