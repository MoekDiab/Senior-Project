#include <LiquidCrystal_I2C.h> // Include the LiquidCrystal Library to support lcd

#include <RH_ASK.h> // Driver library to send and receive data

#include <SPI.h> // Needed to compile the code

RH_ASK driver; // Initalize driver to receive data

int minutes = 0, seconds = 0;
bool maxFlag = true; // Boolean flag to check if the timer is below the maximum time of 99:59
bool received = false;
LiquidCrystal_I2C lcd(0x27, 16, 2); // Declare lcd 16 columns, 2 rows
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // For debbugging
  if (!driver.init()) { // Check if driver is initialized
    Serial.println("Driver init error");
  }
  lcd.init(); // Initialize LCD
  lcd.backlight(); // Activate backlight on the lcd
  
}


void updateTimer() {
  if (received) {
    seconds++;
    if (seconds > 59) {
      if (minutes >= 99 && seconds > 59) {
        maxFlag = false; // Flip the flag to false if the timer is at maximum.
      }
      seconds = 0;
      minutes++;
    }

    if (maxFlag) { // If maxFlag is true, meaning we are below the maximum time.
      lcd.clear(); // Clear lcd and print the minutes and seconds.
      lcd.print(F("Time : "));
      if (minutes < 10) lcd.print(F("0"));
      lcd.print(minutes);
      lcd.print(F(":"));
      if (seconds < 10) lcd.print(F("0"));
      lcd.print(seconds);

    }
  }
}

void loop() { 
  uint8_t loc[19]; // Buffer of unsigned char* to hold  the Latitude and the Longitude of len 19

  uint8_t locLen = sizeof(loc); // Get the size of the buffer
  if (driver.recv(loc, & locLen)) {

    loc[locLen] = '\0'; // Null terminate the location buffer after reading
    String locStr = (const char * ) loc; // Cast the location buffer to const char* and convert it to an arduino String
    int delimIndex = locStr.indexOf('|'); // Get the index of the delimiter used to separate the Latitude and Longitude
    String latStr = locStr.substring(0, delimIndex); // Get the Latitude
    String lngStr = locStr.substring(delimIndex + 1, locStr.length()); // Get the Longitude

    Serial.println("Latituide and Longitude: " + latStr + "," + lngStr + "\n"); // Write the Lat and Lng to the Serial

    received = true;
  }

  delay(1000);
  // Minutes and seconds timer.
  updateTimer();
}
