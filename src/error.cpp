#include "error.h"
#include "rtc_util.h"
#include <Arduino.h>

void error_handling() {
  if (!rtc.begin()) {
    Serial.println("RTC not found!");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting to compile time.");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}
