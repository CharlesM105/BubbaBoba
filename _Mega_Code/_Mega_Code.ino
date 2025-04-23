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

Servo servo_A2;

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
  Serial1.begin(9600); // Serial1 for communication with Uno
  lcd.init(); lcd.backlight();
  lcd.setCursor(3, 1); lcd.print("YUMMY BOBA BOT");
  delay(1500);

  pinMode(ENCODER_CLK, INPUT);
  pinMode(ENCODER_DT, INPUT);
  pinMode(ENCODER_SW, INPUT_PULLUP);
  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
  lastStateCLK = digitalRead(ENCODER_CLK);

  pinMode(A0, INPUT);
  pinMode(A1, OUTPUT);

  SPI.begin();
  mfrc522.PCD_Init();

  servo_A2.attach(A2, 500, 2500);
  servo_A2.write(0);
  delay(500);
  servo_A2.detach();

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
  lcd.clear();
  lcd.print("Homing in progress");

  // Wait until the Uno sends "HOMED\n"
  String buffer = "";
  while (true) {
    while (Serial1.available()) {
      char c = Serial1.read();
      if (c == '\n') {
        if (buffer == "HOMED") {
          lcd.clear();
          lcd.print("Homing is complete");
          delay(1000);
          return;
        } else {
          buffer = ""; // Reset if not "HOMED"
        }
      } else {
        buffer += c;
      }
    }
  }
}