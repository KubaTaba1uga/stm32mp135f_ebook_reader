#ifndef EBOOK_READER_MENU_CORE_H
#define EBOOK_READER_MENU_CORE_H
#include <stdbool.h>

#include "library/library.h"
#include "utils/err.h"
#include "utils/lvgl.h"

typedef lvgl_obj_t wdgt_bar_t;
typedef lvgl_obj_t wdgt_books_t;

struct MenuView {
  wdgt_bar_t bar;
  wdgt_books_t books;
  books_list_t books_data;
};

err_t menu_view_init(struct MenuView *view, books_list_t books,
                     void (*book_cb)(book_t, void *), void *data);
void menu_view_destroy(struct MenuView *view);

err_t wdgt_bar_init(wdgt_bar_t *out);
void wdgt_bar_destroy(wdgt_bar_t *out);
err_t wdgt_books_init(wdgt_books_t *out, books_list_t books,
                      void (*event_cb)(book_t, void *), void *event_data);
void wdgt_books_destroy(wdgt_books_t *out);

#endif // EBOOK_READER_MENU_CORE_H
