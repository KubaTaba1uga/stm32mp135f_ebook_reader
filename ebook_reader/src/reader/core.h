#ifndef EBOOK_READER_READER_CORE_H
#define EBOOK_READER_READER_CORE_H
#include <stdbool.h>

#include "library/library.h"
#include "utils/err.h"
#include "utils/lvgl.h"

typedef lvgl_obj_t wdgt_page_t;

struct ReaderViewBook {
  int scale;
  int x_off;
  int y_off;
  int page;
};

struct ReaderView {
  wdgt_page_t page;
  book_t book;
  struct ReaderViewBook last_book;
  void (*next_page_cb)(void *);
  void (*prev_page_cb)(void *);
  void (*book_settings_cb)(void *);
  void (*menu_cb)(void *);
  void *cb_data;
};

err_t reader_view_init(struct ReaderView *view, book_t book,
                       void (*next_page_cb)(void *),
                       void (*prev_page_cb)(void *), void (*menu_cb)(void *),
                       void (*book_settings_cb)(void *), void *data);
void reader_view_destroy(struct ReaderView *view);
err_t reader_view_refresh(struct ReaderView *view);

err_t wdgt_page_init(wdgt_page_t *out, const unsigned char *page_data,
                     int page_size, void (*cb)(lvgl_event_t), void *data);
void wdgt_page_destroy(wdgt_page_t *out);

/* err_t wdgt_books_init(wdgt_books_t *out, books_list_t books, */
/*                       void (*event_cb)( void *), void *event_data); */
/* void wdgt_books_destroy(wdgt_books_t *out); */

#endif // EBOOK_READER_READER_CORE_H
