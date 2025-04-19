#include <Arduino.h>
#include <Wire.h>
#include <Ticker.h>
#include "rtc_util.h"
#include "watering.h"
#include "startup.h"
#include "error.h"
#include "config.h"

// Tickers and globals
Ticker zone1Timer;
Ticker zone2Timer;
Ticker stopWateringTimer;

bool isWatering = false;

void setup() {
  pinMode(RELAY_ZONE1, OUTPUT);
  pinMode(RELAY_ZONE2, OUTPUT);
  digitalWrite(RELAY_ZONE1, HIGH);
  digitalWrite(RELAY_ZONE2, HIGH);

  Serial.begin(115200);
  delay(1000);
  Wire.begin();

  error_handling();
  startup_sequence();
}

void loop() {
  water_function(6, 30);
  delay(10000);
}
