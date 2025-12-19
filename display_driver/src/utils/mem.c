#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void *mem_alloc(size_t size) {
  void *mem = malloc(size);
  if (!mem) {
    puts("ERROR: No memory");
    _exit(ENOMEM);
  }

  return mem;
}

void mem_free(void *mem) {
  free(mem);

  }
