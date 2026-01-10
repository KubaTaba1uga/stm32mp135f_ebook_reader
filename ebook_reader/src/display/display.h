#ifndef EBOOK_READER_DISPLAY_H
#define EBOOK_READER_DISPLAY_H
#include <display_driver.h>

#include "utils/error.h"
#include "utils/settings.h"

typedef struct ebk_Display *ebk_display_t;

ebk_error_t ebk_display_init(ebk_display_t *, enum ebk_DisplayModelEnum);
ebk_error_t ebk_display_show_boot_img(ebk_display_t);
void ebk_display_destroy(ebk_display_t *);

#endif // EBOOK_READER_DISPLAY_H
