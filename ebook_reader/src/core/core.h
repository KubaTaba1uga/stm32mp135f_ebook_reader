#ifndef EBOOK_READER_CORE
#define EBOOK_READER_CORE

#include "utils/error.h"

typedef struct ebk_Core *ebk_core_t;

ebk_error_t ebk_core_init(ebk_core_t *out);

ebk_error_t ebk_core_main(ebk_core_t core);

void ebk_core_destroy(ebk_core_t *out);

#endif // EBOOK_READER_CORE
