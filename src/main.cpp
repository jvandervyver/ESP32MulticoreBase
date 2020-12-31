#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/rtc_wdt.h"
#include "esp_int_wdt.h"
#include "esp_task_wdt.h"
#include "Arduino.h"
#include "main.h"
#include "multicore_main.h"

// This sets up the ESP32 to run two independent setup() and loop() functions
// per CPU: core0_setup(), core1_setup(), core0_loop(), core1_loop()
// Those can be modified in multicore_main.cpp
//
// This file focuses on the setup to make those functions act as independent
// arduino apps with shared memory and a locking mechanism:
// synchronized(
//   printf("I can only run on one core at a time");
//   delay(5);
// )

#define TASK_NAME(core_number) "core ## core_number ## task"
#define TASK_HANDLE(core_number) core ## core_number ## _task
#define CREATE_TASK(core_number) \
  xTaskCreatePinnedToCore([](void* _) TASK_CODE(core_number), TASK_NAME(core_number), STACK_BYTE_SIZE, NULL, 100, &TASK_HANDLE(core_number), core_number);
#define TASK_CODE(core_number) { \
  core ## core_number ## _setup(); \
  while(1) { \
    core ## core_number ## _loop(); \
  } \
}

static SemaphoreHandle_t synchronize_semaphore;
static TaskHandle_t core0_task;
static TaskHandle_t core1_task;

void setup() {
  // Disable all the watchdogs.  The watchdogs kill performance
  // and resets the ESP32 at the drop of a hat
  rtc_wdt_protect_off();
  rtc_wdt_disable();
  disableCore0WDT();
  disableCore1WDT();
  disableLoopWDT();

  // Semaphore for synchronized block
  synchronize_semaphore = xSemaphoreCreateMutex();

  // Create the setup and loop for core 0
  CREATE_TASK(0);
  delay(1000);

  // Create the setup and loop for core 1
  CREATE_TASK(1);
  delay(1000);

  // Completely delete the loop/main thread
  vTaskDelete(NULL);
}

void semaphore_lock() {
  xSemaphoreTake(synchronize_semaphore, portMAX_DELAY);
}

void semaphore_unlock() {
  xSemaphoreGive(synchronize_semaphore);
}

void loop() {
}