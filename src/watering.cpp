#include "watering.h"
#include "config.h"
#include <Arduino.h>
#include <Ticker.h>
#include "rtc_util.h"

extern bool isWatering;
extern Ticker zone1Timer;
extern Ticker zone2Timer;
extern Ticker stopWateringTimer;

void water_function(const int hour, const int minute) {
  DateTime now = rtc.now();
  uint32_t currentEpoch = now.unixtime();
  uint32_t lastWatered = readLastWateringTime();
  uint32_t nextWateringEpoch = lastWatered + WATER_INTERVAL_SECONDS;

  Serial.printf("Time: %02d:%02d:%02d | Next Watering In: %lu seconds\n",
    now.hour(), now.minute(), now.second(), nextWateringEpoch - currentEpoch);

  if (now.hour() == hour && now.minute() == minute && !isWatering) {
    if (lastWatered == 0xFFFFFFFF || lastWatered == 0) {
      Serial.println("First boot - skipping watering.");
      writeLastWateringTime(currentEpoch);
      return;
    }

    if ((currentEpoch - lastWatered) >= WATER_INTERVAL_SECONDS) {
      Serial.println("Starting watering sequence.");
      writeLastWateringTime(currentEpoch);

      digitalWrite(RELAY_ZONE1, LOW);
      isWatering = true;

      zone1Timer.once(10 * 60, stop_zone1);
    }
  }
}

void stop_zone1() {
  Serial.println("Stopping Zone 1");
  digitalWrite(RELAY_ZONE1, HIGH);
  zone2Timer.once(5, start_zone2);
}

void start_zone2() {
  Serial.println("Starting Zone 2");
  digitalWrite(RELAY_ZONE2, LOW);
  stopWateringTimer.once(10 * 60, stop_zone2);
}

void stop_zone2() {
  Serial.println("Stopping Zone 2");
  digitalWrite(RELAY_ZONE2, HIGH);
  isWatering = false;
}
