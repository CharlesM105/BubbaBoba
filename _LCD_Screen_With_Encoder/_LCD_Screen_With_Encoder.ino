/*
 * Created by ArduinoGetStarted.com
 *
 * Modified to include a Rotary Encoder with a separate clear button by ChatGPT
 *
 * This example code is in the public domain
 *
 * Tutorial page: https://arduinogetstarted.com/tutorials/arduino-lcd-20x4
 */

#include <LiquidCrystal_I2C.h>

#define ENCODER_CLK 2   // Rotary Encoder CLK pin
#define ENCODER_DT 3    // Rotary Encoder DT pin
#define ENCODER_SW 4    // Rotary Encoder push button (not used for clearing)
#define CLEAR_PIN 5     // Separate button for clearing the counter

LiquidCrystal_I2C lcd(0x27, 20, 4); // I2C address 0x27, 20 columns, 4 rows

volatile int counter = 0;  // Counter variable
int lastStateCLK;          // Previous state of CLK pin
bool buttonPressed = false; // State of encoder button
bool clearButtonPressed = false; // State of clear button

void setup() {
  lcd.init(); // Initialize the LCD
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Hello World");

  // Initialize Rotary Encoder pins
  pinMode(ENCODER_CLK, INPUT);
  pinMode(ENCODER_DT, INPUT);
  pinMode(ENCODER_SW, INPUT_PULLUP);
  pinMode(CLEAR_PIN, INPUT_PULLUP); // Set clear button pin

  lastStateCLK = digitalRead(ENCODER_CLK); // Read initial state

  lcd.setCursor(0, 2);
  lcd.print("Counter: 0");
}

void loop() {
  // Read current state of CLK
  int currentStateCLK = digitalRead(ENCODER_CLK);
  
  // If the state changed, determine direction
  if (currentStateCLK != lastStateCLK) {
    if (digitalRead(ENCODER_DT) != currentStateCLK) {
      counter++;
    } else {
      counter--;
    }

    // Update display
    lcd.setCursor(9, 2); // Position to update counter
    lcd.print("     ");  // Clear previous value
    lcd.setCursor(9, 2);
    lcd.print(counter);
  }
  
  lastStateCLK = currentStateCLK; // Save last state

  // Check if the clear button is pressed
  if (digitalRead(CLEAR_PIN) == LOW) {
    if (!clearButtonPressed) {
      counter = 0; // Reset counter
      lcd.setCursor(9, 2);
      lcd.print("     "); // Clear previous value
      lcd.setCursor(9, 2);
      lcd.print(counter);
      clearButtonPressed = true;
    }
  } else {
    clearButtonPressed = false;
  }
}