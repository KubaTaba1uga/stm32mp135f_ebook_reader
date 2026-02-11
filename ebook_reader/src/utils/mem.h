#ifndef MEM_H
#define MEM_H

#include <stddef.h>
#include <stdlib.h>

#define mem_container_of(ptr, type, member)                                    \
  ((type *)((char *)(ptr) - offsetof(type, member)))

void *mem_malloc(size_t size);
void mem_free(void *mem);

typedef void *ref_t;
ref_t mem_refalloc(size_t size, void (*destroy)(ref_t));
ref_t mem_ref(ref_t ref);
ref_t mem_deref(ref_t ref);

#endif // MEM_H
