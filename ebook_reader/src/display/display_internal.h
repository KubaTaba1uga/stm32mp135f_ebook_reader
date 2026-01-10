#ifndef EBOOK_READER_DISPLAY_INTERNAL_H
#define EBOOK_READER_DISPLAY_INTERNAL_H
#include <display_driver.h>

#include "utils/error.h"

typedef struct ebk_DisplayModule *ebk_display_module_t;

struct ebk_DisplayModule {
  ebk_error_t (*init)(ebk_display_module_t module);
  ebk_error_t (*destroy)(ebk_display_module_t module);
};

struct ebk_Display {
  dd_display_driver_t dd;
  ebk_display_module_t display_module;
};

#endif // EBOOK_READER_DISPLAY_INTERNAL_H
