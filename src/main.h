#ifndef __main___header__
#define __main___header__
#ifdef __cplusplus
extern "C" {
#endif

  #define STACK_BYTE_SIZE       (1024 * 8)  // The size of the each thread stack specified as the number of bytes
  #define TASK_CREATE_DELAY_MS  500         // After creating a task how many milliseconds to delay before doing something else

  // Code that should execute on a single core alone:
  // synchronized(
  //   some_variable++;  // Increment volatile integer
  //   some_ref = NULL;  // Update a volatile pointer
  // );
  #define synchronized(__volatile_code_synchronized_across_cores) \
    do { \
      synchronized_semaphore_lock(); \
      __volatile_code_synchronized_across_cores \
      synchronized_semaphore_unlock(); \
    } while(0)

  // Do not use these functions directly to avoid the possiblity of forgetting to unlock
  // Instead use synchronized above
  void synchronized_semaphore_lock();
  void synchronized_semaphore_unlock();

#ifdef __cplusplus
}
#endif
#endif