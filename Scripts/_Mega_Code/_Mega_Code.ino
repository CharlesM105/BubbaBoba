#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <Adafruit_NeoPixel.h>

#define I2C_ADDR 0x27
#define LCD_COLUMNS 20
#define LCD_LINES 4
LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_LINES);

#define LED_STRIP_PIN 10
#define NUMPIXELS 40
Adafruit_NeoPixel pixels(NUMPIXELS, LED_STRIP_PIN, NEO_GRB + NEO_KHZ800);

#define ENCODER_CLK 18
#define ENCODER_DT 19
#define ENCODER_SW 4

#define RELAY_MILK 5
#define RELAY_FLAVOR_1 6
#define RELAY_FLAVOR_2 7
#define RELAY_FLAVOR_3 8
#define UNO_CONNECTION A0
#define MIXER_PIN 53

const unsigned long MILK_DURATION = 5000;
const unsigned long FLAVOR_DURATION = 6000;
const unsigned long MIX_DURATION = 3000;

bool inDrinkMenu = true;
int menuIndex = 0, menuStartIndex = 0;

String drinkMenu[] = {"Classic Tea", "Strawberry Milk", "Taro Milk Tea", "Back to Control"};
int drinkMenuSize = sizeof(drinkMenu) / sizeof(drinkMenu[0]);

String controlMenu[] = {"Run Diagnostics", "Manual Axis Ctrl", "LED Settings", "Cleaning"};
int controlMenuSize = sizeof(controlMenu) / sizeof(controlMenu[0]);

bool makingDrink = false;
bool promptingBoba = false;
int bobaChoice = 0;
unsigned long drinkStartTime = 0;
int drinkStep = 0;
String currentDrink = "";
bool messageDisplayed = false;

int lastClk = HIGH;
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;

void setup() {
  Serial.begin(9600);

  pinMode(RELAY_FLAVOR_1, OUTPUT);
  pinMode(RELAY_FLAVOR_2, OUTPUT);
  pinMode(RELAY_FLAVOR_3, OUTPUT);
  pinMode(RELAY_MILK, OUTPUT);
  pinMode(UNO_CONNECTION, OUTPUT);
  pinMode(MIXER_PIN, OUTPUT);

  digitalWrite(RELAY_FLAVOR_1, HIGH);
  digitalWrite(RELAY_FLAVOR_2, HIGH);
  digitalWrite(RELAY_FLAVOR_3, HIGH);
  digitalWrite(RELAY_MILK, HIGH);
  digitalWrite(UNO_CONNECTION, LOW);
  digitalWrite(MIXER_PIN, LOW);

  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  pinMode(ENCODER_SW, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(3, 1);
  lcd.print("BUBBA BOBA 3000");
  delay(1500);

  pixels.begin();
  pixels.setBrightness(51);
  pixels.clear();
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 100));
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

  if (menuIndex < menuStartIndex) {
    menuStartIndex = menuIndex;
  } else if (menuIndex >= menuStartIndex + 4) {
    menuStartIndex = menuIndex - 3;
  }

  for (int i = 0; i < 4; i++) {
    int itemIdx = menuStartIndex + i;
    if (itemIdx >= size) break;

    lcd.setCursor(0, i);
    lcd.print((itemIdx == menuIndex) ? "> " : "  ");
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
      promptingBoba = true;
      bobaChoice = 0;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Add boba?");
      lcd.setCursor(0, 1);
      lcd.print("> Yes");
    }
  } else {
    if (selection == "Run Diagnostics") {
      lcd.clear();
      lcd.print("Running tests...");
      delay(1000);
    } else if (selection == "Manual Axis Ctrl") {
      lcd.clear();
      lcd.print("Manual Ctrl TBD");
      delay(1000);
    } else if (selection == "LED Settings") {
      lcd.clear();
      lcd.print("LED Settings TBD");
      delay(1000);
    } else if (selection == "Cleaning") {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Cleaning started...");
      digitalWrite(RELAY_MILK, LOW);
      digitalWrite(RELAY_FLAVOR_1, LOW);
      digitalWrite(RELAY_FLAVOR_2, LOW);
      digitalWrite(RELAY_FLAVOR_3, LOW);
      delay(30000); // 30 seconds
      digitalWrite(RELAY_MILK, HIGH);
      digitalWrite(RELAY_FLAVOR_1, HIGH);
      digitalWrite(RELAY_FLAVOR_2, HIGH);
      digitalWrite(RELAY_FLAVOR_3, HIGH);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Cleaning complete");
      delay(1500);
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

  if (currentClk != lastClk && currentClk == LOW) {
    if (promptingBoba) {
      bobaChoice = 1 - bobaChoice;
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print("> " + String(bobaChoice == 0 ? "Yes" : "No"));
    } else {
      int size = inDrinkMenu ? drinkMenuSize : controlMenuSize;
      menuIndex = (currentDt != currentClk)
        ? (menuIndex + 1) % size
        : (menuIndex - 1 + size) % size;
      showMenu();
    }
  }
  lastClk = currentClk;

  if (buttonState == LOW && lastButtonState == HIGH && (millis() - lastDebounceTime > 250)) {
    if (promptingBoba) {
      promptingBoba = false;
      makingDrink = true;
      messageDisplayed = false;
      drinkStep = 0;
      drinkStartTime = millis();
    } else {
      selectMenuItem();
    }
    lastDebounceTime = millis();
  }
  lastButtonState = buttonState;
}

// your handleDrinkMaking() remains unchanged from the previous version

void handleDrinkMaking() {
  if (!makingDrink) return;

  unsigned long now = millis();

  switch (drinkStep) {
    case 0:
      if (!messageDisplayed) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Starting drink...");
        lcd.setCursor(0, 1);
        lcd.print(currentDrink);
        messageDisplayed = true;
        drinkStartTime = now;
      }
      if (now - drinkStartTime >= 1500) {
        drinkStep++;
        messageDisplayed = false;
      }
      break;

    case 1:
      if (!messageDisplayed) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Moving cup...");
        digitalWrite(UNO_CONNECTION, LOW);
        messageDisplayed = true;
        drinkStartTime = now;
      }
      if (now - drinkStartTime >= 2000) {
        digitalWrite(UNO_CONNECTION, HIGH);
        drinkStep++;
        messageDisplayed = false;
      }
      break;

    case 2:
      if (!messageDisplayed) {
        delay(3000);
        digitalWrite(UNO_CONNECTION, LOW);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Dispensing milk...");
        digitalWrite(RELAY_MILK, LOW);  // Turn ON (for active LOW relay)
        messageDisplayed = true;
        drinkStartTime = now;
      }
      if (now - drinkStartTime >= MILK_DURATION) {
        digitalWrite(RELAY_MILK, HIGH); // Turn OFF
        drinkStep++;
        messageDisplayed = false;
      }
      break;

    case 3:
      if (!messageDisplayed) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Adding flavor...");
        if (currentDrink == "Classic Tea") digitalWrite(RELAY_FLAVOR_1, LOW);
        else if (currentDrink == "Strawberry Milk") digitalWrite(RELAY_FLAVOR_2, LOW);
        else if (currentDrink == "Taro Milk Tea") digitalWrite(RELAY_FLAVOR_3, LOW);
        messageDisplayed = true;
        drinkStartTime = now;
      }
      if (now - drinkStartTime >= FLAVOR_DURATION) {
        digitalWrite(RELAY_FLAVOR_1, HIGH);
        digitalWrite(RELAY_FLAVOR_2, HIGH);
        digitalWrite(RELAY_FLAVOR_3, HIGH);
        drinkStep++;
        messageDisplayed = false;
      }
      break;

    case 4:
      if (!messageDisplayed) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Mixing...");
        digitalWrite(MIXER_PIN, HIGH);
        messageDisplayed = true;
        drinkStartTime = now;
      }
      if (now - drinkStartTime >= MIX_DURATION) {
        digitalWrite(MIXER_PIN, LOW);
        drinkStep++;
        messageDisplayed = false;
      }
      break;

    case 5:
      if (!messageDisplayed) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Enjoy your drink!");
        messageDisplayed = true;
        drinkStartTime = now;
      }
      if (now - drinkStartTime >= 2000) {
        makingDrink = false;
        showMenu();
      }
      break;
  }
}