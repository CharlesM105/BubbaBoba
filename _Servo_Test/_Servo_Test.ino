#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

void setup() {
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
}

void loop() {

    myservo.write(360);              // tell servo to go to position in variable 'pos'
    delay(5000);          
        myservo.write(0);              // tell servo to go to position in variable 'pos'
    delay(10000);                   // waits 15ms for the servo to reach the position               // waits 15ms for the servo to reach the position
  }

