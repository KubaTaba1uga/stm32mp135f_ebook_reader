#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdint.h>
#include <time.h>

#include "utils/error.h"

cdk_error_t time_sleep_ms(uint32_t ms) {
  int err = nanosleep(
      &(struct timespec){
          .tv_nsec = (ms % 1000) * 1000000,
          .tv_sec = ms / 1000,
      },
      NULL);

  if (err == -1) {
    return cdk_errnos(errno, "Cannot sleep");
  }

  return 0;
}

 uint32_t time_now(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (uint32_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}
