#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/rtc_wdt.h"
#include "esp_int_wdt.h"
#include "esp_task_wdt.h"
#include "main.h"
#include "multicore_main.h"
#include "Arduino.h"

// Do not modify anything here, instead make use of multicore_main.cpp

// This sets up the ESP32 to run two independent setup() and loop() functions
// per CPU: core0_setup(), core1_setup(), core0_loop(), core1_loop()
// Those can be modified in multicore_main.cpp

#define TASK_NAME(core_number) "core ## core_number ## task"
#define TASK_HANDLE(core_number) core ## core_number ## _task

#define CREATE_TASK(core_number) \
  xTaskCreatePinnedToCore([](void* _) TASK_CODE(core_number), TASK_NAME(core_number), STACK_BYTE_SIZE, NULL, 100, &TASK_HANDLE(core_number), core_number); \
  delay(TASK_CREATE_DELAY_MS)

#define TASK_CODE(core_number) { \
    core ## core_number ## _setup(); \
    while(1) { \
      core ## core_number ## _loop(); \
    } \
  }

static SemaphoreHandle_t synchronized_semaphore;
static TaskHandle_t core0_task;
static TaskHandle_t core1_task;

void setup() {
  // Semaphore for synchronized block
  synchronized_semaphore = xSemaphoreCreateMutex();

  // Disable all the watchdogs.  The watchdogs kill performance
  // and resets the ESP32 at the drop of a hat
  rtc_wdt_protect_off();
  rtc_wdt_disable();
  disableCore0WDT();
  disableCore1WDT();
  disableLoopWDT();

  // Create the setup and loop for core 0
  CREATE_TASK(0);

  // Create the setup and loop for core 1
  CREATE_TASK(1);

  // Completely delete the loop/main thread
  vTaskDelete(NULL);
}

void synchronized_semaphore_lock() {
  xSemaphoreTake(synchronized_semaphore, portMAX_DELAY);
}

void synchronized_semaphore_unlock() {
  xSemaphoreGive(synchronized_semaphore);
}

// This will never get executed, this function is just here to allow linking to succeed
// Putting anything here is pointless
void loop() {
}
