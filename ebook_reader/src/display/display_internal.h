#ifndef EBOOK_READER_DISPLAY_INTERNAL_H
#define EBOOK_READER_DISPLAY_INTERNAL_H
#include "utils/error.h"

typedef struct ebk_DisplayModule *ebk_display_module_t;

struct ebk_DisplayModule {
  ebk_error_t (*init)(ebk_display_module_t module);
  ebk_error_t (*destroy)(ebk_display_module_t module);
};

#endif // EBOOK_READER_DISPLAY_INTERNAL_H
