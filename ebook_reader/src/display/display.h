#ifndef EBOOK_READER_DISPLAY_H
#define EBOOK_READER_DISPLAY_H
#include <display_driver.h>

#include "utils/error.h"

typedef struct ebk_Display *ebk_display_t;

ebk_error_t ebk_display_init(ebk_display_t *out, enum dd_DisplayDriverEnum model);
void ebk_display_destroy(ebk_display_t *out);

#endif // EBOOK_READER_DISPLAY_H
