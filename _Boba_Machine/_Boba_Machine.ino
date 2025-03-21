// Yummy Boba + Menu System Integration + NeoPixel LED Control with Scrolling Menu

// Libraries
#include <AccelStepper.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_NeoPixel.h>

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

// Rotary Encoder Pins
#define ENCODER_CLK 49  
#define ENCODER_DT 51  
#define ENCODER_SW 53  

// Rotary Encoder Variables
volatile int menuIndex = 0;  
int lastStateCLK;  
bool buttonPressed = false;  
int menuStartIndex = 0;  // Start index for scrolling

// Stepper Motors
AccelStepper stepper1(1, 13, 12);
AccelStepper stepper2(1, 11, 10);
AccelStepper stepper3(1, 9, 8);
AccelStepper stepper4(1, 7, 6);

// Servo
Servo servo_A0;

// NeoPixel LED Strip
#define LED_STRIP_PIN 30
#define NUMPIXELS 200
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LED_STRIP_PIN, NEO_GRB + NEO_KHZ800);

int delayval = 100;
int redColor = 0;
int greenColor = 0;
int blueColor = 0;
int currentBrightness = 51;  // 20% of 255

// Homing Variables
bool isAllowed = false;

// Menu Items
String menuItems[] = {
  "Start Process",
  "Check RFID",
  "Servo Control",
  "Stepper Control",
  "LED Control",
  "Exit"
};
int menuSize = sizeof(menuItems) / sizeof(menuItems[0]);

void setup() {
  Serial.begin(9600);
  Serial.println("Power On");

  // LCD Setup
  lcd.init();
  lcd.backlight();
  lcd.setCursor(5, 1);
  lcd.print("-POWER ON-");
  delay(1000);

  // Stepper Motor Setup
  stepper1.setMaxSpeed(1000);
  stepper2.setMaxSpeed(1000);
  stepper3.setMaxSpeed(1000);
  stepper4.setMaxSpeed(1000);

  stepper1.setAcceleration(1000);
  stepper2.setAcceleration(1000);
  stepper3.setAcceleration(1000);
  stepper4.setAcceleration(1000);

  // Rotary Encoder Setup
  pinMode(ENCODER_CLK, INPUT);
  pinMode(ENCODER_DT, INPUT);
  pinMode(ENCODER_SW, INPUT_PULLUP);
  lastStateCLK = digitalRead(ENCODER_CLK);

  // RFID Setup
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Approximate your card to the reader...");

  // LED Setup
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  // Servo Setup
  servo_A0.attach(A0, 500, 2500);
  servo_A0.write(0);
  Serial.println("Servo Reset");

  // NeoPixel LED Setup
  pixels.begin();
  pixels.setBrightness(currentBrightness);  // Set initial brightness to 20%
  pixels.clear();
  pixels.show();

  // Turn on default LED color (white)
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 255, 255));
  }
  pixels.show();

  // Display Initial Menu
  updateMenuDisplay();
}

void loop() {
  // Handle Rotary Encoder for Menu Navigation
  handleRotaryEncoder();

  // Handle Menu Selection
  if (digitalRead(ENCODER_SW) == LOW) {
    if (!buttonPressed) {
      selectMenuItem();
      buttonPressed = true;
    }
  } else {
    buttonPressed = false;
  }
}

void handleRotaryEncoder() {
  int currentStateCLK = digitalRead(ENCODER_CLK);
  if (currentStateCLK != lastStateCLK) {
    if (digitalRead(ENCODER_DT) != currentStateCLK) {
      menuIndex++;
    } else {
      menuIndex--;
    }

    // Wrap-around logic
    if (menuIndex >= menuSize) menuIndex = 0;
    if (menuIndex < 0) menuIndex = menuSize - 1;

    // Scrolling logic
    if (menuIndex >= menuStartIndex + 3) {
      menuStartIndex++;
    } else if (menuIndex < menuStartIndex) {
      menuStartIndex--;
    }

    updateMenuDisplay();
  }
  lastStateCLK = currentStateCLK;
}

void updateMenuDisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Select an Option:");

  for (int i = 0; i < 3; i++) {
    int itemIndex = menuStartIndex + i;
    if (itemIndex < menuSize) {
      lcd.setCursor(1, i + 1);
      if (itemIndex == menuIndex) {
        lcd.print("> ");
      } else {
        lcd.print("  ");
      }
      lcd.print(menuItems[itemIndex]);
    }
  }
}

void selectMenuItem() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("You selected:");
  lcd.setCursor(2, 2);
  lcd.print(menuItems[menuIndex]);

  delay(1500);  
  if (menuIndex == 0) {
    startProcess();
  } else if (menuIndex == 1) {
    checkRFID();
  } else if (menuIndex == 2) {
    servoControl();
  } else if (menuIndex == 3) {
    stepperControl();
  } else if (menuIndex == 4) {
    ledControl();
  }
  updateMenuDisplay();
}

void startProcess() {
  lcd.clear();
  lcd.print("Starting Process...");
  Serial.println("Starting Process...");
  lcd.clear();
  lcd.print("Homing Complete!");
  Serial.println("Homing Complete!");
  delay(2000);
}

void checkRFID() {
  lcd.clear();
  lcd.print("Scan RFID Now...");
  Serial.println("Waiting for RFID...");

  while (true) {
    if (!mfrc522.PICC_IsNewCardPresent()) continue;
    if (!mfrc522.PICC_ReadCardSerial()) continue;

    String content = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }

    content.toUpperCase();
    lcd.clear();

    if (content == "87E5BEA7" || content == "EAFA9C82") {
      lcd.print("Access Granted!");
      Serial.println("Access Granted");
      isAllowed = true;
      flashColor(0, 255, 0); // green
    } else {
      lcd.print("Access Denied!");
      Serial.println("Access Denied");
      isAllowed = false;
      flashColor(255, 0, 0); // red
    }

    delay(2000);

    // Return to white
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(255, 255, 255));
    }
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

    // Off
    for (int j = 0; j < NUMPIXELS; j++) {
      pixels.setPixelColor(j, 0);
    }
    pixels.show();
    delay(300);
  }
}

void servoControl() {
  lcd.clear();
  lcd.print("Moving Servo...");
  Serial.println("Moving Servo...");
  servo_A0.write(90);
  delay(1000);
  servo_A0.write(0);
  Serial.println("Servo Reset");
  lcd.print("Servo Reset!");
  delay(1000);
}

void stepperControl() {
  lcd.clear();
  lcd.print("Stepper Test...");
  Serial.println("Stepper Test...");

  stepper1.moveTo(500);
  while (stepper1.distanceToGo() != 0) stepper1.run();
  delay(500);
  stepper1.moveTo(0);
  while (stepper1.distanceToGo() != 0) stepper1.run();

  stepper2.moveTo(500);
  while (stepper2.distanceToGo() != 0) stepper2.run();
  delay(500);
  stepper2.moveTo(0);
  while (stepper2.distanceToGo() != 0) stepper2.run();

  stepper3.moveTo(500);
  while (stepper3.distanceToGo() != 0) stepper3.run();
  delay(500);
  stepper3.moveTo(0);
  while (stepper3.distanceToGo() != 0) stepper3.run();

  lcd.print("All Steppers Reset!");
  Serial.println("Steppers Reset!");
  delay(1000);
}

void ledControl() {
  int subMenuIndex = 0;
  bool selected = false;

  while (!selected) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("LED Menu:");
    lcd.setCursor(1, 1);
    lcd.print(subMenuIndex == 0 ? "> Brightness" : "  Brightness");
    lcd.setCursor(1, 2);
    lcd.print(subMenuIndex == 1 ? "> Color" : "  Color");

    // Wait for encoder movement or button press
    int currentStateCLK = digitalRead(ENCODER_CLK);
    if (currentStateCLK != lastStateCLK) {
      if (digitalRead(ENCODER_DT) != currentStateCLK) {
        subMenuIndex++;
      } else {
        subMenuIndex--;
      }

      if (subMenuIndex > 1) subMenuIndex = 0;
      if (subMenuIndex < 0) subMenuIndex = 1;
    }
    lastStateCLK = currentStateCLK;

    if (digitalRead(ENCODER_SW) == LOW) {
      delay(200); // debounce
      selected = true;
    }
  }

  if (subMenuIndex == 0) {
    adjustBrightness();
  } else if (subMenuIndex == 1) {
    changeColor();
  }
}

void adjustBrightness() {
  bool adjusting = true;
  int brightnessValue = currentBrightness;
  int brightnessBarLength;
  lastStateCLK = digitalRead(ENCODER_CLK); // reset encoder state

  while (adjusting) {
    // Read encoder
    int currentStateCLK = digitalRead(ENCODER_CLK);
    if (currentStateCLK != lastStateCLK) {
      if (digitalRead(ENCODER_DT) != currentStateCLK) {
        brightnessValue += 5;
      } else {
        brightnessValue -= 5;
      }

      if (brightnessValue > 255) brightnessValue = 255;
      if (brightnessValue < 0) brightnessValue = 0;

      currentBrightness = brightnessValue;
      pixels.setBrightness(currentBrightness);
      pixels.show(); // Apply brightness

      // Update display
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Adjust Brightness");
      lcd.setCursor(0, 1);
      lcd.print("Value: ");
      lcd.print(currentBrightness);

      brightnessBarLength = map(currentBrightness, 0, 255, 0, 20);
      lcd.setCursor(0, 2);
      for (int i = 0; i < brightnessBarLength; i++) {
        lcd.write(255);  // Full block character
      }
      for (int i = brightnessBarLength; i < 20; i++) {
        lcd.print(" ");  // Fill the rest with blanks
      }
    }

    lastStateCLK = currentStateCLK;

    if (digitalRead(ENCODER_SW) == LOW) {
      delay(300);  // debounce
      adjusting = false;
    }
  }

  // Save brightness and return
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(redColor, greenColor, blueColor));
  }
  pixels.setBrightness(currentBrightness);
  pixels.show();
}

void changeColor() {
  redColor = random(0, 255);
  greenColor = random(0, 255);
  blueColor = random(0, 255);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Changing Color...");
  lcd.setCursor(0, 1);
  lcd.print("R:");
  lcd.print(redColor);
  lcd.print(" G:");
  lcd.print(greenColor);
  lcd.print(" B:");
  lcd.print(blueColor);

  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(redColor, greenColor, blueColor));
  }
  pixels.setBrightness(currentBrightness);
  pixels.show();

  delay(1500);
}