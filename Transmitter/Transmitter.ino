
#include <TinyGPS++.h> // Include the TinyGPS++ library
#include <SoftwareSerial.h> // Include SoftwareSerial to allow communication between RX and TX Pins.
#include <RH_ASK.h> // Driver Library to send data.
#include <SPI.h> // Needed to compile the code.

RH_ASK driver; // Initialize the driver

static const int RXPin = 4, TXPin = 3; // Initialize RX and TX variables on pins 4 and 3.
static const uint32_t GPSBaud = 9600; // Set GPS baud rate to 9600.

TinyGPSPlus gps; // Declare gps object.
SoftwareSerial ss(RXPin, TXPin); // Setup SoftwareSerial on Rx and Tx pins.

void setup() {
  Serial.begin(9600); // Open the serial port and set the data rate to 9600BPS
  ss.begin(GPSBaud);
  Serial.println(F("Drone Location"));

  if(!driver.init()){ // Check if transmitter driver is initialized.
    Serial.println(F("Init Failed"));
  }
  
}



void displaySendGPSInfo()
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid()) // If the GPS Location is valid
  {
    Serial.print(gps.location.lat(), 6); // Print Latitude with 6 digits after decimal
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6); //Print Longitude with 6 digits after decimal
    Serial.println();
    
    // Convert the Latitude and Longitude to Strings to be able to send them via the transmitter driver.

    String strLat = String(gps.location.lat(), 6);
    String strLng = String(gps.location.lng(), 6);

Serial.println("Speed \t" + String(gps.speed.kmph()) + " kmph\n\n");

    String loc = strLat + "|" + strLng; // Concatenate the Latitude and the Longitude with a special delimiter character |
    // So we can split the data in the receiver.
    
    driver.send((uint8_t*)loc.c_str(), loc.length()); // Convert the string into a uint8_t* (unsigned char*);
    // Blocks until the transmitter is no longer transmitting
    driver.waitPacketSent();
    delay(1000);
  }
  else
  {// If the GPS Location is not valid, the print INVALID.
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid()) // If the GPS date is valid, then print the date(month, day, year);
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  { // Print INVALID if the date is invalid
    Serial.print(F("INVALID"));
  }
  Serial.print(F(" "));
  if (gps.time.isValid()) // If GPS time is valid, then print the time(hours, minutes, seconds, centiseconds);
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  { // Print INVALID if the GPS time is invalid.
    Serial.print(F("INVALID"));
  }
  Serial.println();
}




void loop()
{
  while (ss.available() > 0)
    if (gps.encode(ss.read()))
      displaySendGPSInfo();

  if (millis() > 5000 && gps.charsProcessed() < 10) // If 5 seconds has passed and we don't have characters from gps
  { // Then there is a wiring error.
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }
}
