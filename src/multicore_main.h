#ifndef __multicore__main_h__
#define __multicore__main_h__
#ifdef __cplusplus
extern "C" {
#endif

  #include "main.h"
  #include "Arduino.h"

  void core0_setup();
  void core1_setup();

  void core0_loop();
  void core1_loop();

#ifdef __cplusplus
}
#endif
#endif