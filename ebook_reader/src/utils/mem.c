#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "mem.h"
#include "utils/log.h"

void *mem_malloc(size_t size) {
  void *mem = malloc(size);
  if (!mem) {
    puts("ERROR: No memory");
    exit(ENOMEM);
  }

  return mem;
}

void mem_free(void *mem) { free(mem); }

struct Reference {
  int ref_count;
  void (*destroy)(ref_t);
  uint8_t bytes[];
};

ref_t mem_refalloc(size_t size, void (*destroy)(ref_t)) {
  struct Reference *ref;
  ref = mem_malloc(sizeof(struct Reference) + size);
  ref->ref_count = 1;
  ref->destroy = destroy;

  return ref->bytes;
};

ref_t mem_ref(ref_t data) {
  if (!data) {
    return NULL;
  }

  struct Reference *ref = mem_container_of(data, struct Reference, bytes);
  ref->ref_count++;

  log_debug("Ref count: %p=%d", ref->bytes, ref->ref_count);
  
  return data;
}

ref_t mem_deref(ref_t data) {
  if (!data) {
    return NULL;
  }

  struct Reference *ref = mem_container_of(data, struct Reference, bytes);
  if (--ref->ref_count == 0) {
    if (ref->destroy) {
      ref->destroy(data);
    }
    log_debug("Ref count: %p=%d", ref->bytes, ref->ref_count);    
    mem_free(ref);
    return NULL;
  };

  log_debug("Ref count: %p=%d", ref->bytes, ref->ref_count);
  
  return data;
};
