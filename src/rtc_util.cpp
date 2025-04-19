#include "rtc_util.h"
#include "config.h"
#include <Wire.h>

RTC_DS3231 rtc;

void writeLastWateringTime(uint32_t epoch) {
  Wire.beginTransmission(0x57);
  Wire.write(SRAM_ADDR);
  Wire.write((epoch >> 24) & 0xFF);
  Wire.write((epoch >> 16) & 0xFF);
  Wire.write((epoch >> 8) & 0xFF);
  Wire.write(epoch & 0xFF);
  Wire.endTransmission();
}

uint32_t readLastWateringTime() {
  Wire.beginTransmission(0x57);
  Wire.write(SRAM_ADDR);
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
