#include <Arduino.h>
#include <Wire.h>
#include <Ticker.h>
#include "RTClib.h"

#define RELAY_ZONE1 25  // IN1 on relay module
#define RELAY_ZONE2 26  // IN2 on relay module
RTC_DS3231 rtc;

// Global Variables
bool hasWateredToday = false;
Ticker stopWateringTimer;
bool isWatering = false;

//Function Definitions
void startup_sequence();
void error_handling();
void water_function(const int hour, const int minute);

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

  // Water at 6:30 AM
  water_function(6,30);  //Pass in the time e.g. (6,30) for 6:30am

  delay(1000 * 10); // Check every 10 seconds
}


//---Functions---///
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

void water_function(const int hour, const int minute) {
  DateTime now = rtc.now();

  // Show current time in Serial Monitor
  Serial.printf("Time: %02d:%02d:%02d | Day: %d\n", now.hour(), now.minute(), now.second(), now.day());

  //Water at 6:30 AM
  if (now.hour() == hour && now.minute() == minute && !hasWateredToday) {   //parameterize this as function inputs passed
    Serial.println("Watering time!");

    if (!isWatering) {
      isWatering = true;
      hasWateredToday = true;

      if (now.day() % 2 == 0) {
        Serial.println("Even day -> Zone 1 ON");
        digitalWrite(RELAY_ZONE1, LOW);
      } else {
        Serial.println("Odd day -> Zone 2 ON");
        digitalWrite(RELAY_ZONE2, LOW);
      }

      // Start non-blocking 20 minute timer
      stopWateringTimer.once(20 * 60, stop_watering); // 20 minutes
    }
    
  }

  // Reset daily flag after 7:00 AM
  if (now.hour() == 7 && hasWateredToday) {
    hasWateredToday = false;
    Serial.println("Resetting daily flag");
  }
}

void stop_watering() {
  Serial.println("Watering conplete - turning off zones");
  digitalWrite(RELAY_ZONE1, HIGH);
  digitalWrite(RELAY_ZONE2, HIGH);
  isWatering = false;
}

