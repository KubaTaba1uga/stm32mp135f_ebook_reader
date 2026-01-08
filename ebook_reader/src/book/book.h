#ifndef EBOOK_READER_BOOK_H
#define EBOOK_READER_BOOK_H

#include "utils/error.h"

struct ebk_Book {
  const char *title;
  const char *file_path;
  const int creation_time;
};

typedef struct ebk_Menu *ebk_menu_t;

ebk_error_t ebk_menu_init(ebk_menu_t out);
void ebk_menu_destroy(ebk_menu_t out);

#endif // EBOOK_READER_CORE_MENU
