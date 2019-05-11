  /*
      Arduino UNO-Nano için
      ADXL 345 - I2C
      BME280- I2C
      SD Card - SPI
      RTC DS3231 - I2C
      SCL --> A5
      SDA --> A4
  */
  
  #include <EnvironmentCalculations.h>
  #include <BME280I2C.h>
  #include <Wire.h> // Wire library - I2C iletişim için kullanılır.
  #include <RTClib.h>
  #include <SD.h> //Yükle SD kütüphanesi
  #include <Servo.h> // Servo motor çalışması için
  
  #define SERIAL_BAUD 19200
  #define TEAM_ID 2505
  
  using namespace EnvironmentCalculations;
  
  int packetCount = 0;
  
  RTC_DS3231 rtc;// RTC verisi tutulur
  DateTime now; //
  
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
  BME280I2C bme(settings); //bme280 verisi tutulur.
  
  // RGB Led pinleri ve parlaklık
  int redPin = 9;
  int greenPin = 10;
  int bluePin = 11;
  
  Servo servoEngine;  // Bir servoyu kontrol etmek için servo nesnesi oluşturmak
  int servoPin = 6; //arduino uno için seçili dijital pin seçilmeli
  int pos = 0;    // servo konumunu saklamak için değişken
  int dest=40; // gitmesi gereken değer
  
  int ADXL345 = 0x53; // The ADXL345 sensor I2C address
  float X_out, Y_out, Z_out;  // Çıkış değişkenleri
  float destX=135.0;
  
  void setup()
  {
    Serial.begin(SERIAL_BAUD);
    while (!Serial) {} // Bekleniyor
    Wire.begin(); // I2C haberleşme için
    Wire.beginTransmission(ADXL345); // Start communicating with the device
    Wire.write(0x2D); // Access/ talk to POWER_CTL Register - 0x2D
    // Enable measurement
    Wire.write(8); // (8dec -> 0000 1000 binary) Bit D3 High for measuring enable
    Wire.endTransmission();
  
    rtc.begin(); // RTC için
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
    // RGB Led için pin atamaları
    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT);
  
    servoEngine.attach(servoPin);
  }
  
  void loop()
  {
    // Önceki veriyi tutup karsılastırma yapılacak yer
  
    now = rtc.now();
    if (!rtc.begin())
    {
      selectColor(255, 0, 0);
      Serial.println("RTC modülü bulunamadı!");
    }
    if (!bme.begin())
    {
      selectColor(255, 0, 0);
      Serial.println("BME280 sensörü bulunamadı!");
    }
    else
    {
      selectColor(0, 255, 0);
      Serial.print("{");
      Serial.print(TEAM_ID);      //Team id bastırılan yer                          <TEAM_ID>
      Serial.print(packetCount);  //Team id bastırılan yer                          <PACKET COUNT>
      printRTCData();             //RTC değerleri bastırılan yer                    <MISSION_TIME>
                                  //İşlemci sıfırlama durumunda iletilecek bilgi    <PACKET COUNT>
      printBME280Data(&Serial);   //BME280 üzerinden alınan yükseklik verisi        <ALTITUDE>
                                  //BME280 üzerinden alınan basınç verisi           <PRESSURE>
                                  //BME280 üzerinden alınan sıcaklık verisi(C)      <TEMP>
                                  //CanSat güç veriyolunun voltajıdır               <VOLTAGE>
      printADXLData();            //ADXL345 üzerinden alınan pitch bilgisi          <PITCH>
                                  //ADXL345 üzerinden alınan roll bilgisi           <ROLL>
                                  //ADXL345 üzerinden alınan yaw bilgisi            <YAW>
                                  //Gömülü'de koşullu olarak üretilen değerdir      <SOFTWARE STATE>    Ne oldugunu sor
      Serial.print(";");
      Serial.println();
      packetCount++;
      delay(500);

      if (X_out > destX);
      {
        moveServo(pos,dest); 
      }
      
      
      
    }
  
  }
  
  void printADXLData()
  {
    // 400 m gibi yüksek değer için register durumunu oku
    //X --> 0.15 m 0.05'e denk gelir.
    Wire.beginTransmission(ADXL345);
    Wire.write(0x32); //  register 0x32 (ACCEL_XOUT_H) ile başlar
    Wire.endTransmission(false);
    Wire.requestFrom(ADXL345, 6, true); // Toplam 10 register oku, her eksen değeri 2 register'da saklanır
    X_out = ( Wire.read() | Wire.read() << 8); // X-axis
    X_out = X_out / 256; // + -2g aralığı için ham değerleri veri sayfasına göre 256'ya bölmemiz gerekir
    Y_out = ( Wire.read() | Wire.read() << 8); // Y-axis
    Y_out = Y_out / 256;
    Z_out = ( Wire.read() | Wire.read() << 8); // Z-axis
    Z_out = Z_out / 256;
    Serial.print(X_out);
    Serial.print(",");
    Serial.print(Y_out);
    Serial.print(",");
    Serial.print(Z_out);
    delay(50);
  }
  
  void moveServo(int pos, int destination)
  {
    if (pos > destination)
    {
      for (int i = pos; i < destination; i++)
      {
        servoEngine.write(pos);
      }
      delay(50);
    }
    else if (pos < destination)
    {
      for (int i = pos; i > destination; i++)
      {
        servoEngine.write(pos);
      }
      delay(50);
    }
    else
      Serial.print("pos, destination ile aynı değere sahip");
  
  }
  
  void selectColor(int R, int G, int B)
  {
    // Brigthness(Parlaklık) 0-255
    // 0-0-0 Beyaz
    analogWrite(redPin, R);
    analogWrite(greenPin, G);
    analogWrite(bluePin, B);
    delay(50);
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
    float temp(NAN);   // Sıcaklık
    float hum(NAN);    // Nem
    float pres(NAN);   // Basınç
  
    BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
    BME280::PresUnit presUnit(BME280::PresUnit_hPa);
    bme.read(pres, temp, hum, tempUnit, presUnit);
  
    AltitudeUnit envAltUnit  =  AltitudeUnit_Meters;
    TempUnit     envTempUnit =  TempUnit_Celsius;
  
    float altitude = Altitude(pres, envAltUnit, referencePressure, outdoorTemp, envTempUnit);
    client->print(altitude);
    client->print(",");
    //client->print((envAltUnit == AltitudeUnit_Meters ? "m" : "ft"));
    client->print(temp);
    client->print(",");
    //client->print("°" + String(tempUnit == BME280::TempUnit_Celsius ? "C" : "F"));
    client->print(pres);
    client->print(",");
    //client->print(String(presUnit == BME280::PresUnit_hPa ? "hPa" : "Pa")); // expected hPa and Pa only
    delay(50);
  }
