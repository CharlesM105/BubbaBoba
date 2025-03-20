#include <LiquidCrystal_I2C.h>

#define ENCODER_CLK 2   // Rotary Encoder CLK pin
#define ENCODER_DT 3    // Rotary Encoder DT pin
#define ENCODER_SW 4    // Rotary Encoder push button (for selection)

LiquidCrystal_I2C lcd(0x27, 20, 4); // I2C address 0x27, 20 columns, 4 rows

// ---- Menu Items ----
String menuItems[] = {
  "Option 1",
  "Option 2",
  "Option 3",
  "Option 4",
  "Option 5",
  "Option 6",
  "Option 7"
};
int menuSize = sizeof(menuItems) / sizeof(menuItems[0]); // Auto-detect menu size

volatile int menuIndex = 0;  // Current selection index
int lastStateCLK;            // Previous state of CLK pin
bool buttonPressed = false;  // State of encoder button
int menuStartIndex = 0;      // Controls the scrolling

void setup() {
  lcd.init(); // Initialize the LCD
  lcd.backlight();

  // Initialize Rotary Encoder pins
  pinMode(ENCODER_CLK, INPUT);
  pinMode(ENCODER_DT, INPUT);
  pinMode(ENCODER_SW, INPUT_PULLUP);

  lastStateCLK = digitalRead(ENCODER_CLK); // Read initial state

  updateMenuDisplay();
}

void loop() {
  // Read current state of CLK
  int currentStateCLK = digitalRead(ENCODER_CLK);
  
  // If the state changed, determine direction
  if (currentStateCLK != lastStateCLK) {
    if (digitalRead(ENCODER_DT) != currentStateCLK) {
      menuIndex++; // Scroll down
    } else {
      menuIndex--; // Scroll up
    }

    // Circular scrolling: loop around when reaching limits
    if (menuIndex >= menuSize) {
      menuIndex = 0;  // Wrap around to first option
      menuStartIndex = 0;
    }
    if (menuIndex < 0) {
      menuIndex = menuSize - 1;  // Wrap around to last option
      menuStartIndex = menuSize - 3;
      if (menuStartIndex < 0) menuStartIndex = 0;  // Prevent negative index
    }

    // Adjust menu start index for scrolling
    if (menuIndex >= menuStartIndex + 3) {
      menuStartIndex++;
    } else if (menuIndex < menuStartIndex) {
      menuStartIndex--;
    }

    updateMenuDisplay(); // Update display with new selection
  }
  
  lastStateCLK = currentStateCLK; // Save last state

  // Check if the encoder push button is pressed
  if (digitalRead(ENCODER_SW) == LOW) {
    if (!buttonPressed) {
      selectMenuItem(); // Execute selected option
      buttonPressed = true;
    }
  } else {
    buttonPressed = false; // Reset flag when button is released
  }
}

// Function to update the menu display
void updateMenuDisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Select an Option:");

  // Display up to 3 menu items at a time
  for (int i = 0; i < 3; i++) {
    int itemIndex = menuStartIndex + i;
    if (itemIndex < menuSize) {
      lcd.setCursor(1, i + 1);
      if (itemIndex == menuIndex) {
        lcd.print("> "); // Highlight selected item
      } else {
        lcd.print("  "); // Regular spacing
      }
      lcd.print(menuItems[itemIndex]);
    }
  }
}

// Function to execute selected menu item
void selectMenuItem() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("You selected:");
  
  lcd.setCursor(2, 2);
  lcd.print(menuItems[menuIndex]); // Show selected item

  delay(1500); // Pause for user to see selection
  updateMenuDisplay(); // Return to menu
}