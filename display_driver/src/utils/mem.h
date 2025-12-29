#ifndef DISPLAY_DRIVER_MEM_H
#define DISPLAY_DRIVER_MEM_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

static inline void *dd_malloc(size_t size) {
  void *mem = malloc(size);
  if (!mem) {
    puts("ERROR: No memory");
    exit(ENOMEM);
  }

  return mem;
}

static inline void dd_free(void *mem) { free(mem); }

#endif // DISPLAY_DRIVER_MEM_H
