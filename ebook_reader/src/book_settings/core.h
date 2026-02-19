#ifndef EBOOK_READER_BOOK_SETTINGS_CORE_H
#define EBOOK_READER_BOOK_SETTINGS_CORE_H
#include <stdbool.h>

#include "utils/err.h"
#include "utils/lvgl.h"

/**
   Book settings view
*/
typedef lvgl_obj_t wdgt_settings_t;

struct BookSettingsView {
  void (*click_set_scale_cb)(void *data);
  void (*click_exit_cb)(void *data);
  void (*click_set_x_off_cb)(void *data);
  void (*click_set_y_off_cb)(void *data);
  wdgt_settings_t settings;
  void *cb_data;
};

err_t book_settings_view_init(struct BookSettingsView *view,
                              void (*click_set_scale_cb)(void *data),
                              void (*click_back_cb)(void *data),
                              void (*click_set_x_off_cb)(void *data),
                              void (*click_set_y_off_cb)(void *data),
                              void *data);
void book_settings_view_destroy(struct BookSettingsView *view);

/**
   Set scale view
*/
typedef lvgl_obj_t wdgt_set_scale_t;

struct BookSettingsSetScaleView {
  void (*inc_scale_cb)(void *data);
  void (*dec_scale_cb)(void *data);
  void (*back_cb)(void *data);
  wdgt_set_scale_t set_scale;
  void *cb_data;
};

err_t book_settings_set_scale_view_init(struct BookSettingsSetScaleView *view,
                                        double scale,
                                        void (*inc_scale_cb)(void *data),
                                        void (*dec_scale_cb)(void *data),
                                        void (*back_cb)(void *data),
                                        void *cb_data);
void book_settings_set_scale_view_destroy(
    struct BookSettingsSetScaleView *view);
void book_settings_set_scale_view_set_scale(
    struct BookSettingsSetScaleView *view, double scale);

/**
   Set X offset view
*/
typedef lvgl_obj_t wdgt_set_x_off_t;

struct BookSettingsSetXOffView {
  void (*inc_x_off_cb)(void *data);
  void (*dec_x_off_cb)(void *data);
  void (*back_cb)(void *data);
  wdgt_set_x_off_t set_x_off;
  void *cb_data;
};

err_t book_settings_set_x_off_view_init(struct BookSettingsSetXOffView *view,
                                        int x_off,
                                        void (*inc_x_off_cb)(void *data),
                                        void (*dec_x_off_cb)(void *data),
                                        void (*back_cb)(void *data),
                                        void *cb_data);
void book_settings_set_x_off_view_destroy(struct BookSettingsSetXOffView *view);
void book_settings_set_x_off_view_set_value(
    struct BookSettingsSetXOffView *view, int x_off);

/**
   Set Y offset view
*/
typedef lvgl_obj_t wdgt_set_y_off_t;

struct BookSettingsSetYOffView {
  void (*inc_y_off_cb)(void *data);
  void (*dec_y_off_cb)(void *data);
  void (*back_cb)(void *data);
  wdgt_set_y_off_t set_y_off;
  void *cb_data;
};

err_t book_settings_set_y_off_view_init(struct BookSettingsSetYOffView *view,
                                        int y_off,
                                        void (*inc_y_off_cb)(void *data),
                                        void (*dec_y_off_cb)(void *data),
                                        void (*back_cb)(void *data),
                                        void *cb_data);
void book_settings_set_y_off_view_destroy(struct BookSettingsSetYOffView *view);
void book_settings_set_y_off_view_set_value(
    struct BookSettingsSetYOffView *view, int y_off);

/**
   Widgets
*/
err_t wdgt_settings_init(wdgt_settings_t *out,
                         void (*set_scale_cb)(lvgl_event_t),
                         void (*back_cb)(lvgl_event_t),
                         void (*set_x_off_cb)(lvgl_event_t),
                         void (*set_y_off_cb)(lvgl_event_t), void *event_data);

void wdgt_settings_destroy(wdgt_settings_t *out);

err_t wdgt_set_scale_init(wdgt_set_scale_t *out, double scale,
                          void (*event_cb)(lvgl_event_t), void *event_data);
void wdgt_set_scale_destroy(wdgt_set_scale_t *out);
void wdgt_set_scale_value(wdgt_set_scale_t scale, double value);

err_t wdgt_set_x_off_init(wdgt_set_x_off_t *out, int x_off,
                          void (*event_cb)(lvgl_event_t), void *event_data);
void wdgt_set_x_off_destroy(wdgt_set_x_off_t *out);
void wdgt_set_x_off_value(wdgt_set_x_off_t x_off, int value);
err_t wdgt_set_y_off_init(wdgt_set_y_off_t *out, int y_off,
                          void (*event_cb)(lvgl_event_t), void *event_data);
void wdgt_set_y_off_destroy(wdgt_set_y_off_t *out);
void wdgt_set_y_off_value(wdgt_set_y_off_t y_off, int value);

#endif // EBOOK_READER_BOOK_SETTINGS_CORE_H
