#include "library/library.h"
#include "menu/core.h"
#include "utils/err.h"

err_t menu_view_init(struct MenuView *view, books_list_t books,
                     void (*book_cb)(book_t, void *), void *data) {
  err_o = wdgt_bar_init(&view->bar);
  ERR_TRY(err_o);

  if (!books) {
    goto out;
  }

  err_o = wdgt_books_init(&view->books, books, book_cb, data);
  ERR_TRY_CATCH(err_o, error_bar_cleanup);

out:
  return 0;

error_bar_cleanup:
  wdgt_bar_destroy(&view->bar);
error_out:
  return err_o;
}

void menu_view_destroy(struct MenuView *view) {
  wdgt_bar_destroy(&view->bar);
  wdgt_books_destroy(&view->books);
  }
