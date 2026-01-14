#include <lvgl.h>

#define EBK_DEBUG_LVGL 1
static const int bar_y = 30;
static const int menu_x_off = 10;
static const int menu_y_off = 20;

lv_obj_t *ebklv_obj_create(void *parent) {

  lv_obj_t *obj = lv_obj_create(parent);

#ifdef EBK_DEBUG_LVGL
  static int color = 6000;
  color += 2000;
  lv_obj_set_style_bg_color(obj, lv_color_hex(color), 0);
#endif

  return obj;
}

typedef lv_obj_t *ebklv_widget_bar_t;
ebklv_widget_bar_t ebklv_bar_create(void) {
  lv_obj_t *bar = ebklv_obj_create(lv_screen_active());
  lv_obj_set_size(bar, lv_display_get_horizontal_resolution(NULL), bar_y);
  lv_obj_set_pos(bar, 0, 0);

  lv_obj_set_style_border_width(bar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_pad_all(bar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

  return bar;
}

void ebklv_bar_destroy(ebklv_widget_bar_t bar) { lv_obj_del(bar); }

typedef lv_obj_t *ebklv_widget_menu_t;
ebklv_widget_menu_t ebklv_menu_create(void) {
  lv_obj_t *menu_container = ebklv_obj_create(lv_screen_active());
  lv_gridnav_add(menu_container, LV_GRIDNAV_CTRL_NONE);

  int menu_x = lv_display_get_horizontal_resolution(NULL) - menu_x_off * 2;
  int menu_y = lv_display_get_vertical_resolution(NULL) - bar_y - menu_y_off;

  lv_obj_set_pos(menu_container, menu_x_off,
                 bar_y + menu_y_off); // set offset 20, 20 for menu
  lv_obj_set_size(menu_container, menu_x, menu_y);
  lv_obj_set_style_pad_all(menu_container, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(menu_container, 0,
                                LV_PART_MAIN | LV_STATE_DEFAULT);

  static lv_style_t style;
  lv_style_init(&style);
  lv_style_set_flex_flow(&style, LV_FLEX_FLOW_ROW_WRAP);
  lv_style_set_flex_main_place(&style, LV_FLEX_ALIGN_SPACE_EVENLY);
  lv_style_set_layout(&style, LV_LAYOUT_FLEX);
  lv_style_set_pad_column(&style, 40);
  lv_style_set_pad_row(&style, 20);
  lv_obj_add_style(menu_container, &style, 0);

  return menu_container;
  /* lv_obj_set_size(cont1, lv_pct(50), lv_pct(100)); */
}

void ebklv_menu_destroy(ebklv_widget_menu_t menu) { return; }
