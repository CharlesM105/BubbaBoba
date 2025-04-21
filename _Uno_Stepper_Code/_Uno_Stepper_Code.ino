#include <AccelStepper.h>

int Homing1 = 0;
int Homing2 = 0;
int Homing3 = 0;
int HomingStepValue1 = 0;
int HomingStepValue2 = 0;
int HomingStepValue3 = 0;

AccelStepper stepper1(1,13,12);
AccelStepper stepper2(1,11,10);
AccelStepper stepper3(1,9,8);
AccelStepper stepper4(1,7,6);

void setup() {
    Serial.begin (9600);
    Serial.println("Power On");

    Homing1 = 1;
    Homing2 = 1;
    Homing3 = 1;
    HomingStepValue1 = 0;
    HomingStepValue2 = 0;
    HomingStepValue3 = 0;

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
}

void loop() {
    if (Homing1 != 0 && (Homing2 != 0 && Homing3 != 0)) {

        // homing stepper 1
        if (Homing1 != 0){
            stepper1.setMaxSpeed(3000.0);  
            stepper1.setAcceleration(3000.0); 
            Serial.println("Stepper 1 is Homing");
            HomingStepValue1 = 1;
            while(digitalRead(4) != HIGH){  
                stepper1.moveTo(HomingStepValue1);  
                HomingStepValue1++;  
                stepper1.run(); 
                delay(2);
            }
            stepper1.setCurrentPosition(0);  
            stepper1.setMaxSpeed(3000);      
            stepper1.setAcceleration(3000.0); 
            HomingStepValue1 = -1;
            Serial.println("Homing Switch 1 Hit");
            stepper1.setCurrentPosition(0);
            Serial.println("Homing 1 Completed");
            Homing1 = 0;
        }

        // homing stepper 2
        if (Homing2 != 0){
            stepper4.setMaxSpeed(3000.0);  
            stepper4.setAcceleration(3000.0); 
            Serial.println("Stepper 2 is Homing");
            HomingStepValue2 = -1;
            while(digitalRead(2) != HIGH){  
                stepper4.moveTo(HomingStepValue2);  
                HomingStepValue2--;  
                stepper4.run(); 
                delay(2);
            }
            stepper4.setCurrentPosition(0);  
            stepper4.setMaxSpeed(3000.0);      
            stepper4.setAcceleration(3000.0); 
            HomingStepValue2 = -1;
            Serial.println("Homing Switch 2 Hit");
            stepper4.setCurrentPosition(0);
            Serial.println("Homing 2 Completed");
            Homing2 = 0;
        }

        // homing stepper 3
        if (Homing3 != 0){
            stepper2.setMaxSpeed(3000.0);  
            stepper2.setAcceleration(3000.0); 
            Serial.println("Stepper 3 is Homing");
            HomingStepValue3 = -1;
            while(digitalRead(3) != HIGH){  
                stepper2.moveTo(HomingStepValue3);  
                HomingStepValue3--;  
                stepper2.run(); 
                delay(2);
            }
            stepper2.setCurrentPosition(0);  
            stepper2.setMaxSpeed(3000.0);      
            stepper2.setAcceleration(3000.0); 
            HomingStepValue3 = -1;
            Serial.println("Homing Switch 3 Hit");
            stepper2.setCurrentPosition(0);
            Serial.println("Homing 3 Completed");
            Homing3 = 0;
        }

        Serial.println("Full Homing Sequence Completed");

        delay(1000);
        // ==== ADDED SECTION: Move X and Y axes 100 steps ====
        Serial.println("Moving X and Y 100 steps...");
        
        // Move stepper1 (X axis) 100 steps
        stepper1.move(-300);
        while (stepper1.isRunning()) {
            stepper1.run();
        }

        // Move stepper4 (Y axis) 100 steps
        stepper4.move(500);
        while (stepper4.isRunning()) {
            stepper4.run();
        }

        //moves z axis
        stepper2.setMaxSpeed(1000.0);  
        stepper2.setAcceleration(1000.0); 
        stepper2.move(-600);
        while (stepper2.isRunning()){
          stepper2.run();
        }

        Serial.println("X and Y movement complete.");
    }
}