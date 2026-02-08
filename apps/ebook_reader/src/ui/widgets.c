#include <lvgl.h>
#include <stdio.h>

#include "ui/widgets.h"
#include "core/lv_obj.h"
#include "core/lv_obj_style_gen.h"
#include "font/lv_symbol_def.h"
#include "layouts/flex/lv_flex.h"
#include "misc/lv_style.h"
#include "utils/mem.h"
#include "utils/time.h"

/* #define EBK_DEBUG_LVGL 1 */

struct UiMenuBookWidget {
  lv_img_dsc_t img;
  ui_t gui;
  int id;
};

struct UiReaderFieldWidget {
  lv_obj_t *field;
  lv_obj_t *label;
  void *data;
  int id;
};

const int bar_y = 48;
const int menu_x_off = 48;
const int menu_y_off = 64;
const int menu_book_x = 296;
const int menu_book_text_y = 80;
const int menu_book_y = 392 + menu_book_text_y;
const int bar_clock_x = 336;

lv_obj_t *ui_wx_obj_create(void *parent) {

  lv_obj_t *obj = lv_obj_create(parent);

#ifdef EBK_DEBUG_LVGL
  static int color = 6000;
  color += 2000;
  lv_obj_set_style_bg_color(obj, lv_color_hex(color), 0);
#endif

  return obj;
}

ui_wx_bar_t ui_wx_bar_create(void) {
  lv_obj_t *bar = ui_wx_obj_create(lv_screen_active());
  lv_obj_set_size(bar, lv_display_get_horizontal_resolution(NULL), bar_y);
  lv_obj_set_pos(bar, 0, 0);

  lv_style_t *bar_style = mem_malloc(sizeof(lv_style_t));
  lv_style_init(bar_style);
  lv_style_set_border_color(bar_style, lv_color_black());
  lv_style_set_border_width(bar_style, 2);
  lv_style_set_border_opa(bar_style, LV_OPA_100);
  lv_style_set_border_side(bar_style,
                           (lv_border_side_t)(LV_BORDER_SIDE_BOTTOM));
  lv_style_set_pad_all(bar_style, 0);
  lv_obj_add_style(bar, bar_style, LV_PART_MAIN | LV_STATE_DEFAULT);

  const int bar_clock_y = bar_y - 2;
  lv_obj_t *clock = ui_wx_obj_create(bar);
  lv_obj_set_style_border_width(clock, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_pos(clock,
                 lv_display_get_horizontal_resolution(NULL) - bar_clock_x, 0);
  lv_obj_set_size(clock, bar_clock_x, bar_clock_y);
  lv_obj_set_user_data(bar, clock);

  lv_obj_t *clock_text = lv_label_create(clock);
  lv_obj_set_style_border_width(clock_text, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_font(clock_text, &lv_font_montserrat_30, 0);
  static char buf[100];
  char *res = time_now_dump(buf, sizeof(buf));
  assert(res != NULL);
  lv_label_set_text(clock_text, time_now_dump(buf, sizeof(buf)));
  lv_obj_set_user_data(clock, clock_text);
  lv_obj_set_user_data(clock_text, bar_style);

  lv_label_set_long_mode(clock_text, LV_LABEL_LONG_MODE_CLIP);
  lv_obj_clear_flag(clock_text, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_clear_flag(clock, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_clear_flag(bar, LV_OBJ_FLAG_SCROLLABLE);

  return bar;
}

void ui_wx_bar_destroy(ui_wx_bar_t bar) {
  lv_obj_t *clock = lv_obj_get_user_data(bar);
  lv_obj_t *clock_text = lv_obj_get_user_data(clock);
  lv_style_t *bar_style = lv_obj_get_user_data(clock_text);
  lv_obj_del(clock_text);
  lv_obj_del(clock);
  lv_style_reset(bar_style);
  mem_free(bar_style);
  lv_obj_del(bar);
}

ui_wx_menu_t ui_wx_menu_create(void) {
  lv_obj_t *menu_container = ui_wx_obj_create(lv_screen_active());
  lv_gridnav_add(menu_container, LV_GRIDNAV_CTRL_NONE);

  int menu_x = lv_display_get_horizontal_resolution(NULL) - menu_x_off * 2;
  int menu_y = lv_display_get_vertical_resolution(NULL) - bar_y - menu_y_off;

  lv_obj_set_pos(menu_container, menu_x_off / 2, bar_y + menu_y_off / 2);
  lv_obj_set_size(menu_container, menu_x, menu_y);
  lv_obj_set_style_pad_ver(menu_container, menu_y_off / 2,
                           LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_pad_hor(menu_container, menu_x_off / 2,
                           LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(menu_container, 0,
                                LV_PART_MAIN | LV_STATE_DEFAULT);

  lv_style_t *style = mem_malloc(sizeof(lv_style_t));
  lv_style_init(style);
  lv_style_set_flex_flow(style, LV_FLEX_FLOW_ROW_WRAP);
  lv_style_set_flex_main_place(style, LV_FLEX_ALIGN_SPACE_EVENLY);
  lv_style_set_layout(style, LV_LAYOUT_FLEX);
  lv_style_set_pad_column(style, 96);
  lv_style_set_pad_row(style, 48);
  lv_style_set_bg_color(style, lv_color_white());
  lv_obj_add_style(menu_container, style, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_user_data(menu_container, style);

  return menu_container;
}

void ui_wx_menu_destroy(ui_wx_menu_t menu) {
  lv_style_t *style = lv_obj_get_user_data(menu);
  lv_style_reset(style);
  lv_obj_del(menu);
  mem_free(style);
}

ui_wx_menu_book_t ui_wx_menu_book_create(ui_wx_menu_t menu,
                                         const char *book_title,
                                         bool is_focused,
                                         const uint8_t *thumbnail, int id,
                                         ui_t gui) {
  lv_obj_t *book_card = ui_wx_obj_create(menu);
  lv_obj_set_size(book_card, menu_book_x + 16, menu_book_y + 16);

  // Configure data required to display book
  struct UiMenuBookWidget *book_data =
      mem_malloc(sizeof(struct UiMenuBookWidget));
  *book_data = (struct UiMenuBookWidget){0};
  book_data->id = id;
  book_data->gui = gui;
  lv_obj_set_user_data(book_card, book_data);

  if (thumbnail) {
    lv_obj_t *book_img = lv_image_create(book_card);
    lv_img_dsc_t *dsc = &book_data->img;
    *dsc = (lv_img_dsc_t){0};
    dsc->header.cf = LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED;
    dsc->header.w = menu_book_x;
    dsc->header.h = (menu_book_y - menu_book_text_y);
    dsc->data_size = dsc->header.w * dsc->header.h * 4;
    dsc->data = thumbnail;
    lv_image_set_src(book_img, dsc);
    lv_obj_set_style_border_width(book_img, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(book_img, LV_OBJ_FLAG_CLICK_FOCUSABLE);
  }

  // Configure book label
  lv_obj_t *book_label = lv_label_create(book_card);
  lv_obj_set_pos(book_label, 0, menu_book_y - (menu_book_text_y * 0.75));
  lv_obj_set_style_text_color(lv_screen_active(), lv_color_black(),
                              LV_PART_MAIN);
  lv_obj_set_style_text_font(book_label, &lv_font_montserrat_24, 0);
  lv_label_set_text(book_label, book_title);

  // Configure not focused border
  lv_obj_set_style_border_width(book_card, 3, LV_PART_MAIN | LV_STATE_DEFAULT);

  // Configure focused border
  lv_obj_set_style_outline_width(book_card, 8, LV_PART_MAIN | LV_STATE_FOCUSED);
  lv_obj_set_style_outline_pad(book_card, 8, LV_PART_MAIN | LV_STATE_FOCUSED);
  lv_obj_set_style_outline_color(book_card, lv_color_hex(0x00A0FF),
                                 LV_PART_MAIN | LV_STATE_FOCUSED);

  // Disable scrolling inside a card
  lv_label_set_long_mode(book_label, LV_LABEL_LONG_MODE_CLIP);
  lv_obj_clear_flag(book_label, LV_OBJ_FLAG_CLICK_FOCUSABLE);
  lv_obj_clear_flag(book_label, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_flag(book_card, LV_OBJ_FLAG_CLICK_FOCUSABLE);
  lv_obj_clear_flag(book_card, LV_OBJ_FLAG_SCROLLABLE);

  return book_card;
}

void ui_wx_menu_book_destroy(ui_wx_menu_book_t book) {
  struct UiMenuBookWidget *book_data = lv_obj_get_user_data(book);
  mem_free(book_data);
  lv_obj_del(book);
};

int ui_wx_menu_book_get_id(ui_wx_menu_book_t book) {
  struct UiMenuBookWidget *book_data = lv_obj_get_user_data(book);
  return book_data->id;
}

ui_t ui_wx_menu_book_get_ui(ui_wx_menu_book_t book) {
  struct UiMenuBookWidget *book_data = lv_obj_get_user_data(book);
  return book_data->gui;
};

ui_wx_reader_t ui_wx_reader_create(int page_len,
                                   const unsigned char *page_buf) {
  puts(__func__);
  lv_obj_t *page_wx = lv_image_create(lv_screen_active());

  static lv_img_dsc_t dsc = {0};
  dsc.header.cf = LV_COLOR_FORMAT_ARGB8888;
  dsc.header.w = lv_display_get_horizontal_resolution(NULL);
  dsc.header.h = lv_display_get_vertical_resolution(NULL);
  dsc.data_size = page_len;
  dsc.data = page_buf;
  lv_image_set_src(page_wx, &dsc);

  return page_wx;
}

void ui_wx_reader_destroy(ui_wx_reader_t reader) { lv_obj_del(reader); }

ui_wx_reader_settings_t ui_wx_reader_settings_create(void) {
  const int setting_x = 480;
  const int setting_y = 800;

  lv_obj_t *settings = ui_wx_obj_create(lv_screen_active());
  lv_gridnav_add(settings, LV_GRIDNAV_CTRL_NONE);

  lv_obj_set_pos(settings,
                 lv_display_get_horizontal_resolution(NULL) - setting_x,
                 lv_display_get_vertical_resolution(NULL) - setting_y);
  lv_obj_set_size(settings, setting_x, setting_y);
  /* lv_obj_set_style_bg_color(settings, lv_color_black(), 0); */

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

  return settings;
}

void ui_wx_reader_settings_destroy(ui_wx_reader_settings_t reader_settings) {
  lv_style_t *style = lv_obj_get_user_data(reader_settings);
  lv_obj_del(reader_settings);
  lv_style_reset(style);
  mem_free(style);
}

ui_wx_reader_settings_field_t
ui_wx_reader_settings_add_field(ui_wx_reader_settings_t reader_settings,
                                const char *field, int id, void *data) {
  const int setting_x = 480;

  ui_wx_reader_settings_field_t field_wx = ui_wx_obj_create(reader_settings);
  lv_obj_t *field_label = lv_label_create(field_wx);
  lv_obj_set_style_text_color(field_label, lv_color_black(), LV_PART_MAIN);
  lv_obj_set_size(field_wx, setting_x - 40, 60);
  lv_label_set_text(field_label, field);

  struct UiReaderFieldWidget *field_data =
      mem_malloc(sizeof(struct UiReaderFieldWidget));
  *field_data = (struct UiReaderFieldWidget){
      .data = data, .id = id, .field = field_wx, .label = field_label};
  lv_obj_set_user_data(field_wx, field_data);

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

  return field_wx;
}

void ui_wx_reader_settings_field_destroy(ui_wx_reader_settings_field_t field) {
  struct UiReaderFieldWidget *field_data = lv_obj_get_user_data(field);
  lv_obj_del(field_data->label);
  lv_obj_del(field_data->field);
  mem_free(field_data);
};

void *
ui_wx_reader_settings_field_get_data(ui_wx_reader_settings_field_t field) {
  struct UiReaderFieldWidget *field_data = lv_obj_get_user_data(field);
  return field_data->data;
}

int *ui_wx_reader_settings_field_get_id(ui_wx_reader_settings_field_t field) {
  struct UiReaderFieldWidget *field_data = lv_obj_get_user_data(field);
  return &field_data->id;
}

ui_wx_reader_set_hor_num_t ui_wx_reader_set_hor_num_create(double hor_num) {
  const int setting_x = 800;
  const int setting_y = 480;

  lv_obj_t *set_hor_num = ui_wx_obj_create(lv_screen_active());
  lv_gridnav_add(set_hor_num, LV_GRIDNAV_CTRL_NONE);

  lv_obj_set_pos(set_hor_num,
                 (lv_display_get_horizontal_resolution(NULL) - setting_x) / 2,
                 (lv_display_get_vertical_resolution(NULL) - setting_y) / 2);
  lv_obj_set_size(set_hor_num, setting_x, setting_y);
  /* lv_obj_set_style_bg_color(set_hor_num, lv_color_black(), 0); */

  lv_obj_t *up_btn = lv_button_create(set_hor_num);
  lv_obj_t *up_label = lv_label_create(up_btn);
  lv_label_set_text(up_label, LV_SYMBOL_UP);

  char buf[8] = {0};
  snprintf(buf, sizeof(buf), "%2.3f", hor_num);
  puts(buf);
  lv_obj_t *hor_num_label = lv_label_create(set_hor_num);
  lv_label_set_text(hor_num_label, buf);

  lv_obj_t *down_btn = lv_button_create(set_hor_num);
  lv_obj_t *down_label = lv_label_create(down_btn);
  lv_label_set_text(down_label, LV_SYMBOL_DOWN);

  lv_obj_set_layout(set_hor_num, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(set_hor_num, LV_FLEX_FLOW_COLUMN);

  lv_obj_set_flex_align(
      set_hor_num,
      LV_FLEX_ALIGN_CENTER,  
      LV_FLEX_ALIGN_CENTER,  
      LV_FLEX_ALIGN_CENTER); 

  lv_obj_set_style_pad_row(set_hor_num, 16, 0);
  lv_obj_set_style_pad_all(
      set_hor_num, 24, 0);


  lv_obj_set_size(up_btn, 220, 90);
  lv_obj_set_size(down_btn, 220, 90);

  lv_obj_center(up_label);
  lv_obj_center(down_label);

  lv_obj_set_style_text_font(hor_num_label, &lv_font_montserrat_30, 0);
  lv_obj_set_style_text_font(up_label, &lv_font_montserrat_30, 0);
  lv_obj_set_style_text_font(down_label, &lv_font_montserrat_30, 0);

  lv_obj_set_style_border_width(hor_num_label, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(up_label, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(down_label, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(set_hor_num, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  
  lv_obj_set_style_text_align(hor_num_label, LV_TEXT_ALIGN_CENTER, 0);

  return set_hor_num;
}

void ui_wx_reader_set_hor_num_destroy(ui_wx_reader_set_hor_num_t hor_num) {
  lv_obj_del(hor_num);
  
};

ui_wx_reader_set_ver_num_t ui_wx_reader_set_ver_num_create(double ver_num) {
  const int ver_num_x = 600;
  const int ver_num_y = 300;

  lv_obj_t *set_ver_num = ui_wx_obj_create(lv_screen_active());
  lv_gridnav_add(set_ver_num, LV_GRIDNAV_CTRL_NONE);

  lv_obj_set_pos(set_ver_num,
                 (lv_display_get_horizontal_resolution(NULL) - ver_num_x) / 2,
                 (lv_display_get_vertical_resolution(NULL) - ver_num_y) / 2);
  lv_obj_set_size(set_ver_num, ver_num_x, ver_num_y);

  lv_obj_t *left_btn = lv_button_create(set_ver_num);
  lv_obj_t *left_label = lv_label_create(left_btn);
  lv_label_set_text(left_label, LV_SYMBOL_LEFT);

  char buf[8] = {0};
  snprintf(buf, sizeof(buf), "%5.5d", (int)ver_num);
  puts(buf);  
  lv_obj_t *ver_num_label_cont = lv_obj_create(set_ver_num);
  lv_obj_t *ver_num_label = lv_label_create(ver_num_label_cont);
  lv_label_set_text(ver_num_label, buf);
  lv_obj_align(ver_num_label, LV_ALIGN_CENTER, 0, 0);
  
  lv_obj_t *right_btn = lv_button_create(set_ver_num);
  lv_obj_t *right_label = lv_label_create(right_btn);
  lv_label_set_text(right_label, LV_SYMBOL_RIGHT);

  lv_obj_set_layout(set_ver_num, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(set_ver_num, LV_FLEX_FLOW_ROW);

  lv_obj_set_flex_align(
      set_ver_num,
      LV_FLEX_ALIGN_CENTER,  
      LV_FLEX_ALIGN_CENTER,  
      LV_FLEX_ALIGN_CENTER); 

  lv_obj_set_size(left_btn, lv_pct(25), 90);
  lv_obj_set_size(right_btn, lv_pct(25), 90);
  lv_obj_set_size(ver_num_label_cont, lv_pct(25), 90);  

  lv_obj_center(left_label);
  lv_obj_center(right_label);
  lv_obj_center(ver_num_label_cont);  

  lv_obj_set_style_text_font(ver_num_label, &lv_font_montserrat_30, 0);
  lv_obj_set_style_text_font(left_label, &lv_font_montserrat_30, 0);
  lv_obj_set_style_text_font(right_label, &lv_font_montserrat_30, 0);

  lv_obj_set_style_border_width(ver_num_label_cont, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(left_label, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(right_label, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(set_ver_num, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

  return set_ver_num;
}

void ui_wx_reader_set_ver_num_destroy(ui_wx_reader_set_ver_num_t ver_num) {
  lv_obj_del(ver_num);
};
