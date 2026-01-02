#ifndef EBOOK_READER_TIME_H
#define EBOOK_READER_TIME_H

#include "utils/error.h"
#include <stdint.h>
#include <time.h>

static inline cdk_error_t time_sleep_ms(uint32_t ms) {
  int err = nanosleep(
      &(struct timespec){
          .tv_nsec = (ms % 1000) * 1000000,
          .tv_sec = ms / 1000,
      },
      NULL);

  if (err) {
    return cdk_errnoi(EINVAL);
  }

  return 0;
}

#endif // EBOOK_READER_MEM_H
