#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "mem.h"

void *mem_malloc(size_t size) {
  void *mem = malloc(size);
  if (!mem) {
    puts("ERROR: No memory");
    exit(ENOMEM);
  }

  return mem;
}

 void mem_free(void *mem) { free(mem); }
