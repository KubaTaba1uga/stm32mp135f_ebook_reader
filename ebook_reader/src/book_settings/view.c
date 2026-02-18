#include <lvgl.h>
#include <stdio.h>

#include "book_settings/core.h"
#include "core/lv_group.h"
#include "utils/err.h"

static void scale_click_event_cb(lvgl_event_t);
static void scale_change_event_cb(lvgl_event_t);
static void x_off_click_event_cb(lvgl_event_t);
static void exit_click_event_cb(lvgl_event_t);
static void x_off_change_event_cb(lvgl_event_t e);

err_t book_settings_view_init(struct BookSettingsView *view,
                              void (*click_set_scale_cb)(void *data),
                              void (*click_back_cb)(void *data),
			      void (*click_set_x_off_cb)(void *data),                              
                              void *data) {
  *view = (struct BookSettingsView){
      .click_set_scale_cb = click_set_scale_cb,
      .click_exit_cb = click_back_cb,
      .click_set_x_off_cb = click_set_x_off_cb,
      .cb_data = data,
  };

  err_o = wdgt_settings_init(&view->settings, scale_click_event_cb,
                             exit_click_event_cb,x_off_click_event_cb, view);
  ERR_TRY(err_o);

  return 0;

error_out:
  return err_o;
}

void book_settings_view_destroy(struct BookSettingsView *view) {
  wdgt_settings_destroy(&view->settings);
};

err_t book_settings_set_scale_view_init(struct BookSettingsSetScaleView *view,
                                        double scale,
                                        void (*inc_scale_cb)(void *data),
                                        void (*dec_scale_cb)(void *data),
                                        void (*back_cb)(void *data),
                                        void *cb_data) {
  *view = (struct BookSettingsSetScaleView){
      .inc_scale_cb = inc_scale_cb,
      .dec_scale_cb = dec_scale_cb,
      .back_cb = back_cb,
      .cb_data = cb_data,
  };

  err_o =
      wdgt_set_scale_init(&view->set_scale, scale, scale_change_event_cb, view);
  ERR_TRY(err_o);

  return 0;

error_out:
  return err_o;
}

void book_settings_set_scale_view_destroy(
    struct BookSettingsSetScaleView *view) {
  wdgt_set_scale_destroy(&view->set_scale);
}

static void scale_click_event_cb(lvgl_event_t e) {

  struct BookSettingsView *view = lv_event_get_user_data(e);
  view->click_set_scale_cb(view->cb_data);
};

static void x_off_click_event_cb(lvgl_event_t e){
  struct BookSettingsView *view = lv_event_get_user_data(e);
  view->click_set_x_off_cb(view->cb_data);
}

static void exit_click_event_cb(lvgl_event_t e) {
  puts(__func__);
  struct BookSettingsView *view = lv_event_get_user_data(e);
  view->click_exit_cb(view->cb_data);
};


static void scale_change_event_cb(lvgl_event_t e) {

  struct BookSettingsSetScaleView *view = lv_event_get_user_data(e);
  lv_key_t key = lv_event_get_key(e);
  if (key == LV_KEY_UP) {
    view->inc_scale_cb(view->cb_data);
  }
  if (key == LV_KEY_DOWN) {
    view->dec_scale_cb(view->cb_data);
  }
  if (key == LV_KEY_ENTER || key == '\n' || key == '\r' || key == LV_KEY_ESC) {
    view->back_cb(view->cb_data);
  }
}

void book_settings_set_scale_view_set_scale(
    struct BookSettingsSetScaleView *view, double scale) {
  wdgt_set_scale_value(view->set_scale, scale);
}

err_t book_settings_set_x_off_view_init(struct BookSettingsSetXOffView *view,
                                        int x_off,
                                        void (*inc_x_off_cb)(void *data),
                                        void (*dec_x_off_cb)(void *data),
                                        void (*back_cb)(void *data),
                                        void *cb_data) {
  *view = (struct BookSettingsSetXOffView) {
      .inc_x_off_cb = inc_x_off_cb,
      .dec_x_off_cb = dec_x_off_cb,
      .back_cb = back_cb,
      .cb_data = cb_data,
  };

  err_o =
      wdgt_set_x_off_init(&view->set_x_off, x_off, x_off_change_event_cb, view);
  ERR_TRY(err_o);

  return 0;

error_out:
  return err_o;
}

void book_settings_set_x_off_view_destroy(
    struct BookSettingsSetXOffView *view) {
  wdgt_set_x_off_destroy(&view->set_x_off);
}

static void x_off_change_event_cb(lvgl_event_t e) {
  puts(__func__);
  struct BookSettingsSetXOffView *view = lv_event_get_user_data(e);
  lv_key_t key = lv_event_get_key(e);
  if (key == LV_KEY_RIGHT) {
    view->inc_x_off_cb(view->cb_data);
  }
  if (key == LV_KEY_LEFT) {
    view->dec_x_off_cb(view->cb_data);
  }
  if (key == LV_KEY_ENTER || key == '\n' || key == '\r' || key == LV_KEY_ESC) {
    view->back_cb(view->cb_data);
  }
}

void book_settings_set_x_off_view_set_x_off(
    struct BookSettingsSetXOffView *view, int x_off) {
  wdgt_set_x_off_value(view->set_x_off, x_off);
}
