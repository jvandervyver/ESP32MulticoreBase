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

  #define TASK_LAMBDA(__function_code__) [](void* _) { __function_code__ }
  #define CREATE_TASK(__name__, __task_handle__, __core_id__, __function_code__) \
    do { \
      xTaskCreatePinnedToCore(TASK_LAMBDA(__function_code__), __name__, STACK_BYTE_SIZE, NULL, 100, &__task_handle__, __core_id__); \
      delay(500); \
    } while(0)

  // Create the setup and loop for core 0
  CREATE_TASK("core0", core0_task, 0, {
    core0_setup();

    while(1) {
      core0_loop();
    }
  });

  // Create the setup and loop for core 1
  CREATE_TASK("core1", core1_task, 1, {
    core1_setup();

    while(1) {
      core1_loop();
    }
  });

  // Wait a bit then completely delete the loop/main thread
  delay(1000);
  vTaskDelete(NULL);
}

void _synchronize_semaphore_lock() {
  xSemaphoreTake(synchronize_semaphore, portMAX_DELAY);
}

void _synchronize_semaphore_unlock() {
  xSemaphoreGive(synchronize_semaphore);
}

void loop() {
}