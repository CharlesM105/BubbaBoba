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
#define ENCODER_CLK 18   // Moved off relay conflict pins
#define ENCODER_DT 19
#define ENCODER_SW 20

// === Relays === (Moved to avoid pin conflict)
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
unsigned long lastButtonPress = 0;
const unsigned long encoderDebounce = 100;
unsigned long startupIgnoreTime = 0;

String drinkMenu[] = {"Classic Tea", "Strawberry Milk", "Taro Milk Tea", "Back to Control"};
int drinkMenuSize = sizeof(drinkMenu) / sizeof(drinkMenu[0]);

String controlMenu[] = {"Run Diagnostics", "Manual Axis Ctrl", "LED Settings"};
int controlMenuSize = sizeof(controlMenu) / sizeof(controlMenu[0]);

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial.println("Encoder Test Start");

  startupIgnoreTime = millis();  // Track startup time to block initial button

  spinGoldLEDs(3000);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(3, 1);
  lcd.print("YUMMY BOBA BOT");
  delay(1500);

  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  pinMode(ENCODER_SW, INPUT_PULLUP);

  pinMode(RELAY_FLAVOR_1, OUTPUT);
  pinMode(RELAY_FLAVOR_2, OUTPUT);
  pinMode(RELAY_FLAVOR_3, OUTPUT);
  pinMode(RELAY_MILK, OUTPUT);

  pinMode(A0, OUTPUT);
  digitalWrite(A0, LOW);

  digitalWrite(RELAY_FLAVOR_1, LOW);
  digitalWrite(RELAY_FLAVOR_2, LOW);
  digitalWrite(RELAY_FLAVOR_3, LOW);
  digitalWrite(RELAY_MILK, LOW);

  pixels.begin();
  pixels.setBrightness(currentBrightness);
  pixels.clear();
  pixels.show();

  performHoming();
  showDrinkMenu();
}

void loop() {
  handleRotaryEncoder();

  // === Button debounce with startup lock ===
  static bool lastButtonState = HIGH;
  bool buttonState = digitalRead(ENCODER_SW);

  if (millis() - startupIgnoreTime >= 3000) {
    if (buttonState != lastButtonState) {
      delay(10);  // debounce
      buttonState = digitalRead(ENCODER_SW);
      if (buttonState == LOW && lastButtonState == HIGH) {
        lastButtonPress = millis();
        selectMenuItem();
      }
    }
  }

  lastButtonState = buttonState;

  spinGoldLEDs(3000);
}

void performHoming() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Homing in progress");
  for (int i = 1; i > 0; i--) {
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
  static int lastClkState = HIGH;
  static unsigned long lastTurnTime = 0;

  int clkState = digitalRead(ENCODER_CLK);
  int dtState = digitalRead(ENCODER_DT);

  if (clkState != lastClkState && clkState == LOW) {
    if (millis() - lastTurnTime > encoderDebounce) {
      int menuSize = inDrinkMenu ? drinkMenuSize : controlMenuSize;

      if (dtState == HIGH) {
        menuIndex++;
        Serial.println("Rotated Clockwise");
      } else {
        menuIndex--;
        Serial.println("Rotated CounterClockwise");
      }

      if (menuIndex < 0) menuIndex = menuSize - 1;
      if (menuIndex >= menuSize) menuIndex = 0;

      showDrinkMenu();
      lastTurnTime = millis();
    }
  }

  lastClkState = clkState;
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
      makeDrink(selection);
    }
  } else {
    if (selection == "LED Settings") {
      lcd.clear();
      lcd.print("LED Settings TBD");
      delay(1000);
    } else if (selection == "Run Diagnostics") {
      lcd.clear();
      lcd.print("Running tests...");
    } else if (selection == "Manual Axis Ctrl") {
      lcd.clear();
      lcd.print("Manual Ctrl TBD");
      delay(1000);
    }

    inDrinkMenu = true;
    menuIndex = 0;
    menuStartIndex = 0;
  }

  menuIndex = -1;
}

void spinGoldLEDs(int durationMillis) {
  unsigned long startTime = millis();
  int pixelCount = pixels.numPixels();
  int position = 0;

  while (millis() - startTime < durationMillis) {
    pixels.clear();
    for (int i = 0; i < 5; i++) {
      int index = (position + i) % pixelCount;
      pixels.setPixelColor(index, pixels.Color(255, 215, 0));
    }
    pixels.show();
    delay(60);
    position = (position + 1) % pixelCount;
  }

  pixels.clear();
  pixels.show();
}

void makeDrink(String drinkName) {
  lcd.clear();
  lcd.print("Making Drink:");
  lcd.setCursor(0, 1);
  lcd.print(drinkName);
  delay(1000);

  if (drinkName == "Classic Tea") {
    digitalWrite(A0, HIGH);
    delay(500);
    digitalWrite(A0, LOW);
    delay(5000);
    digitalWrite(RELAY_FLAVOR_1, HIGH);
    delay(2000);
    digitalWrite(RELAY_FLAVOR_1, LOW);
    digitalWrite(RELAY_MILK, HIGH);
    delay(2000);
    digitalWrite(RELAY_MILK, LOW);
    delay(5000);

  } else if (drinkName == "Strawberry Milk") {
    digitalWrite(A0, HIGH);
    delay(500);
    digitalWrite(A0, LOW);
    delay(5000);
    digitalWrite(RELAY_FLAVOR_2, HIGH);
    delay(2000);
    digitalWrite(RELAY_FLAVOR_2, LOW);
    digitalWrite(RELAY_MILK, HIGH);
    delay(2000);
    digitalWrite(RELAY_MILK, LOW);
    delay(5000);

  } else if (drinkName == "Taro Milk Tea") {
    digitalWrite(A0, HIGH);
    delay(500);
    digitalWrite(A0, LOW);
    delay(5000);
    digitalWrite(RELAY_FLAVOR_3, HIGH);
    delay(2000);
    digitalWrite(RELAY_FLAVOR_3, LOW);
    digitalWrite(RELAY_MILK, HIGH);
    delay(2000);
    digitalWrite(RELAY_MILK, LOW);
    delay(5000);
  }

  lcd.clear();
  lcd.print("Enjoy your drink!");
  delay(2000);
}