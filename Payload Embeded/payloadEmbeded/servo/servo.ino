#include <Servo.h>

Servo servoEngine;  // Bir servoyu kontrol etmek için servo nesnesi oluşturmak
int count;
int destination=60;
bool isServo= false;

void setup() 
{
  servoEngine.attach(9);  // 9 no'lu pini servo nesnesine tutturur
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

  void moveServo(int pos, int destination)
  {
    for (count=0; count < destination; count++)
      {
        servoEngine.write(destination);
      }
  }
