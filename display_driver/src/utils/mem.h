#ifndef DD_MEM_H
#define DD_MEM_H

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

static inline void *dd_malloc(size_t size) {
  void *mem = malloc(size);
  if (!mem) {
    puts("ERROR: No memory");
    _Exit(ENOMEM);
  }

  return mem;
}

static inline void dd_free(void *mem) { free(mem); }

#endif
