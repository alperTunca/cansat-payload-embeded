// (c) Michael Schoeffler 2016, http://www.mschoeffler.de
#include <SD.h> //Load SD library
int chipSelect = 4; //chip select pin for the MicroSD Card Adapter
File file; // file object that is used to read and write data
void setup() {
  Serial.begin(9600); // start serial connection to print out debug messages and data
  
  pinMode(chipSelect, OUTPUT); // chip select pin must be set to OUTPUT mode
  if (!SD.begin(chipSelect)) { // Initialize SD card
    Serial.println("Could not initialize SD card."); // if return value is false, something went wrong.
  }
  
  if (SD.exists("file.txt")) { // if "file.txt" exists, fill will be deleted
    Serial.println("File exists.");
    if (SD.remove("file.txt") == true) {
      Serial.println("Successfully removed file.");
    } else {
      Serial.println("Could not removed file.");
    }
  }
}
void loop() {
  file = SD.open("file.txt", FILE_WRITE); // open "file.txt" to write data
  if (file) {
    int number = random(10); // generate random number between 0 and 9
    file.println(number); // write number to file
    file.close(); // close file
    Serial.print("Wrote number: "); // debug output: show written number in serial monitor
    Serial.println(number);
  } else {
    Serial.println("Could not open file (writing).");
  }
  
  file = SD.open("file.txt", FILE_READ); // open "file.txt" to read data
  if (file) {
    Serial.println("--- Reading start ---");
    char character;
    while ((character = file.read()) != -1) { // this while loop reads data stored in "file.txt" and prints it to serial monitor
      Serial.print(character);
    }
    file.close();
    Serial.println("--- Reading end ---");
  } else {
    Serial.println("Could not open file (reading).");
  }
  delay(5000); // wait for 5000ms
}
