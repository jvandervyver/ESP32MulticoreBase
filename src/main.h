#ifndef __main___header__
#define __main___header__
#ifdef __cplusplus
extern "C" {
#endif

  // The size of the each thread stack specified as the number of bytes
  #define STACK_BYTE_SIZE  (1024 * 8)

  // Do not use these functions directly to avoid the possiblity of forgetting to unlock
  void _synchronize_semaphore_lock();
  void _synchronize_semaphore_unlock();

  #define synchronized(__code_to_be_synchronized_across_cores__) \
    do { \
      _synchronize_semaphore_lock(); \
      do { \
        __code_to_be_synchronized_across_cores__ \
      } while(0); \
      _synchronize_semaphore_unlock(); \
    } while(0)

#ifdef __cplusplus
}
#endif
#endif