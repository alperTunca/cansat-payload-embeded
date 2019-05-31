#include <EEPROM.h>
#include <Wire.h>

int ADXL345 = 0x53; // The ADXL345 sensor I2C address
float X_out, Y_out, Z_out;  // Output variables

int address = 0;
byte value;

int packetCount=0;

void setup() 
{

  Serial.begin(9600); // Start serial communication to print results on serial port screen
  Wire.begin(); // Initiate the Wire libraryS
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
  
  Serial.print("-----> Read X:");
  Serial.print(div256(EEPROM.read(0)));

  Serial.print("-----> Read Y:");
  Serial.print(div256(EEPROM.read(1)));

  Serial.print("-----> Read Z:");
  Serial.print(div256(EEPROM.read(2)));
  
  Serial.print(" packetCount: ");
  Serial.println(packetCount);
  packetCount++;
  delay(300);
}

  double div256(int data)
  {
    while(data < 0)
    {
      data = data+256;
    }
    return data/(double)256;
  }

  void printADXLData()
  {
    Wire.beginTransmission(ADXL345);
    Wire.write(0x32); //  register starts with 0x32 (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(ADXL345, 6, true); // Toplam 6 kayıt okunur, her eksen değeri 2 kayıtta saklanır
    
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
    Serial.print(Z_out);
    Serial.println(";");
    delay(50);
   }
   
   void eepromADXL(int adress, int x_out, int y_out, int z_out)
   {
    int tempX=x_out*256;
    int tempY=y_out*256;
    int tempZ=z_out*256;
    EEPROM.write(adress,tempX);
    EEPROM.write(adress+1,tempY); 
    EEPROM.write(adress+2,tempZ);
   }

   void eepromBME(int adress, int altitude, int pressure, int temperature)
   {
    int tempTemp = temperature*100;
    int tempAlt = altitude*100;
    int tempPress = pressure*100; 
   
    EEPROM.write(adress,temp/100);
    EEPROM.write(adress+1,temp-((temp/100)*100));
   }
