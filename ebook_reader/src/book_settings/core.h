#ifndef EBOOK_READER_BOOK_SETTINGS_CORE_H
#define EBOOK_READER_BOOK_SETTINGS_CORE_H
#include <stdbool.h>

#include "library/library.h"
#include "utils/err.h"
#include "utils/lvgl.h"

typedef lvgl_obj_t wdgt_settings_t;
typedef lvgl_obj_t wdgt_set_scale_t;

struct BookSettingsView {
  void (*click_set_scale_cb)(void *data);
  wdgt_settings_t settings;
  void *cb_data;
};

err_t book_settings_view_init(struct BookSettingsView *view,
                              void (*click_set_scale_cb)(void *data),
                              void *data);
void book_settings_view_destroy(struct BookSettingsView *view);

struct BookSettingsSetScaleView {
  void (*inc_scale_cb)(void *data);
  void (*dec_scale_cb)(void *data);
  wdgt_set_scale_t set_scale;
  void *cb_data;
};

err_t book_settings_set_scale_view_init(struct BookSettingsSetScaleView *view,
                                        double scale,
                                        void (*inc_scale_cb)(void *data),
                                        void (*dec_scale_cb)(void *data),
                                        void *cb_data);
void book_settings_set_scale_view_destroy(
    struct BookSettingsSetScaleView *view);
void book_settings_set_scale_view_set_scale(
    struct BookSettingsSetScaleView *view, double scale);

err_t wdgt_settings_init(wdgt_settings_t *out,
                         void (*set_scale_cb)(lvgl_event_t), void *event_data);
void wdgt_settings_destroy(wdgt_settings_t *out);

err_t wdgt_set_scale_init(wdgt_set_scale_t *out, double scale,
                          void (*event_cb)(lvgl_event_t), void *event_data);
void wdgt_set_scale_destroy(wdgt_set_scale_t *out);

void wdgt_set_scale_value(wdgt_set_scale_t scale, double value);

#endif // EBOOK_READER_BOOK_SETTINGS_CORE_H
