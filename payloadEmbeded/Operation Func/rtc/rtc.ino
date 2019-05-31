#include <RTClib.h>
  /*
   * I2C Commm. (Wire.h)
   * 
   * Vin -->  3.3V
   * Gnd -->  Gnd
   * SDA -->  A4 
   * SCL -->  A5 
   */
RTC_DS3231 rtc;
void setup() {
  
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  Serial.begin(9600);
}

void loop() {
  DateTime now = rtc.now();
  Serial.print(now.day(), DEC);
  Serial.print(".");
  Serial.print(now.month(), DEC);
  Serial.print(".");
  Serial.print(now.year(), DEC);
  Serial.print(",");
  Serial.print(now.hour(), DEC);
  Serial.print(":");
  Serial.print(now.minute(), DEC);
  Serial.print(":");
  Serial.print(now.second(), DEC);
  Serial.println(";");
  delay(10000);
}
