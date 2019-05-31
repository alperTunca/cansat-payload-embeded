/*
   alperTunca CanSat 2019 Payload Electronic Circuit Code

   -> Sensors in the system;
   -> ADXL345, BME280, RTC, SD Card, RGB Led, Servo, Buzzer
   
   The microSD adapter has never been run. It is waiting to run with SD card.
   
   Sensors ready for EEPROM 
      Team ID
      packetCount
      RTC

   Sensors ready for work
      Team ID
      packetCount
      ADXL
      BME280
      RTC
      Buzzer
      Servo
      RGB Led
   
*/

#include <EnvironmentCalculations.h>
#include <BME280I2C.h>
#include <Wire.h> // Wire library - For I2C comm.
#include <RTClib.h>
#include <SD.h>
#include <Servo.h>
#include <EEPROM.h>

#define SERIAL_BAUD 19200
#define TEAM_ID 2505
#define NOTE_C4  262
#define NOTE_G3  196
#define NOTE_A3  220
#define NOTE_B3  247

using namespace EnvironmentCalculations;

int packetCount = 0;

RTC_DS3231 rtc;// For RTC data variable
DateTime now; //The current time will keep variable

// Default environmental values:
float referencePressure = 1018.6;  // hPa local QFF (official meteor station reading)
float outdoorTemp = 4.7;           // ° C measured local outside temperature.
float barometerAltitude = 1650.3;  // meter ... map readings + barometer location

BME280I2C::Settings settings(
  BME280::OSR_X1,
  BME280::OSR_X1,
  BME280::OSR_X1,
  BME280::Mode_Forced,
  BME280::StandbyTime_1000ms,
  BME280::Filter_16,
  BME280::SpiEnable_False,
  BME280I2C::I2CAddr_0x76
);
BME280I2C bme(settings); //For BME280 data variable
float temp(NAN);   // Temperature
float hum(NAN);    // Humidity
float pres(NAN);   // Pressure
float altitude;   // Rakım

// RGB LED's pins
int redPin = 9;
int greenPin = 10;
int bluePin = 11;

Servo servoEngine;  // Create a servo object to control a servo
int servoPin = 6;
int pos = 0;    // To save the servo status
int dest = 40; // Targeted rotation angle
bool isServo = false; // Servo task completion state

int ADXL345 = 0x53; // The ADXL345 sensor I2C address
float X_out, Y_out, Z_out;  // Output Variables

// notes in the melody:
int melody[] = { NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4 };

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {  4, 8, 8, 4, 4, 4, 4, 4 };

int destAltitude = 20;

void setup()
{
  Serial.begin(SERIAL_BAUD);
  while (!Serial) {} // Waiting....

  Wire.begin();
  Wire.beginTransmission(ADXL345); // Start communicating with the device
  Wire.write(0x2D); // POWER_CTL register with Access / Speech Regulator - 0x2D
  Wire.write(8); // (8dec -> 0000 1000 binary)
  Wire.endTransmission();

  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  servoEngine.attach(servoPin);
}

void loop()
{

  now = rtc.now();
    if (!rtc.begin())
    {
    selectColor(255, 0, 0);
    Serial.println("RTC not found!");
    }
  if (!bme.begin())
  {
    selectColor(255, 0, 0);
    Serial.println("BME280 not found!");
  }


  else
  {
    if (packetCount == 0)
    {
      Serial.println("Ready to take off.");
    }
    selectColor(0, 255, 0);
    Serial.print("{");
    Serial.print(TEAM_ID);      //Team ID printed                                 <TEAM_ID>
    Serial.print(",");
    Serial.print(packetCount);  //Total number of incoming data packets           <PACKET COUNT>
    Serial.print(",");
    printRTCData();             //RTC values are printed                          <MISSION_TIME>
    printBME280Data(&Serial);   //Height data received from BME280                <ALTITUDE>
                                //Pressure data received from BME280              <PRESSURE>
                                //Temperature data received from BME280           <TEMP>
    printADXLData();            //PITCH information from ADXL345                  <PITCH>
                                //ROLL information from ADXL345                   <ROLL>
                                //YAW information from ADXL345                    <YAW>
                                //Gömülü'de koşullu olarak üretilen değerdir      <SOFTWARE STATE>
    Serial.println(";}");
    if (destAltitude > altitude);
    {
      if (isServo == false)
      {
        moveServo(pos, dest);
        playMusic();
        isServo = true;
      }
      else
      {
        servoEngine.detach();
      }
    }
    packetCount++;
    delay(500);
  }
}

void moveServo(int pos, int destination)
{
  // Used detach () to stop servo.
  for (int count = 0; count < destination; count++)
  {
    servoEngine.write(destination);
  }
}



void printADXLData()
{
  Wire.beginTransmission(ADXL345);
  Wire.write(0x32); //  Register starts with 0x32 (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(ADXL345, 6, true); // Total 10 register arrows, each axis value is stored at 2 registers
  X_out = ( Wire.read() | Wire.read() << 8); // X-axis
  X_out = X_out / 256; // We need to divide the raw values for the + -2g range by 256 according to the data sheet
  Y_out = ( Wire.read() | Wire.read() << 8); // Y-axis
  Y_out = Y_out / 256;
  Z_out = ( Wire.read() | Wire.read() << 8); // Z-axis
  Z_out = Z_out / 256;
  Serial.print(X_out);
  Serial.print(",");
  Serial.print(Y_out);
  Serial.print(",");
  Serial.print(Z_out);
  delay(100);
}

void selectColor(int R, int G, int B)
{
  // Brigthness 0-255
  // 0-0-0 White
  analogWrite(redPin, R);
  analogWrite(greenPin, G);
  analogWrite(bluePin, B);
  delay(100);
}

void printRTCData()
{
  Serial.print(now.day(), DEC);
  Serial.print(":");
  Serial.print(now.month(), DEC);
  Serial.print(":");
  Serial.print(now.year(), DEC);
  Serial.print(":");
  Serial.print(now.hour(), DEC);
  Serial.print(":");
  Serial.print(now.minute(), DEC);
  Serial.print(":");
  Serial.print(now.second(), DEC);
  Serial.print(",");
  delay(50);
}


void printBME280Data(Stream* client)
{

  BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
  BME280::PresUnit presUnit(BME280::PresUnit_hPa);
  bme.read(pres, temp, hum, tempUnit, presUnit);

  AltitudeUnit envAltUnit  =  AltitudeUnit_Meters;
  TempUnit     envTempUnit =  TempUnit_Celsius;

  altitude = Altitude(pres, envAltUnit, referencePressure, outdoorTemp, envTempUnit);
  client->print(altitude);
  client->print(",");
  client->print(temp);
  client->print(",");
  client->print(pres);
  client->print(",");
  delay(100);
}


void playMusic()
{
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 8; thisNote++)
  {
    // to calculate the note duration, take one second divided by the note type.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(8, melody[thisNote], noteDuration);
    // to distinguish the notes, set a minimum time between them.
    delay(noteDuration * 1.30);
    // stop the tone playing:
    noTone(8);
  }
}

void eepromPacketCount(int data, int  adress)
{
  int tempNum = data;
  int digit = 1;
  while (tempNum > 10)
  {
    tempNum = tempNum / 10;
    digit = digit + 1;
  }

  if (digit == 1 || digit == 2)
  {
    // en sağ 1. 2. hane
    EEPROM.update(adress, data);
  }
  else if (digit == 3 || digit == 4)
  {
    // en sağ 1. 2. hane
    EEPROM.update(adress + 1, data % 100);
    // en sağ 3. 4. hane
    EEPROM.update(adress, (data - (data % 100)) / 100);
  }
  else if (digit == 5 || digit == 6)
  {
    // en sağ 1. 2. hane
    EEPROM.update(adress + 2, data % 100);
    int data10k = data % 10000;
    // en sağdan 3. 4. hane
    EEPROM.update(adress + 1, (data10k - (data10k % 100)) / 100);
    // en sağdan 5. 6. hane
    EEPROM.update(adress, (data - data10k) / 10000);
  }
}

void eepromRTC(int adress)
{
  EEPROM.update(adress, now.day());
  EEPROM.update(adress+1, now.month());
  int year2digit = (now.year() - (now.year() % 2000)) / 100;
  EEPROM.update(adress+2, year2digit);
  EEPROM.update(adress+3, now.year() % 2000);
  EEPROM.update(adress+4, now.hour());
  EEPROM.update(adress+5, now.minute());
  EEPROM.update(adress+6, now.second());
}
