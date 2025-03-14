//Yummy Boba Code


//libaries
#include <AccelStepper.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
//RFID Code Portion
#include <SPI.h>
#include <MFRC522.h>

//variables
int Homing1 = 0;
int Homing2 = 0;
int Homing3 = 0;
int HomingStepValue1 = 0;
int HomingStepValue2 = 0;
int HomingStepValue3 = 0;
bool isAllowed = false; //Variable to allow code to continue once RFID is detected

//lcd stuff
#define I2C_ADDR    0x27
#define LCD_COLUMNS 20
#define LCD_LINES   4
LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_LINES);

//RFID Code Portion
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

//rotary encoder stuff
#define SW 43
#define DT 45
#define CLK 47
 int counter = 0; 
 int aState;
 int aLastState;  

//stepper n servo stuff
AccelStepper stepper1(1,13,12);
AccelStepper stepper2(1,11,10);
AccelStepper stepper3(1,9,8);
AccelStepper stepper4(1,7,6);
  Servo servo_A0;



//start up code
 void setup() { 

//serial monitor stuff
     Serial.begin (9600);
     Serial.println("Power On");


//lcd stuff
   lcd.init();
   lcd.backlight();
  lcd.setCursor(5, 1);
  lcd.print("-POWER ON-");
  lcd.setCursor(2, 1);

//variable stuff
      Homing1 = 1;
      Homing2 = 1;
      Homing3 = 1;
      HomingStepValue1 = 0;
      HomingStepValue2 = 0;
      HomingStepValue3 = 0;

//pin modes
   pinMode (A1,INPUT);
   pinMode (5,INPUT);
   pinMode (4,INPUT);
   pinMode (3,INPUT);
   pinMode (A0,OUTPUT);
   pinMode (A2,OUTPUT);
   pinMode (CLK,INPUT);
   pinMode (DT,INPUT);
   pinMode (SW,INPUT);

//stepper values reset
   stepper1.setMaxSpeed(1000);
   stepper2.setMaxSpeed(1000);
   stepper3.setMaxSpeed(1000);
   stepper4.setMaxSpeed(1000);

   stepper1.setAcceleration(1000);
   stepper2.setAcceleration(1000);
   stepper3.setAcceleration(1000);
   stepper4.setAcceleration(1000);

   stepper1.setCurrentPosition(0);
   stepper2.setCurrentPosition(0);
   stepper3.setCurrentPosition(0);
   stepper4.setCurrentPosition(0);


//initial button check
   if(digitalRead(2) == HIGH){
      Serial.println("Warning: Button 1 Pressed Early");
   }
      if(digitalRead(3) == HIGH){
      Serial.println("Warning: Button 2 Pressed Early");
   }
      if(digitalRead(4) == HIGH){
      Serial.println("Warning: Button 3 Pressed Early");
   }


//servo reset
  servo_A0.attach(A0, 500, 2500);
        servo_A0.write(0);
        delay(100);
   Serial.println("Servo Reset");
   delay(100);

//more rotary encoder stuff
   aLastState = digitalRead(CLK);   


     Serial.println("Initialization Complete");     
     delay(1000);

  //RFID Portion
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();
}



//main code
 void loop() { 

  //RFID Portion
// Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) == "87 E5 BE A7" || "EA FA 9C 82") //change here the UID of the card/cards that you want to give access
  {
    bool isAllowed = true; 
    Serial.println("Authorized access");
    Serial.println();
    delay(3000);
  }
 
 else   {
    bool isAllowed = false; 
    Serial.println(" Access denied");
    delay(3000);
  }


//rotary encoder stuff
    aState = digitalRead(CLK); 
 
   if (aState != aLastState){     
     if (digitalRead(DT) != aState) { 
       counter ++;
     } else {
       counter --;
     }
     Serial.println(counter);
   } 
   aLastState = aState; 



//full homing sequence
if (Homing1 != 0 && (Homing2 != 0 && Homing3 != 0)) {

//lcd stuff
    lcd.clear();
    lcd.setCursor(6, 1);
    lcd.print("-HOMING-");
    lcd.setCursor(4, 3);
    lcd.print("DO NOT TOUCH");

//homing stepper 1
     if (Homing1 != 0){

  stepper1.setMaxSpeed(100.0);  
  stepper1.setAcceleration(100.0); 

   Serial.println("Stepper 1 is Homing");

   HomingStepValue1 = 1;

  while(digitalRead(2) != HIGH){  
    stepper1.moveTo(HomingStepValue1);  
    HomingStepValue1++;  
    stepper1.run(); 
    delay(5);
  }

  stepper1.setCurrentPosition(0);  

  stepper1.setMaxSpeed(100.0);      
  stepper1.setAcceleration(100.0); 

  HomingStepValue1 = -1;
   Serial.println("Homing Switch 1 Hit");

  stepper1.setCurrentPosition(0);
  Serial.println("Homing 1 Completed");
  
     Homing1 = 0;
 }


//homing stepper 2
      if (Homing2 != 0){

  stepper2.setMaxSpeed(100.0);  
  stepper2.setAcceleration(100.0); 

   Serial.println("Stepper 2 is Homing");

   HomingStepValue2 = 1;

  while(digitalRead(3) != HIGH){  
    stepper2.moveTo(HomingStepValue2);  
    HomingStepValue2++;  
    stepper2.run(); 
    delay(5);
  }

  stepper2.setCurrentPosition(0);  

  stepper2.setMaxSpeed(100.0);      
  stepper2.setAcceleration(100.0); 

  HomingStepValue2 = -1;
   Serial.println("Homing Switch 2 Hit");

  stepper2.setCurrentPosition(0);
  Serial.println("Homing 2 Completed");
  
     Homing2 = 0;
 }


//homing stepper 3
      if (Homing3 != 0){

  stepper3.setMaxSpeed(100.0);  
  stepper3.setAcceleration(100.0); 

   Serial.println("Stepper 3 is Homing");

   HomingStepValue3 = 1;

  while(digitalRead(4) != HIGH){  
    stepper3.moveTo(HomingStepValue3);  
    HomingStepValue3++;  
    stepper3.run(); 
    delay(5);
  }

  stepper3.setCurrentPosition(0);  

  stepper3.setMaxSpeed(100.0);      
  stepper3.setAcceleration(100.0); 

  HomingStepValue3 = -1;
   Serial.println("Homing Switch 3 Hit");
   
  stepper3.setCurrentPosition(0);
  Serial.println("Homing 3 Completed");
  
     Homing3 = 0;
  }

Serial.println("Full Homing Sequence Completed");

//lcd stuff
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print(" -HOMING  COMPLETE-");
 }
//write the rest of the stuff here
}
