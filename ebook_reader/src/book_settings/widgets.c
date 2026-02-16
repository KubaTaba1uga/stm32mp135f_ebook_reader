#include <lvgl.h>
#include <stdio.h>

#include "book_settings/core.h"
#include "core/lv_obj.h"
#include "library/library.h"
#include "lv_api_map_v8.h"
#include "misc/lv_event.h"
#include "utils/lvgl.h"
#include "utils/mem.h"

struct UiReaderFieldWidget {
  lv_obj_t *field;
  lv_obj_t *label;
  void *data;
};

err_t wdgt_settings_init(wdgt_settings_t *out) {
  puts(__func__);
  const int setting_x = 480;
  const int setting_y = 800;

  wdgt_settings_t settings = *out = lvgl_obj_create(lv_screen_active());
  lv_gridnav_add(settings, LV_GRIDNAV_CTRL_NONE);

  lv_obj_set_pos(settings,
                 lv_display_get_horizontal_resolution(NULL) - setting_x,
                 lv_display_get_vertical_resolution(NULL) - setting_y);
  lv_obj_set_size(settings, setting_x, setting_y);

  lv_style_t *style = mem_malloc(sizeof(lv_style_t));
  lv_style_init(style);
  lv_style_set_flex_flow(style, LV_FLEX_FLOW_ROW_WRAP);
  lv_style_set_flex_main_place(style, LV_FLEX_ALIGN_SPACE_EVENLY);
  lv_style_set_layout(style, LV_LAYOUT_FLEX);
  lv_style_set_pad_column(style, 16);
  lv_style_set_pad_row(style, 8);
  lv_style_set_bg_color(style, lv_color_white());
  lv_obj_add_style(settings, style, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_user_data(settings, style);

  return 0;
};

void wdgt_settings_destroy(wdgt_settings_t *out) {
  if (mem_is_null_ptr(out)) {
    return;
  }

  lv_style_t *style = lv_obj_get_user_data(*out);
  lv_obj_del(*out);
  lv_style_reset(style);
  mem_free(style);
};

struct WdgtSetScale {
  lv_obj_t *field_label;
  void (*event_cb)(lvgl_event_t);
  void *event_data;
};
static void set_scale_field_event_cb(lvgl_event_t);

err_t wdgt_set_scale_init(wdgt_set_scale_t *out, wdgt_settings_t parent,
                          double scale, void (*event_cb)(lvgl_event_t),
                          void *event_data) {
  puts(__func__);
  const int setting_x = 480;
  struct WdgtSetScale *priv = mem_malloc(sizeof(struct WdgtSetScale));
  *priv = (struct WdgtSetScale){
      .event_cb = event_cb,
      .event_data = event_data,
  };

  lvgl_obj_t field_wx = *out = lvgl_obj_create(parent);
  lv_obj_set_user_data(field_wx, priv);

  lv_obj_t *field_label = priv->field_label = lv_label_create(field_wx);
  lv_obj_set_style_text_color(field_label, lv_color_black(), LV_PART_MAIN);
  lv_obj_set_size(field_wx, setting_x - 40, 60);
  lv_label_set_text(field_label, "Set scale");

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
  lv_obj_add_event_cb(field_wx, set_scale_field_event_cb, LV_EVENT_KEY, priv);

  return 0;
};

void wdgt_set_scale_destroy(wdgt_set_scale_t *out) {
  if (mem_is_null_ptr(out)) {
    return;
  }

  lv_obj_t *field_wdgt = *out;
  lv_obj_t *field_label = lv_obj_get_user_data(*out);
  lv_obj_del(field_wdgt);
  lv_obj_del(field_label);
  *out = NULL;
}

static void set_scale_field_event_cb(lvgl_event_t e) {
  struct WdgtSetScale *set_scale = lv_event_get_user_data(e);
  // TO-DO: create view for scale  where user can change the value,
  //        remember to call scale_view_event_cb when left rigth is
  //        detected.
};
