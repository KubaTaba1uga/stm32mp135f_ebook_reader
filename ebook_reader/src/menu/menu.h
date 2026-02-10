#ifndef EBOOK_READER_MENU_H
#define EBOOK_READER_MENU_H

#include "utils/err.h"
#include "library/library.h"

typedef struct Menu *menu_t;

err_t menu_init(menu_t *out, library_t lib);
void menu_destroy(menu_t *out);

#endif // EBOOK_READER_MENU_H
