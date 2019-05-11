#include <Wire.h>  // Wire library - I2C iletişim için kullanılır.
/*  
 *   I2C Comm. (Wire.h)
 *   
 *   SDA --> A4
 *   SCL --> A4
 *   GND --> GND
 *   VCC --> 5V
*/

int ADXL345 = 0x53; // The ADXL345 sensor I2C address
float X_out, Y_out, Z_out;  // Çıkış değişkenleri

void setup() {
  Serial.begin(9600); // Serial port ekranında sonuçları yazdırmak için seri iletişim başlatmak
  Wire.begin(); // Initiate the Wire library
  // Set ADXL345 in measuring mode
  Wire.beginTransmission(ADXL345); // Start communicating with the device 
  Wire.write(0x2D); // Access/ talk to POWER_CTL Register - 0x2D
  // Enable measurement
  Wire.write(8); // (8dec -> 0000 1000 binary) Bit D3 High for measuring enable 
  Wire.endTransmission();
  delay(500);
}
void loop() 
{
  printADXLData();  
}


 void printADXLData()
  {
    Wire.beginTransmission(ADXL345);
    Wire.write(0x32); //  register 0x32 (ACCEL_XOUT_H) ile başlar
    Wire.endTransmission(false);
    Wire.requestFrom(ADXL345, 10, true); // Toplam 6 register oku, her eksen değeri 2 register'da saklanır
    // 10 register yazdım
    
    X_out = ( Wire.read()| Wire.read() << 8); // X-axis
    X_out = X_out/256; // + -2g aralığı için ham değerleri veri sayfasına göre 256'ya bölmemiz gerekir
    Y_out = ( Wire.read()| Wire.read() << 8); // Y-axis
    Y_out = Y_out/256;
    Z_out = ( Wire.read()| Wire.read() << 8); // Z-axis
    Z_out = Z_out/256;
    Serial.print(X_out);
    Serial.print(",");
    Serial.print(Y_out);
    Serial.print(",");
    Serial.println(Z_out);
    delay(50);
   }
