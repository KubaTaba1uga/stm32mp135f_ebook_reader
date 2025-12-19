#ifndef DISP_DRVER_MEM_H
#define DISP_DRVER_MEM_H

#include <stddef.h>

void * mem_alloc(size_t size);
void mem_free(void *mem);

#endif // DISP_DRVER_MEM_H
