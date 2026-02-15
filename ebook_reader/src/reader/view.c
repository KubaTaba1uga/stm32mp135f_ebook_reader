#include <lvgl.h>

#include "reader/core.h"
#include "utils/err.h"
#include "utils/mem.h"

err_t reader_view_init(struct ReaderView *view, book_t book,
                       void (*next_page_cb)(book_t, void *),
                       void (*prev_page_cb)(book_t, void *), void *data) {
  *view = (struct ReaderView){
    .book_data = mem_ref(book),
  };

  const unsigned char *page_data;
  int page_size = 0;

  page_data =
      book_get_page(book, lv_display_get_horizontal_resolution(NULL),
                    lv_display_get_vertical_resolution(NULL), &page_size);
  ERR_TRY(err_o);

  err_o = wdgt_page_init(&view->page, page_data, page_size);
  ERR_TRY(err_o);

  return 0;

error_out:
  mem_deref(book);
  return err_o;
};

void reader_view_destroy(struct ReaderView *view) {
  mem_deref(view->book_data);
}
