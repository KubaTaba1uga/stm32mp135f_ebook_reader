#ifndef EBOOK_READER_CORE_MENU
#define EBOOK_READER_CORE_MENU

#include "utils/error.h"

struct cbk_Menu {};

ebk_error_t ebk_core_init(ebk_core_t *out);
void ebk_core_destroy(ebk_core_t *out);

#endif // EBOOK_READER_CORE_MENU
