# cansat-payload-embeded
2019 CanSat Competition - Payload Electronic Circuit Arduino Code (For Ardunio Nano)

ADXL345, BME280 and RTC use I2C for communication.
SD Card Adapter use SPI for communication.  
Servo Engine, RGB Led and Buzzer requires digital signal to work. 

Pin assignments for each sensor: (Each sensor uses common GND.)  

* **ADXL345**
  + SDA --> A4
  + SCL --> A5
        
* **BME280**
  + SDA --> A4    
  + SCL --> A5  
        
* **RTC** 
  + SDA --> A4  
  + SCL --> A5  
        
* **Buzzer**
  + Digital --> D2   
        
* **Servo En.**
  + Digital --> D7   

* **RGB Led**
  + Red --> D3  
  + Green --> D6  
  + Blue --> D9 
  
* **SD Card A.**
  + CS --> D4
  + SCK --> D13
  + MOSI --> D11
  + MISO --> D12
