/*
   * I2C Commm. (Wire.h)
   * 
   * Vin -->  3.3V
   * Gnd -->  Gnd
   * SDA -->  A4 
   * SCL -->  A5 
   * 
   */
  
  #include <EnvironmentCalculations.h>
  #include <BME280I2C.h>
  #include <Wire.h>
  
  #define SERIAL_BAUD 9600
  
  // Default environmental values:
  float referencePressure = 1018.6;  // hPa local QFF (official meteor station reading)
  float outdoorTemp = 4.7;           //° C measured local outside temperature.
  float barometerAltitude = 1650.3;  //meter ... map readings + barometer location
  
  
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
  
  BME280I2C bme(settings);
  
  void setup()
  {
    Serial.begin(SERIAL_BAUD);
    while(!Serial) {} // Waiting
    Wire.begin();
    while(!bme.begin())
    {
      Serial.println("BME280 sensörü bulunamadı!");
      delay(1000);
    }
  }
  
  void loop()
  {
     printBME280Data(&Serial);
     delay(500);
  }
  
  void printBME280Data
  (
     Stream* client
  )
  {
     float temp(NAN);   // Temperature
     float hum(NAN);    // Moisture
     float pres(NAN);   // Pressure
  
     BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
     BME280::PresUnit presUnit(BME280::PresUnit_hPa);
  
     bme.read(pres, temp, hum, tempUnit, presUnit);
  
     client->print("Sıcaklık: ");
     client->print(temp);
     client->print("°"+ String(tempUnit == BME280::TempUnit_Celsius ? "C" :"F"));
     
     client->print("\t\tBasınç: ");
     client->print(pres);
     client->print(String(presUnit == BME280::PresUnit_hPa ? "hPa" : "Pa")); // expected hPa and Pa only
  
     EnvironmentCalculations::AltitudeUnit envAltUnit  =  EnvironmentCalculations::AltitudeUnit_Meters;
     EnvironmentCalculations::TempUnit     envTempUnit =  EnvironmentCalculations::TempUnit_Celsius;
 
     float altitude = EnvironmentCalculations::Altitude(pres, envAltUnit, referencePressure, outdoorTemp, envTempUnit);

  
     client->print("\t\tRakım: ");
     client->print(altitude);
     client->print((envAltUnit == EnvironmentCalculations::AltitudeUnit_Meters ? "m" : "ft"));
    client->print("\n");
     delay(1000);
  }
