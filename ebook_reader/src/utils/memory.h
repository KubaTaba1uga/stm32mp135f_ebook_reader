#ifndef EBOOK_READER_MEM_H
#define EBOOK_READER_MEM_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils/log.h"

static inline void *mem_malloc(size_t size) {
  void *mem = malloc(size);
  if (!mem) {
    log_warn("No memory");
    exit(ENOMEM);
  }

  return mem;
}

static inline void mem_free(void *mem) { free(mem); }

#endif // EBOOK_READER_MEM_H
