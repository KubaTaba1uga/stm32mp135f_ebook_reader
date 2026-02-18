#include <lvgl.h>
#include <stdio.h>
#include <string.h>

#include "library/library.h"
#include "misc/lv_event.h"
#include "reader/core.h"
#include "utils/err.h"
#include "utils/log.h"
#include "utils/mem.h"

static void reader_page_event_cb(lv_event_t *e);

err_t reader_view_init(struct ReaderView *view, book_t book,
                       void (*next_page_cb)(void *),
                       void (*prev_page_cb)(void *), void (*menu_cb)(void *),
                       void (*book_settings_cb)(void *), void *data) {
  *view = (struct ReaderView){
      .book = mem_ref(book),
      .last_book =
          {
              .scale = book_get_scale(book),
              .x_off = book_get_x_off(book),
              .y_off = book_get_y_off(book),
              .page = book_get_page_no(book),
          },
      .next_page_cb = next_page_cb,
      .prev_page_cb = prev_page_cb,
      .menu_cb = menu_cb,
      .book_settings_cb = book_settings_cb,
      .cb_data = data,
  };

  const unsigned char *page_data;
  int page_size = 0;

  page_data =
      book_get_page(book, lv_display_get_horizontal_resolution(NULL),
                    lv_display_get_vertical_resolution(NULL), &page_size);
  ERR_TRY(err_o);

  err_o = wdgt_page_init(&view->page, page_data, page_size,
                         reader_page_event_cb, view);
  ERR_TRY(err_o);

  return 0;

error_out:
  mem_deref(book);
  return err_o;
};

void reader_view_destroy(struct ReaderView *view) {
  puts(__func__);
  if (view->page) {
    wdgt_page_destroy(&view->page);
  }

  if (view->book) {
    mem_deref(view->book);
    log_debug("Book destroyed");
  }

  *view = (struct ReaderView){0};
}

err_t reader_view_refresh(struct ReaderView *view) {
  struct ReaderViewBook book_new = {
      .scale = book_get_scale(view->book),
      .x_off = book_get_x_off(view->book),
      .y_off = book_get_y_off(view->book),
      .page = book_get_page_no(view->book),
  };

  if (memcmp(&view->last_book, &book_new, sizeof(struct ReaderViewBook)) == 0) {
    goto out;
  };

  const unsigned char *page_data;
  int page_size = 0;

  page_data =
      book_get_page(view->book, lv_display_get_horizontal_resolution(NULL),
                    lv_display_get_vertical_resolution(NULL), &page_size);
  ERR_TRY(err_o);

  wdgt_page_refresh(view->page, page_data, page_size);

  view->last_book = book_new;

out:
  return 0;

error_out:
  mem_deref(view->book);
  return err_o;
}

static void reader_page_event_cb(lv_event_t *e) {
  struct ReaderView *view = lv_event_get_user_data(e);
  lv_key_t key = lv_event_get_key(e);

  switch (key) {
  case LV_KEY_LEFT:
    view->prev_page_cb(view->cb_data);
    break;
  case LV_KEY_RIGHT:
    view->next_page_cb(view->cb_data);
    break;
  case LV_KEY_ESC:
    view->menu_cb(view->cb_data);
    break;
  default:
    if (key == '\n' || key == '\r' || key == LV_KEY_ENTER) {
      view->book_settings_cb(view->cb_data);
    }
  }
};
