A#include <TinyGPS++.h>
#include <RTClib.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>
#include <BME280I2C.h>
#include <Wire.h>
#include <EnvironmentCalculations.h>
#include <MPU6050.h>
#include <Servo.h>

#define referencePressure  1016.3f // hPa local QFF (official meteor-station reading)
#define outdoorTemp  29.1f // °C  measured local outdoor temp.
#define barometerAltitude  50.3f


RTC_DS3231 rtc;
SoftwareSerial xBee(2, 3); // RX, TX

unsigned int sum = 0;
// The TinyGPS++ object
TinyGPSPlus gps;
BME280I2C bme;

MPU6050 acc_sensor;
int16_t gx, gy, gz; // Gyro Defination
unsigned char sign = 0;
float temp = 0;
unsigned long start_time = 0;
unsigned long end_time = 0;

int power_failure = 1;
bool climb = false;
bool descent = false;

File file;
Servo myservo;

// The serial connection to the GPS device


unsigned int pckCnt = 0;

/*States
  0: FSW Start
  1: Calibration waiting
  2: Calibrated and waiting for launch
  3: Launch
  4: Deploy from the rocket
  5: Released from container
  6: Landed and waiting for pickup
*/

unsigned int state = 0;

void setup() {


  Serial.begin(9600);

  xBee.begin(9600);
  xBee.println(F("xBee Started"));

  Wire.begin();
  
  myservo.attach(6);
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  acc_sensor.initialize();
  xBee.println(acc_sensor.testConnection() ? "MPU6050 success" : "MPU6050 failed");
  xBee.println(F("BME is starting..."));
  
  while (!bme.begin())
  {
    delay(500);
    xBee.println(F("BME Error."));
  }
  
  
  if (!SD.begin(4)) {
    while (1);
    xBee.println(F("SDCard Error."));
  }


  // If there is a file named power_1.txt in the SD card, a power failure has occurred.
  file = SD.open(F("power_1.txt"), FILE_READ);
  if(file)
  {
    xBee.println(F("Detect power failure."));
    // Read records
    file = SD.open("power_1.txt");
    if (file) {
      Serial.println(F("power_1.txt:"));
  
      // read from the file until there's nothing else in it:
      while (file.available()) {
        
      }
      // close the file:
      file.close();
    } else {
      // if the file didn't open, print an error:
      Serial.println(F("error opening test.txt"));
    }
    state = 2;
  }
  else
  {
    power_failure = 0;
    xBee.println(F("Normal start")); 
    file.close();  
  }

   // A file named power_1.txt is being created after reading the power failure status. 
   // This file will be deleted when the Stop command is received. 
   // If it does not, the file will maintain its presence and will be the next power outage.
   file = SD.open(F("power_1.txt"), FILE_WRITE);
   file.print("power_failure");
   file.close();
   
  xBee.println(F("Initializing success."));
  delay(500);
}


void loop() 
{
    tone(8,4000,100);
    
    DateTime now = rtc.now();
    DateTime future (now);
  
    if(state < 3)
    {
      xBee.listen();
      
      if(state == 0)
      {
        xBee.println(F("Payload is waiting for to start."));
      }
      else if(state == 1)
      {
        xBee.println(F("Calibration waiting"));  
      }
      else if(state == 2)
      {
        xBee.println(F("Calibrated and waiting for launch"));  
      }
      
      if(state != 2)
      {
        while (xBee.available() > 0) 
        {
          char cmd = xBee.read();
          if(cmd == 's')
          {
            xBee.println(F("FSW started."));  
            state = 1;
          }
          else if(cmd == 'c')
          {
            
            // When the calibration command is sent, the log file is deleted to write new data from scratch.
            SD.remove("log.txt");
            // When the calibration command is received, the power failure file is deleted or when the stop command is received
            SD.remove("power_1.txt");
            xBee.println(F("Calibration is success"));
            state = 2;
          }else if(cmd == 'r'){
            myservo.write(0);
            delay(500);
            myservo.write(180);
          }
        }  
      }  
    }
  
    // Receiving a calibrate command.
    // Receiving a start command.
    // After receiving a start command, calibration cannot be performed.

    // When isStart = 1, it starts sending data.
    if (state > 1) 
    {    
      
      // Waiting for 1 second.
   
      acc_sensor.getMotion6(NULL, NULL, NULL, &gx, &gy, &gz); // reading acceleration and gyro values

      float temp(NAN), hum(NAN), pres(NAN);
  
      BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
      BME280::PresUnit presUnit(BME280::PresUnit_Pa);
      EnvironmentCalculations::AltitudeUnit envAltUnit  =  EnvironmentCalculations::AltitudeUnit_Meters;
      EnvironmentCalculations::TempUnit     envTempUnit =  EnvironmentCalculations::TempUnit_Celsius;
    
    
      bme.read(pres, temp, hum, tempUnit, presUnit);
      float altitude = EnvironmentCalculations::Altitude(pres, envAltUnit, referencePressure, outdoorTemp, envTempUnit);
      /*States
        0: FSW Start
        1: Calibration
        2: Calibrated and waiting for launch
        3: Launch
        4: Deploy from the rocket
        5: Released from container
        6: Landed and waiting for pickup
      */
      if(altitude > 0 && !climb)
      {
        state = 3;
        climb = true;
      }
      else if(altitude < 450 && climb)
      {
        state = 4;
      }
      else if (altitude < 420 && (!descent ))
      {
        descent = true;
        state = 5;
        climb = false;
        //Camera init
        myservo.write(0);
        delay(500);
        myservo.write(180);

      }else if( altitude < 20 && descent){
        state = 6;
        descent = false;
      }

        //<TEAM ID>,<MISSION TIME>,<PACKET COUNT>,<ALTITUDE>, <PRESSURE>, <TEMP>,<VOLTAGE>,<GPS TIME>,<GPS LATITUDE>,<GPS LONGITUDE>
        //,<GPS ALTITUDE>,<GPS SATS>,<PITCH>,<ROLL>,<BLADE SPIN RATE>,<SOFTWARE STATE>,<BONUS DIRECTION>
        float voltage = (analogRead(A6)) * (6.5 / 1023.0);
        
        sum += gy;
       
        
        smartDelay(10);
        
      if(state > 3)
      {
        xBee.print(F("["));
        xBee.print(F("2505"));
        xBee.print(F(","));
        xBee.print(future.hour(), DEC);
        xBee.print(F(":"));
        xBee.print(future.minute(), DEC);
        xBee.print(F(":"));
        xBee.print(future.second(), DEC);
        xBee.print(F(","));
        xBee.print(pckCnt / 100);
        xBee.print(F(","));
        xBee.print(altitude);
        xBee.print(F(","));
        xBee.print(pres);
        xBee.print(F(","));
        xBee.print(temp);
        xBee.print(F(","));
        xBee.print(voltage);
        xBee.print(F(","));
        xBee.print(gps.location.lat());
        xBee.print(F(","));
        xBee.print(gps.location.lng());
        xBee.print(F(","));
        xBee.print(gps.altitude.meters());
        xBee.print(F(","));
        xBee.print(gx, DEC);//PITCH
        xBee.print(F(","));
        xBee.print(gy, DEC);//Roll
        xBee.print(F(","));
        xBee.print(gz, DEC);//Yaw
        xBee.print(F(","));
        xBee.print(state);
        xBee.print(F(","));
        xBee.print(sum % 360, DEC);//Roll count
        xBee.println(F("]"));
    
        file = SD.open(F("log.txt"), FILE_WRITE);
    
        if (file) {
          file.print(F("2505"));
          file.print(F(","));
          file.print(future.hour(), DEC);
          file.print(F(":"));
          file.print(future.minute(), DEC);
          file.print(F(":"));
          file.print(future.second(), DEC);
          file.print(F(","));
          file.print(pckCnt);
          file.print(F(","));
          file.print(altitude);
          file.print(F(","));
          file.print(pres);
          file.print(F(","));
          file.print(temp);
          file.print(F(","));
          file.print(voltage);
          file.print(F(","));
          file.print(gps.location.lat());
          file.print(F(","));
          file.print(gps.location.lng());
          file.print(F(","));
          file.print(gps.altitude.meters());
          file.print(F(","));
          xBee.print(gx, DEC);//PITCH
          xBee.print(F(","));
          xBee.print(gy, DEC);//Roll
          xBee.print(F(","));
          xBee.print(gz, DEC);//Yaw
          xBee.print(F(","));
          xBee.print(state);
          xBee.print(F(","));
          xBee.print(sum % 360, DEC);//Roll count
          file.println();
    
        } else {
          // if the file didn't open, print an error:
        }
    
        file.close();
      }
      pckCnt++;
      }
    

} 

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (Serial.available())
      gps.encode(Serial.read());
  } while (millis() - start < ms);
}
