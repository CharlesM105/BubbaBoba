#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <Adafruit_NeoPixel.h>

// === LCD ===
#define I2C_ADDR 0x27
#define LCD_COLUMNS 20
#define LCD_LINES 4
LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_LINES);

// === LEDs ===
#define LED_STRIP_PIN 30
#define NUMPIXELS 40
Adafruit_NeoPixel pixels(NUMPIXELS, LED_STRIP_PIN, NEO_GRB + NEO_KHZ800);

// === Rotary Encoder ===
#define ENCODER_CLK 18
#define ENCODER_DT 19
#define ENCODER_SW 21  // WORKING PIN

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

// === BUTTON STATE TRACKING ===
bool lastButtonState = HIGH;
unsigned long buttonDebounceTime = 0;
const unsigned long debounceDelay = 50;

// === DRINK-MAKING STATE ===
bool makingDrink = false;
unsigned long drinkStartTime = 0;
int drinkStep = 0;
String currentDrink = "";

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  startupIgnoreTime = millis();

  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  pinMode(ENCODER_SW, INPUT_PULLUP);

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

  lcd.init();
  lcd.backlight();
  lcd.setCursor(3, 1);
  lcd.print("YUMMY BOBA BOT");
  delay(1500);

  pixels.begin();
  pixels.setBrightness(currentBrightness);
  pixels.clear();
  pixels.show();

  performHoming();
  showDrinkMenu();
}

void loop() {
  handleRotaryEncoder();
  handleButton();
  handleDrinkMaking();
}

void handleButton() {
  bool buttonState = digitalRead(ENCODER_SW);

  if (millis() - startupIgnoreTime < 3000) return;

  if (buttonState != lastButtonState) {
    buttonDebounceTime = millis();
  }

  if ((millis() - buttonDebounceTime) > debounceDelay) {
    if (lastButtonState == HIGH && buttonState == LOW) {
      Serial.println("Button Press Detected");
      selectMenuItem();
    }
  }

  lastButtonState = buttonState;
}

void handleRotaryEncoder() {
  static int lastClkState = HIGH;
  static unsigned long lastTurnTime = 0;
  const unsigned long encoderDebounce = 10;

  int clkState = digitalRead(ENCODER_CLK);
  int dtState = digitalRead(ENCODER_DT);

  if (clkState != lastClkState) {
    if (clkState == LOW && (millis() - lastTurnTime > encoderDebounce)) {
      int menuSize = inDrinkMenu ? drinkMenuSize : controlMenuSize;

      if (dtState != clkState) {
        menuIndex++;
      } else {
        menuIndex--;
      }

      if (menuIndex < 0) menuIndex = menuSize - 1;
      if (menuIndex >= menuSize) menuIndex = 0;

      showDrinkMenu();
      lastTurnTime = millis();
    }
    lastClkState = clkState;
  }
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
  if (makingDrink) return;

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
    menuStartIndex = 0;
  }
}

void handleDrinkMaking() {
  if (!makingDrink) return;

  unsigned long now = millis();

  switch (drinkStep) {
    case 0:
      lcd.clear();
      lcd.print("Making Drink:");
      lcd.setCursor(0, 1);
      lcd.print(currentDrink);
      drinkStep++;
      drinkStartTime = now;
      break;

    case 1:
      if (now - drinkStartTime >= 1000) {
        digitalWrite(A0, HIGH);
        drinkStep++;
        drinkStartTime = now;
      }
      break;

    case 2:
      if (now - drinkStartTime >= 500) {
        digitalWrite(A0, LOW);
        drinkStep++;
        drinkStartTime = now;
      }
      break;

    case 3:
      if (now - drinkStartTime >= 5000) {
        if (currentDrink == "Classic Tea") {
          digitalWrite(RELAY_FLAVOR_1, HIGH);
        } else if (currentDrink == "Strawberry Milk") {
          digitalWrite(RELAY_FLAVOR_2, HIGH);
        } else if (currentDrink == "Taro Milk Tea") {
          digitalWrite(RELAY_FLAVOR_3, HIGH);
        }
        drinkStep++;
        drinkStartTime = now;
      }
      break;

    case 4:
      if (now - drinkStartTime >= 2000) {
        digitalWrite(RELAY_FLAVOR_1, LOW);
        digitalWrite(RELAY_FLAVOR_2, LOW);
        digitalWrite(RELAY_FLAVOR_3, LOW);
        digitalWrite(RELAY_MILK, HIGH);
        drinkStep++;
        drinkStartTime = now;
      }
      break;

    case 5:
      if (now - drinkStartTime >= 2000) {
        digitalWrite(RELAY_MILK, LOW);
        drinkStep++;
        drinkStartTime = now;
      }
      break;

    case 6:
      if (now - drinkStartTime >= 5000) {
        lcd.clear();
        lcd.print("Enjoy your drink!");
        drinkStep++;
        drinkStartTime = now;
      }
      break;

    case 7:
      if (now - drinkStartTime >= 2000) {
        makingDrink = false;
        showDrinkMenu();
      }
      break;
  }
}