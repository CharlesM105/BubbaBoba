//Homing Code :)

//libaries
#include <AccelStepper.h>

//variables
int Homing1 = 0;
int Homing2 = 0;
int Homing3 = 0;
int HomingStepValue1 = 0;
int HomingStepValue2 = 0;
int HomingStepValue3 = 0;

//stepper stuff
AccelStepper stepper1(1,13,12);
AccelStepper stepper2(1,9,8);
AccelStepper stepper3(1,11,10);
AccelStepper stepper4(1,7,6);

//start up code

 void setup() { 

//serial monitor stuff
     Serial.begin (115200);
     Serial.println("Power On");

//variable stuff
      Homing1 = 1;
      Homing2 = 1;
      Homing3 = 1;
      HomingStepValue1 = 0;
      HomingStepValue2 = 0;
      HomingStepValue3 = 0;

//pin modes
   pinMode (4,INPUT);
   pinMode (3,INPUT);
   pinMode (2,INPUT);

//stepper values reset
   stepper1.setMaxSpeed(3000);
   stepper2.setMaxSpeed(3000);
   stepper3.setMaxSpeed(3000);
   stepper4.setMaxSpeed(3000);

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
 }

//main code
 void loop() { 

//full homing sequence
if (Homing1 != 0 && (Homing2 != 0 && Homing3 != 0)) {

//homing stepper 1
     if (Homing1 != 0){

  stepper1.setMaxSpeed(5000.0);  
  stepper1.setAcceleration(1000.0); 

   Serial.println("Stepper 1 is Homing");

   HomingStepValue1 = 1;

  while(digitalRead(4) != HIGH){  
    stepper1.moveTo(HomingStepValue1);  
    HomingStepValue1++;  
    stepper1.run(); 
    delay(5);
  }

  stepper1.setCurrentPosition(0);  

  stepper1.setMaxSpeed(5000.0);      
  stepper1.setAcceleration(1000.0); 

  HomingStepValue1 = -1;
   Serial.println("Homing Switch 1 Hit");

  stepper1.setCurrentPosition(0);
  Serial.println("Homing 1 Completed");
  
     Homing1 = 0;
 }


//homing stepper 2
      if (Homing2 != 0){

  stepper4.setMaxSpeed(5000.0);  
  stepper4.setAcceleration(1000.0); 

   Serial.println("Stepper 2 is Homing");

   HomingStepValue2 = 1;

  while(digitalRead(2) != HIGH){  
    stepper4.moveTo(HomingStepValue2);  
    HomingStepValue2--;  
    stepper4.run(); 
    delay(5);
  }

  stepper4.setCurrentPosition(0);  

  stepper4.setMaxSpeed(5000.0);      
  stepper4.setAcceleration(1000.0); 

  HomingStepValue2 = -1;
   Serial.println("Homing Switch 2 Hit");

  stepper4.setCurrentPosition(0);
  Serial.println("Homing 2 Completed");
  
     Homing2 = 0;
 }


//homing stepper 3
      if (Homing3 != 0){

  stepper3.setMaxSpeed(20000.0);  
  stepper3.setAcceleration(1000.0); 

   Serial.println("Stepper 3 is Homing");

   HomingStepValue3 = -1;

  while(digitalRead(3) != HIGH){  
    stepper3.moveTo(HomingStepValue3);  
    HomingStepValue3--;  
    stepper3.run(); 
    delay(2);
  }

  stepper3.setCurrentPosition(0);  

  stepper3.setMaxSpeed(20000.0);      
  stepper3.setAcceleration(1000.0); 

  HomingStepValue3 = +1;
   Serial.println("Homing Switch 3 Hit");
   
  stepper3.setCurrentPosition(0);
  Serial.println("Homing 3 Completed");
  
     Homing3 = 0;
  }

Serial.println("Full Homing Sequence Completed"); 
 }
}