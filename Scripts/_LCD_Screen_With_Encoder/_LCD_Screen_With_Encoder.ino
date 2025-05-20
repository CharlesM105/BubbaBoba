#include <LiquidCrystal_I2C.h>

// LCD Setup
#define I2C_ADDR 0x27
#define LCD_COLUMNS 20
#define LCD_LINES 4
LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_LINES);

// Rotary Encoder Pins
#define ENCODER_CLK 2
#define ENCODER_DT 3
#define ENCODER_SW 4

// Saved Ratios (default values)
int milkRatio = 60;
int flavorRatio = 20;
int bobaRatio = 20;

// Drink Menu
String drinkMenu[] = {
  "Brown Sugar Tea",
  "Strawberry Milk",
  "Taro Milk Tea",
  "Back to Control"
};
int drinkMenuSize = sizeof(drinkMenu) / sizeof(drinkMenu[0]);

// Control Menu
String menuItems[] = {
  "Run Diagnostics",
  "RFID Access",
  "Servo Trigger",
  "Manual Axis Ctrl",
  "LED Settings",
  "Control Menu",
  "Reset System"
};
int menuSize = sizeof(menuItems) / sizeof(menuItems[0]);

volatile int menuIndex = 0;
int menuStartIndex = 0;
int lastStateCLK;
bool buttonPressed = false;
bool inDrinkMenu = false;

void setup() {
  lcd.init();
  lcd.backlight();

  pinMode(ENCODER_CLK, INPUT);
  pinMode(ENCODER_DT, INPUT);
  pinMode(ENCODER_SW, INPUT_PULLUP);
  lastStateCLK = digitalRead(ENCODER_CLK);

  runHomingSequence();
  showDrinkMenu();
}

void loop() {
  handleRotaryEncoder();

  if (digitalRead(ENCODER_SW) == LOW && !buttonPressed) {
    buttonPressed = true;
    showSelectedMenu();
  } else if (digitalRead(ENCODER_SW) == HIGH) {
    buttonPressed = false;
  }
}

void runHomingSequence() {
  lcd.clear();
  lcd.setCursor(1, 0); lcd.print("Homing Axes...");
  lcd.setCursor(0, 2); lcd.print("Press to Skip");
  delay(500);

  while (digitalRead(ENCODER_SW) == HIGH) {
    delay(10);
    // TODO: Add motor homing code here
  }
}

void showDrinkMenu() {
  lcd.clear();
  lcd.setCursor(1, 1); lcd.print("Milk Tea Selection");
  delay(1500);

  menuIndex = 0;
  menuStartIndex = 0;
  inDrinkMenu = true;
  updateMenuDisplay();
}

void handleRotaryEncoder() {
  int currentStateCLK = digitalRead(ENCODER_CLK);
  if (currentStateCLK != lastStateCLK) {
    if (digitalRead(ENCODER_DT) != currentStateCLK) {
      menuIndex++;
    } else {
      menuIndex--;
    }

    int currentMenuSize = inDrinkMenu ? drinkMenuSize : menuSize;

    if (menuIndex >= currentMenuSize) menuIndex = 0;
    if (menuIndex < 0) menuIndex = currentMenuSize - 1;

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
    if (inDrinkMenu && itemIndex < drinkMenuSize) {
      lcd.setCursor(1, i + 1);
      lcd.print((itemIndex == menuIndex) ? "> " : "  ");
      lcd.print(drinkMenu[itemIndex]);
    } else if (!inDrinkMenu && itemIndex < menuSize) {
      lcd.setCursor(1, i + 1);
      lcd.print((itemIndex == menuIndex) ? "> " : "  ");
      lcd.print(menuItems[itemIndex]);
    }
  }
}

void showSelectedMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Selected:");
  lcd.setCursor(2, 2);

  if (inDrinkMenu) {
    lcd.print(drinkMenu[menuIndex]);
    delay(1000);
    if (drinkMenu[menuIndex] == "Back to Control") {
      inDrinkMenu = false;
      menuIndex = 0;
      menuStartIndex = 0;
    } else {
      askBobaPreference();
    }
  } else {
    lcd.print(menuItems[menuIndex]);
    delay(1000);

    if (menuItems[menuIndex] == "Reset System") {
      runHomingSequence();
      showDrinkMenu();
      return;
    }

    if (menuItems[menuIndex] == "Control Menu") {
      openControlMenu();
      return;
    }
  }

  updateMenuDisplay();
}

void askBobaPreference() {
  bool addBoba = true;
  bool confirmed = false;

  while (!confirmed) {
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("Add Boba?");
    lcd.setCursor(2, 2); lcd.print(addBoba ? "Yes" : "No");

    int currentStateCLK = digitalRead(ENCODER_CLK);
    if (currentStateCLK != lastStateCLK) {
      addBoba = !addBoba;
    }
    lastStateCLK = currentStateCLK;

    if (digitalRead(ENCODER_SW) == LOW) {
      confirmed = true;
      delay(300);
    }
  }

  if (addBoba) {
    adjustBobaPercentage();
  } else {
    bobaRatio = 0;
  }

  bool addSyrup = false;
  confirmed = false;

  while (!confirmed) {
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("Add Syrup Ring?");
    lcd.setCursor(2, 2); lcd.print(addSyrup ? "Yes" : "No");

    int currentStateCLK = digitalRead(ENCODER_CLK);
    if (currentStateCLK != lastStateCLK) {
      addSyrup = !addSyrup;
    }
    lastStateCLK = currentStateCLK;

    if (digitalRead(ENCODER_SW) == LOW) {
      confirmed = true;
      delay(300);
    }
  }

  lcd.clear();
  lcd.print("Creating drink...");
  delay(1500);

  startDrinkMakingProcess(addSyrup);
}

void adjustBobaPercentage() {
  int percentage = bobaRatio;

  while (true) {
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("Set Boba Amount");
    lcd.setCursor(2, 2); lcd.print(percentage); lcd.print("%");

    int currentStateCLK = digitalRead(ENCODER_CLK);
    if (currentStateCLK != lastStateCLK) {
      percentage += (digitalRead(ENCODER_DT) != currentStateCLK) ? 5 : -5;
      percentage = constrain(percentage, 0, 100);
    }
    lastStateCLK = currentStateCLK;

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

  if (bobaRatio > 0) {
    moveToBoba();
    dispenseBoba();
  }

  liftCup();
  presentCupToUser();
  waitToStartNewOrder();
}

void slideCupOut() {
  lcd.clear();
  lcd.print("Sliding cup out...");
  delay(1500); // TODO: Replace with motor movement
}

void waitForCupGrab() {
  lcd.clear();
  lcd.print("Place cup inside");
  lcd.setCursor(0, 2);
  lcd.print("Press to cont...");
  while (digitalRead(ENCODER_SW) == HIGH) delay(10);
}

void returnCupToCenter() {
  lcd.clear();
  lcd.print("Returning cup...");
  delay(1500); // TODO: Replace with motor logic
}

void applySyrupRing() {
  lcd.clear();
  lcd.print("Applying syrup ring...");
  delay(1500); // TODO: Replace with motor + dispenser logic
}

void dispenseMilk() {
  lcd.clear();
  lcd.print("Dispensing Milk");
  lcd.setCursor(0, 2); lcd.print(milkRatio); lcd.print("%");
  delay(1500); // TODO: Add milk pump logic
}

void dispenseFlavor() {
  lcd.clear();
  lcd.print("Adding Flavor");
  lcd.setCursor(0, 2); lcd.print(flavorRatio); lcd.print("%");
  delay(1500); // TODO: Add flavor logic
}

void mixDrink() {
  lcd.clear();
  lcd.print("Mixing drink...");
  delay(2000); // TODO: Mixer motor logic
}

void moveToBoba() {
  lcd.clear();
  lcd.print("Positioning Boba");
  delay(1000); // TODO: motor movement to boba dispenser
}

void dispenseBoba() {
  lcd.clear();
  lcd.print("Dispensing Boba");
  delay(1500); // TODO: add boba dispensing logic
}

void liftCup() {
  lcd.clear();
  lcd.print("Raising mixer...");
  delay(1000); // TODO: Raise Z axis
}

void presentCupToUser() {
  lcd.clear();
  lcd.print("Slide out cup");
  lcd.setCursor(0, 1);
  lcd.print("Grab & Press Btn");
  while (digitalRead(ENCODER_SW) == HIGH) delay(10);

  lcd.clear();
  lcd.print("Cup retrieved!");
  delay(1000);
}

void waitToStartNewOrder() {
  lcd.clear();
  lcd.print("Start another?");
  lcd.setCursor(0, 2);
  lcd.print("Press button...");

  while (digitalRead(ENCODER_SW) == HIGH) delay(10);

  lcd.clear();
  lcd.print("Resetting system...");
  delay(1000);

  runHomingSequence();
  showDrinkMenu();
}

void openControlMenu() {
  int* values[3] = { &milkRatio, &flavorRatio, &bobaRatio };
  const char* labels[3] = { "Milk %", "Flavor %", "Boba %" };

  for (int i = 0; i < 3; i++) {
    while (true) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Adjust " + String(labels[i]));
      lcd.setCursor(2, 2);
      lcd.print(*values[i]); lcd.print("%");

      int currentStateCLK = digitalRead(ENCODER_CLK);
      if (currentStateCLK != lastStateCLK) {
        *values[i] += (digitalRead(ENCODER_DT) != currentStateCLK) ? 5 : -5;
        *values[i] = constrain(*values[i], 0, 100);
      }
      lastStateCLK = currentStateCLK;

      if (digitalRead(ENCODER_SW) == LOW) {
        delay(300);
        break;
      }
    }
  }

  lcd.clear();
  lcd.print("Defaults Saved!");
  delay(1000);
}