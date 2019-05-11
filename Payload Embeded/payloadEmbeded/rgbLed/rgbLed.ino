  int redPin=9;
  int greenPin=10;
  int bluePin=11;
  int brightness=255;
  int zero=0;
  int selectPin=0;
   
  void setup() {
   
  Serial.begin(9600);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  }
   
  void loop() 
  {   
    //selectPin = rand()%3;
    selectPin = 3;
    if (selectPin==1) 
    {
    analogWrite(redPin, brightness);
    analogWrite(greenPin, zero);
    analogWrite(bluePin, zero);
    Serial.println("255-0-0");
    delay(1000);
    }
    if (selectPin==2) 
    {
    analogWrite(redPin, zero);
    analogWrite(greenPin, zero);
    analogWrite(bluePin, brightness);
    Serial.println("0-0-255");
    delay(1000);
    }
    if (selectPin==3) 
    {
    analogWrite(redPin, zero);
    analogWrite(greenPin, brightness);
    analogWrite(bluePin, zero);
    Serial.println("0-255-0");
    delay(1000);
    } 
    else
    { 
    analogWrite(redPin, brightness);
    analogWrite(greenPin, brightness);
    analogWrite(bluePin, brightness);
    Serial.println("255-255-255");
    delay(1000);
    } 
  }
