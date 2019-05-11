  /*
   * I2C Commm. (Wire.h)
   * 
   * Vin -->  3.3V
   * Gnd -->  Gnd
   * SDA -->  A4 
   * SCK -->  A5 
   * 
   */
  
  #include <EnvironmentCalculations.h>
  #include <BME280I2C.h>
  #include <Wire.h>
  
  #define SERIAL_BAUD 9600
  
  // Varsayılan çevresel değerler:
  float referencePressure = 1018.6;  // hPa yerel QFF (resmi meteor istasyonu okuması)
  float outdoorTemp = 4.7;           // ° C ölçülen yerel dış hava sıcaklığı.
  float barometerAltitude = 1650.3;  // metre ... harita okumaları + barometre konumu
  
  
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
    while(!Serial) {} // Bekleniyor
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
     float temp(NAN);   // Sıcaklık
     float hum(NAN);    // Nem
     float pres(NAN);   // Basınç
  
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
