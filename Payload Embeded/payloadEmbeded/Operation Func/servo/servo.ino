#include <Servo.h>

Servo servoEngine;  // Create a servo object to control a servo
int count;
int destination=60;
bool isServo= false;

void setup() 
{
  servoEngine.attach(6);  // Attaches pin 9 to servo object
  Serial.begin(9600);
  count = 0;
}

void loop() 
{
  if(isServo==false)
  {
    moveServo(0,120); 
    isServo=true;
  }
  else
  {
    Serial.println("Servo Okey");
    servoEngine.detach();
  }
}

  void moveServo(int pos, int dest)
  { // A rotation angle from 0 to 120 degrees is provided.
    // The reason for 2 variables is to hold the end position. (pos,dest)
    for (count=0; count < dest; count++)
      {
        servoEngine.write(dest);
      }
  }
