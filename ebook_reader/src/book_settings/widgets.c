#include <lvgl.h>
#include <stdio.h>

#include "book_settings/core.h"
#include "core/lv_group.h"
#include "core/lv_obj.h"
#include "core/lv_obj_style_gen.h"
#include "core/lv_obj_tree.h"
#include "library/library.h"
#include "lv_api_map_v8.h"
#include "misc/lv_color.h"
#include "misc/lv_event.h"
#include "misc/lv_style.h"
#include "utils/err.h"
#include "utils/lvgl.h"
#include "utils/mem.h"

typedef lvgl_obj_t wdgt_settings_field_t;

struct WdgtBookSettings {
  struct {
    wdgt_settings_field_t set_scale_field;
    wdgt_settings_field_t set_x_off_field;
    lv_style_t style;
  } settings;

  void (*set_scale_cb)(lvgl_event_t);
  void *cb_data;
};

static lvgl_obj_t wdgt_set_hor_num_create(double hor_num);
static void wdgt_set_hor_num_set_value(lvgl_obj_t wdgt, double value);
err_t wdgt_settings_field_init(wdgt_set_scale_t *out, wdgt_settings_t parent,
                               const char *name, void (*event_cb)(lvgl_event_t),
                               void *event_data);
void wdgt_settings_field_destroy(wdgt_set_scale_t *out);

void dump_indev_groups(void) {
  lv_indev_t *indev = lv_indev_get_next(NULL);
  while (indev) {
    lv_indev_type_t t = lv_indev_get_type(indev);
    if (t == LV_INDEV_TYPE_KEYPAD || t == LV_INDEV_TYPE_ENCODER) {
      lv_group_t *g = lv_indev_get_group(indev);
      printf("indev=%p type=%d group=%p\n", (void *)indev, (int)t, (void *)g);
    }
    indev = lv_indev_get_next(indev);
  }
}

err_t wdgt_settings_init(wdgt_settings_t *out,
                         void (*set_scale_cb)(lvgl_event_t), void *event_data) {
  puts(__func__);
  const int setting_x = 480;
  const int setting_y = 800;

  dump_indev_groups();

  wdgt_settings_t settings = *out = lvgl_obj_create(lv_screen_active());

  struct WdgtBookSettings *priv = mem_malloc(sizeof(struct WdgtBookSettings));
  lv_obj_set_user_data(settings, priv);
  lv_gridnav_add(settings, LV_GRIDNAV_CTRL_NONE);

  lv_obj_set_pos(settings,
                 lv_display_get_horizontal_resolution(NULL) - setting_x,
                 lv_display_get_vertical_resolution(NULL) - setting_y);
  lv_obj_set_size(settings, setting_x, setting_y);

  *priv = (struct WdgtBookSettings){
      .set_scale_cb = set_scale_cb,
      .cb_data = event_data,
  };

  lv_style_t *style = &priv->settings.style;
  *style = (lv_style_t){0};
  lv_style_init(style);
  lv_style_set_flex_flow(style, LV_FLEX_FLOW_ROW_WRAP);
  lv_style_set_flex_main_place(style, LV_FLEX_ALIGN_SPACE_EVENLY);
  lv_style_set_layout(style, LV_LAYOUT_FLEX);
  lv_style_set_pad_column(style, 16);
  lv_style_set_pad_row(style, 8);
  lv_style_set_bg_color(style, lv_color_white());
  lv_obj_add_style(settings, style, LV_PART_MAIN | LV_STATE_DEFAULT);

  err_o = wdgt_settings_field_init(&priv->settings.set_scale_field, settings,
                                   "Set scale", set_scale_cb, event_data);
  ERR_TRY(err_o);

  err_o = wdgt_settings_field_init(&priv->settings.set_x_off_field, settings,
                                   "Set X off", set_scale_cb, event_data);
  ERR_TRY(err_o);

  return 0;

error_out:
  lv_style_reset(style);
  lv_obj_del(settings);
  mem_free(priv);
  *out = NULL;
  return err_o;
};

void wdgt_settings_destroy(wdgt_settings_t *out) {
  if (mem_is_null_ptr(out)) {
    return;
  }

  struct WdgtBookSettings *priv = lv_obj_get_user_data(*out);
  wdgt_settings_field_destroy(&priv->settings.set_scale_field);
  wdgt_settings_field_destroy(&priv->settings.set_x_off_field);
  lv_style_reset(&priv->settings.style);
  lv_obj_del(*out);
  mem_free(priv);
  *out = NULL;
};

struct WdgtSetScale {
  lv_obj_t *field_label;
  void (*event_cb)(lvgl_event_t);
  void *event_data;
};

err_t wdgt_settings_field_init(wdgt_set_scale_t *out, wdgt_settings_t parent,
                               const char *name, void (*event_cb)(lvgl_event_t),
                               void *event_data) {
  puts(__func__);
  const int setting_x = 480;

  lvgl_obj_t field_wx = *out = lvgl_obj_create(parent);

  lv_obj_t *field_label = lv_label_create(field_wx);
  lv_obj_set_style_text_color(field_label, lv_color_black(), LV_PART_MAIN);
  lv_obj_set_size(field_wx, setting_x - 40, 60);
  lv_label_set_text(field_label, name);
  lv_obj_set_user_data(field_wx, field_label);

  // Configure not focused border
  lv_obj_set_style_border_width(field_wx, 3, LV_PART_MAIN | LV_STATE_DEFAULT);

  // Configure focused border
  lv_obj_set_style_border_width(field_wx, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(field_wx, 8, LV_PART_MAIN | LV_STATE_FOCUSED);
  lv_obj_set_style_border_color(field_wx, lv_color_hex(0x00A0FF),
                                LV_PART_MAIN | LV_STATE_FOCUSED);

  // Disable scrolling inside a field
  lv_label_set_long_mode(field_label, LV_LABEL_LONG_MODE_CLIP);
  lv_obj_clear_flag(field_label, LV_OBJ_FLAG_CLICK_FOCUSABLE);
  lv_obj_clear_flag(field_label, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_flag(field_wx, LV_OBJ_FLAG_CLICK_FOCUSABLE);
  lv_obj_clear_flag(field_wx, LV_OBJ_FLAG_SCROLLABLE);

  // Set user cb
  lv_obj_add_event_cb(field_wx, event_cb, LV_EVENT_KEY, event_data);

  return 0;
};

void wdgt_settings_field_destroy(wdgt_set_scale_t *out) {
  if (mem_is_null_ptr(out)) {
    return;
  }

  lv_obj_t *field_wdgt = *out;
  lv_obj_t *field_label = lv_obj_get_user_data(*out);
  lv_obj_del(field_label);
  lv_obj_del(field_wdgt);
  *out = NULL;
}

void settings_field_set_scale_event_cb(lvgl_event_t e) {}

err_t wdgt_set_scale_init(wdgt_set_scale_t *out, double scale,
                          void (*event_cb)(lvgl_event_t), void *event_data) {
  puts(__func__);

  lvgl_obj_t key_catcher = *out = lvgl_obj_create(lv_screen_active());
  lv_obj_set_size(key_catcher, 1, 1);
  lv_obj_set_style_opa(key_catcher, LV_OPA_MIN, LV_PART_MAIN);
  lv_group_focus_obj(key_catcher);
  lv_obj_add_event_cb(key_catcher, event_cb, LV_EVENT_KEY, event_data);

  lvgl_obj_t wdgt_scale = wdgt_set_hor_num_create(scale);  
  lv_obj_set_user_data(key_catcher, wdgt_scale);

  return 0;
};

void wdgt_set_scale_destroy(wdgt_set_scale_t *out) {
  if (mem_is_null_ptr(out)) {
    return;
  }

  lvgl_obj_t key_catcher = *out;
  lvgl_obj_t wdgt_scale = lv_obj_get_user_data(key_catcher);
  lv_obj_del(key_catcher);
  lv_obj_del(wdgt_scale);
  *out = NULL;
}

void wdgt_set_scale_value(wdgt_set_scale_t key_catcher, double value) {
  lvgl_obj_t wdgt_scale = lv_obj_get_user_data(key_catcher);  
  wdgt_set_hor_num_set_value(wdgt_scale, value);
}  

static lvgl_obj_t wdgt_set_hor_num_create(double hor_num) {
  const int setting_x = 600;
  const int setting_y = 300;

  lv_obj_t *set_hor_num = lvgl_obj_create(lv_screen_active());
  lv_gridnav_add(set_hor_num, LV_GRIDNAV_CTRL_NONE);

  lv_obj_set_pos(set_hor_num,
                 (lv_display_get_horizontal_resolution(NULL) - setting_x) / 2,
                 (lv_display_get_vertical_resolution(NULL) - setting_y) / 2);
  lv_obj_set_size(set_hor_num, setting_x, setting_y);
  /* lv_obj_set_style_bg_color(set_hor_num, lv_color_black(), 0); */

  lv_obj_t *up_btn = lvgl_obj_create(set_hor_num);
  lv_obj_t *up_label = lv_label_create(up_btn);
  lv_label_set_text(up_label, LV_SYMBOL_UP);

  char buf[8] = {0};
  snprintf(buf, sizeof(buf), "%2.3f", hor_num);
  lv_obj_t *hor_num_label = lv_label_create(set_hor_num);
  lv_label_set_text(hor_num_label, buf);
  lv_obj_set_user_data(set_hor_num, hor_num_label);
  
  lv_obj_t *down_btn = lvgl_obj_create(set_hor_num);
  lv_obj_t *down_label = lv_label_create(down_btn);
  lv_label_set_text(down_label, LV_SYMBOL_DOWN);

  lv_obj_set_layout(set_hor_num, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(set_hor_num, LV_FLEX_FLOW_COLUMN);

  lv_obj_set_flex_align(set_hor_num, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);

  lv_obj_set_style_pad_row(set_hor_num, 16, 0);
  lv_obj_set_style_pad_all(set_hor_num, 24, 0);

  lv_obj_set_size(up_btn, 220, 90);
  lv_obj_set_size(down_btn, 220, 90);

  lv_obj_center(up_label);
  lv_obj_center(down_label);

  lv_obj_set_style_text_font(hor_num_label, &lv_font_montserrat_30, 0);
  lv_obj_set_style_text_font(up_label, &lv_font_montserrat_30, 0);
  lv_obj_set_style_text_font(down_label, &lv_font_montserrat_30, 0);

  lv_obj_set_style_border_width(hor_num_label, 0,
                                LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(up_label, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(down_label, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(set_hor_num, 0,
                                LV_PART_MAIN | LV_STATE_DEFAULT);

  lv_obj_set_style_text_align(hor_num_label, LV_TEXT_ALIGN_CENTER, 0);

  return set_hor_num;
}

static void wdgt_set_hor_num_set_value(lvgl_obj_t wdgt, double value) {
  lvgl_obj_t label = lv_obj_get_user_data(wdgt);
  char buf[8] = {0};
  snprintf(buf, sizeof(buf), "%2.3f", value);
  lv_label_set_text(label, buf);  
}  
