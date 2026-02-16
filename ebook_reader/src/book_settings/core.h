#ifndef EBOOK_READER_BOOK_SETTINGS_CORE_H
#define EBOOK_READER_BOOK_SETTINGS_CORE_H
#include <stdbool.h>

#include "library/library.h"
#include "utils/err.h"
#include "utils/lvgl.h"

typedef lvgl_obj_t wdgt_settings_t;
typedef lvgl_obj_t wdgt_set_scale_t;

struct BookSettingsView {
  void (*update_cb)(book_t book, void *data);
  wdgt_set_scale_t set_scale;
  wdgt_settings_t settings;
  void *update_data;
  book_t book;
};

err_t book_settings_view_init(struct BookSettingsView *view, book_t book,
                              void (*book_update_cb)(book_t book, void *data),
                              void *data);
void book_settings_view_destroy(struct BookSettingsView *view);

err_t wdgt_settings_init(wdgt_settings_t *out);
void wdgt_settings_destroy(wdgt_settings_t *out);
err_t wdgt_set_scale_init(wdgt_set_scale_t *out, wdgt_settings_t parent,
                          double scale, void (*event_cb)(lvgl_event_t),
                          void *event_data);
void wdgt_set_scale_destroy(wdgt_set_scale_t *out);

#endif // EBOOK_READER_BOOK_SETTINGS_CORE_H
