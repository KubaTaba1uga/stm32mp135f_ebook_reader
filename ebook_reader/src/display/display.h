#ifndef EBOOK_READER_DISPLAY_H
#define EBOOK_READER_DISPLAY_H

#include "utils/err.h"

typedef struct Display *display_t;

err_t display_init(display_t *out);
void display_destroy(display_t *out);

#endif // EBOOK_READER_DISPLAY_H
