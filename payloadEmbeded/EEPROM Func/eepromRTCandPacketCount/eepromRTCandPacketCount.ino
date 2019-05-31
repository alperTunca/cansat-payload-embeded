#include <EEPROM.h>
#include <RTClib.h>

RTC_DS3231 rtc;

int packetCount=0;

void setup() 
{
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  Serial.begin(9600);
}

void loop() 
{
  DateTime now = rtc.now();
  packetCount++;
  delay(300);
  eepromRTC(0);  
  eepromPacketCount(1555,5);
  Serial.print(EEPROM.read(0));
  Serial.print(",");
  Serial.print(EEPROM.read(1));
  Serial.print(",");
  Serial.print(EEPROM.read(2));
  Serial.print(",");
  Serial.println(EEPROM.read(0));  
}

void eepromRTC(int adress)
{
  EEPROM.update(0, now.day());
  EEPROM.update(1, now.month());
  int year2digit = (now.year()-(now.year()%2000))/100;
  EEPROM.update(2, year2digit);
  EEPROM.update(3, now.year()%2000);
  EEPROM.update(4, now.hour());
  EEPROM.update(5, now.minute());
  EEPROM.update(6, now.second());
}


void eepromPacketCount(int data, int  adress)
{
  int tempNum=data;
  int digit=1;
  while(tempNum>10)
  {
    tempNum=tempNum/10;
    digit=digit+1;
  }

  if (digit==1 || digit==2)
  {
    // 1st and 2nd digit from the right
    EEPROM.update(adress,data);
  }
  else if (digit==3 || digit==4)
  {
    // 1st and 2nd digit from the right
    EEPROM.update(adress+1,data%100);
    // 3rd and 4th digit from the right
    EEPROM.update(adress,(data-(data%100))/100)
  }
  else if (digit==5 || digit==6)
  {
    // 1st and 2nd digit from the right
    EEPROM.update(adress+2,data%100);
    int data10k=data%10000;
    // 3rd and 4th digit from the right
    EEPROM.update(adress,(data10k-(data10k%100))/100);
    // 5th and 6th digit from the right
    EEPROM.update(adress,(data-data10k)/10000);
  }
}
  
