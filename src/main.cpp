#include <Arduino.h>
#include <Wire.h>
#include <Ticker.h>
#include "RTClib.h"

#define RELAY_ZONE1 25  // IN1 on relay module
#define RELAY_ZONE2 26  // IN2 on relay module
RTC_DS3231 rtc;

// SRAM address to store 4-byte last watering timestamp
#define SRAM_ADDR 0x08

// Global Variables

Ticker zone1Timer;
Ticker zone2Timer;
Ticker stopWateringTimer;

bool hasWateredToday = false;
bool isWatering = false;

//Function Prototypes
void stop_zone1();
void start_zone2();
void stop_zone2();
void startup_sequence();
void error_handling();
void water_function(const int hour, const int minute);
void writeLastWateringTime(uint32_t epoch);
uint32_t readLastWateringTime();


void setup() {
  //Start Serial Bus & I2C
  Serial.begin(115200);
  Wire.begin();  // SDA = GPIO21, SCL = GPIO22 (defaults)

  error_handling();

  //Set Pin modes
  pinMode(RELAY_ZONE1, OUTPUT);
  pinMode(RELAY_ZONE2, OUTPUT);

  //Run Startup Sequence
  startup_sequence();
}

void loop() {
  water_function(6,30);  //Pass in the time e.g. (6,30) for 6:30am
  delay(1000 * 10); // Check every 10 seconds
}


//---Functions---///
//---Startup---///
void startup_sequence() {
    // Turn both relays off initially (LOW = off)
  digitalWrite(RELAY_ZONE1, HIGH);
  digitalWrite(RELAY_ZONE2, HIGH);

  Serial.println("Starting test sequence...");

  // Turn on zone 1
  Serial.println("Zone 1 ON");
  digitalWrite(RELAY_ZONE1, LOW);
  delay(30000);  // 30 seconds

  // Turn off zone 1
  Serial.println("Zone 1 OFF");
  digitalWrite(RELAY_ZONE1, HIGH);
  delay(10000);  // 10 seconds

  // Turn on zone 2
  Serial.println("Zone 2 ON");
  digitalWrite(RELAY_ZONE2, LOW);
  delay(30000);  // 30 seconds

  // Turn off zone 2
  Serial.println("Zone 2 OFF");
  digitalWrite(RELAY_ZONE2, HIGH);
}

void error_handling() {

  //RTC Error Handlers
  if (!rtc.begin()) {
    Serial.println("Couldnt find RTC");
    while (1)
      ;
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power. Setting time to compile time.");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

//---Core Logic---///
void water_function(const int hour, const int minute) {
  DateTime now = rtc.now();

  // Show current time in Serial Monitor
  Serial.printf("Time: %02d:%02d:%02d | Day: %d | Epoch: %lu\n", now.hour(), now.minute(), now.second(), now.day(), now.unixtime());


  // Only check if current time is within watering window (e.g., 6:30 AM)
  if (now.hour() == hour && now.minute() == minute && !isWatering) {

    // Step 1: Get last watering time from RTC SRAM
    uint32_t lastWateringEpoch = readLastWateringTime();

    // Step 2: Compute time since last watering
    uint32_t currentEpoch = now.unixtime();
    uint32_t secondsSinceLast = currentEpoch - lastWateringEpoch;
    Serial.printf("Seconds since last watering: %lu\n", secondsSinceLast);

    //Step 3: Water if 4 or more days have passed
    if (secondsSinceLast >= 345600) { // 4 days = 345600 seconds
      Serial.println("It's been 4 days - starting watering sequence!");

      // Update last watering time
      writeLastWateringTime(currentEpoch);

      // Begin watering Zone 1 - same logic you already had
      digitalWrite(RELAY_ZONE1, LOW);  // Turn on Zone 1
      isWatering = true;

      zone1Timer.once(15 * 60, stop_zone1);  //Stop zone 1 in 15 minutes
    }
  }
}

void stop_zone1() {
  Serial.println("Stopping Zone 1, pausing before Zone 2...");
  digitalWrite(RELAY_ZONE1, HIGH);
  zone2Timer.once(5, start_zone2);  //5 second delay
}

void start_zone2() {
  Serial.println("Starting Zone 2");
  digitalWrite(RELAY_ZONE2, LOW);
  stopWateringTimer.once(15 * 60, stop_zone2); // Stop zone 2 in 15 minutes
}

void stop_zone2() {
  Serial.println("Watering complete. Shutting off Zone 2.");
  digitalWrite(RELAY_ZONE2, HIGH);
  isWatering = false;
}


//---RTC SRAM Utilities---//

void writeLastWateringTime(uint32_t epoch) {
  Wire.beginTransmission(0x57); // DS3231 I2C address
  Wire.write(SRAM_ADDR);             // Starting SRAM address

  // Write 4 bytes (big endian)
  Wire.write((epoch >> 24) & 0xFF);
  Wire.write((epoch >> 16) & 0xFF);
  Wire.write((epoch >> 8) & 0xFF);
  Wire.write(epoch & 0xFF);
  Wire.endTransmission();
}

uint32_t readLastWateringTime() {
  Wire.beginTransmission(0x57);
  Wire.write(0x08); // Starting address
  Wire.endTransmission();

  Wire.requestFrom(0x57, 4);
  uint32_t epoch = 0;
  if (Wire.available() == 4) {
    epoch |= (Wire.read() << 24);
    epoch |= (Wire.read() << 16);
    epoch |= (Wire.read() << 8);
    epoch |= Wire.read();
  }

  return epoch;
}

