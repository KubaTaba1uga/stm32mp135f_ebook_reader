#include "book_settings/core.h"
#include "core/lv_group.h"
#include "core/lv_obj_event.h"
#include "library/library.h"
#include "misc/lv_event.h"
#include "utils/err.h"
#include "utils/mem.h"
#include <stdio.h>

static void scale_view_event_cb(lvgl_event_t);

err_t book_settings_view_init(struct BookSettingsView *view, book_t book,
                              void (*update_cb)(book_t book, void *data),
                              void *data) {
  *view = (struct BookSettingsView){
      .book = mem_ref(book),
      .update_cb = update_cb,
      .update_data = data,
  };

  err_o = wdgt_settings_init(&view->settings);
  ERR_TRY(err_o);

  err_o = wdgt_set_scale_init(&view->set_scale, view->settings,
                              book_get_scale(book), scale_view_event_cb, data);
  ERR_TRY(err_o);

  return 0;

error_out:
  return err_o;
}

static void scale_view_event_cb(lvgl_event_t e) {
  puts(__func__);
  struct BookSettingsView *view = lv_event_get_user_data(e);
  lv_key_t key = lv_event_get_key(e);
  bool is_match = false;

  if (key == LV_KEY_LEFT) {
    double scale = book_get_scale(view->book);
    book_set_scale(view->book, scale - 0.25);
    is_match = true;
  }
  if (key == LV_KEY_RIGHT) {
    double scale = book_get_scale(view->book);
    book_set_scale(view->book, scale + 0.25);
    is_match = true;
  }

  if (is_match) {
    view->update_cb(view->book, view->update_data);
  }

  return;
};
