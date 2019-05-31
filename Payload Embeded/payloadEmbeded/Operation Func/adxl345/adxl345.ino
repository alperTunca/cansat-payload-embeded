#include <Wire.h>  // Wire library for I2C.
/*  
 *   I2C Comm. (Wire.h)
 *   
 *   SDA --> A4
 *   SCL --> A4
 *   GND --> GND
 *   VCC --> 5V
*/

int ADXL345 = 0x53; // The ADXL345 sensor I2C address
float X_out, Y_out, Z_out;  // Output Variables

void setup() 
{
  Serial.begin(9600); // Start serial communication to print results on serial port screen
  Wire.begin(); // Initiate the Wire library
  // Set ADXL345 in measuring mode
  Wire.beginTransmission(ADXL345); // Start communicating with the device 
  Wire.write(0x2D); // Access/ talk to POWER_CTL Register - 0x2D
  // Enable measurement
  Wire.write(8); // (8dec -> 0000 1000 binary) Bit D3 High for measuring enable 
  Wire.endTransmission();
  
}
void loop() 
{
  printADXLData();  
  delay(1000);
}


 void printADXLData()
  {
    Wire.beginTransmission(ADXL345);
    Wire.write(0x32); //  register 0x32 (ACCEL_XOUT_H) ile başlar
    Wire.endTransmission(false);
    Wire.requestFrom(ADXL345, 6, true); // Total 6 register read, each axis value is stored in 2 registers
    
    X_out = ( Wire.read()| Wire.read() << 8); // X-axis
    X_out = X_out/256; // We need to divide the raw values for the + -2g range by 256 according to the data sheet
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
