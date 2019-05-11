#include <Servo.h>

Servo myservo;  // Bir servoyu kontrol etmek için servo nesnesi oluşturmak
int pos = 0;    // servo konumunu saklamak için değişken

void setup() 
{
  myservo.attach(9);  // 9 no'lu pini servo nesnesine tutturur
}

void loop() 
{
  for (pos = 0; pos <= 180; pos += 1) { // 0 dereceden 180 dereceye kadar gider
                                        // her adım 1 derece
    myservo.write(pos);                 // servo'ya 'pos' değişkeninde pozisyona gitmesini söyle
    delay(15);                       
  }
  
  for (pos = 180; pos >= 0; pos -= 1)   //  180 dereceden 0 dereceye kadar gider
  { 
    myservo.write(pos);                 // servo'ya 'pos' değişkeninde pozisyona gitmesini söyle
    delay(15);                      
  }
}
