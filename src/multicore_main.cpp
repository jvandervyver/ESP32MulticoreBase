#include "multicore_main.h"
#include "Arduino.h"

static unsigned int count = 0;

void core0_setup() {
}

void core1_setup() {
}

void core0_loop() {
  synchronized(
    ++count;

    Serial.print("Count from core0: ");
    Serial.println(count);
  );
}

void core1_loop() {
  synchronized(
    ++count;
  );
}
