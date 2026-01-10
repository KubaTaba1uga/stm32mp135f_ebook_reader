#ifndef EBOOK_READER_DISPLAY_INTERNAL_H
#define EBOOK_READER_DISPLAY_INTERNAL_H
#include <display_driver.h>

#include "utils/error.h"
#include "utils/settings.h"

typedef struct ebk_DisplayModule *ebk_display_module_t;

struct ebk_DisplayModule {
  ebk_error_t (*show_boot_img)(ebk_display_module_t module);
  void (*destroy)(ebk_display_module_t module);
  void *private;
};

struct ebk_Display {
  enum ebk_DisplayModelEnum model;
  struct ebk_DisplayModule display;
};

const char *ebk_display_mdump(enum ebk_DisplayModelEnum model);

#endif // EBOOK_READER_DISPLAY_INTERNAL_H
