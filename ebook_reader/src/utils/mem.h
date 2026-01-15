#ifndef MEM_H
#define MEM_H

#include <stdlib.h>

#define mem_container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

void *mem_malloc(size_t size);
void mem_free(void *mem);

#endif // MEM_H
