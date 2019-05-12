# cansat-payload-embeded
2019 CanSat Competition - Payload Electronic Circuit Arduino Code (For Ardunio Nano)

ADXL345, BME280 and RTC use I2C for communication.  
SD Card Adapter use SPI for communication.  
RGB Led requires PWM signal to work.  
Servo Engine and Buzzer requires digital signal to work. 

Pin assignments for each sensor: (Each sensor uses common 5V and GND.)  

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
  + Digital --> D10   

* **RGB Led**
  + Red --> D6  
  + Green --> D9  
  + Blue --> D10 
  
* **SD Card A.**
  + CS --> D4  
  + MOSI --> D11
  + MISO --> D12
