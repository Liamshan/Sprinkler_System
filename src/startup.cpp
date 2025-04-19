#include "startup.h"
#include "config.h"
#include <Arduino.h>

void startup_sequence() {
  digitalWrite(RELAY_ZONE1, HIGH);
  digitalWrite(RELAY_ZONE2, HIGH);
  delay(500);

  Serial.println("Zone 1 ON");
  digitalWrite(RELAY_ZONE1, LOW);
  delay(3000);
  Serial.println("Zone 1 OFF");
  digitalWrite(RELAY_ZONE1, HIGH);

  Serial.println("Zone 2 ON");
  digitalWrite(RELAY_ZONE2, LOW);
  delay(3000);
  Serial.println("Zone 2 OFF");
  digitalWrite(RELAY_ZONE2, HIGH);
}
