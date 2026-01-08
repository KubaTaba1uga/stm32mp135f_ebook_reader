#ifndef EBOOK_READER_MEM_H
#define EBOOK_READER_MEM_H

#include <stdlib.h>

void *ebk_mem_malloc(size_t size);
void ebk_mem_free(void *mem);

#endif // EBOOK_READER_MEM_H
