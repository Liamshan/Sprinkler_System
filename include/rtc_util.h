#pragma once

#include <RTClib.h>

extern RTC_DS3231 rtc;

void writeLastWateringTime(uint32_t epoch);
uint32_t readLastWateringTime();
