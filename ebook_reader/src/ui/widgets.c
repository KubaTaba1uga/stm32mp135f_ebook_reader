#include <lvgl.h>

#include "ui/widgets.h"
#include "utils/mem.h"

/* #define EBK_DEBUG_LVGL 1 */

struct UiMenuBookWidget {
  lv_img_dsc_t img;
  ui_t gui;
  int id;
};

const int bar_y = 30;
const int menu_x_off = 10;
const int menu_y_off = 20;
const int menu_book_x = 120;
const int menu_book_text_y = 50;
const int menu_book_y = 170 + menu_book_text_y;

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

  lv_obj_set_style_border_width(bar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_pad_all(bar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

  return bar;
}

void ui_wx_bar_destroy(ui_wx_bar_t bar) { lv_obj_del(bar); }

ui_wx_menu_t ui_wx_menu_create(void) {
  lv_obj_t *menu_container = ui_wx_obj_create(lv_screen_active());
  lv_gridnav_add(menu_container, LV_GRIDNAV_CTRL_NONE);

  int menu_x = lv_display_get_horizontal_resolution(NULL) - menu_x_off * 2;
  int menu_y = lv_display_get_vertical_resolution(NULL) - bar_y - menu_y_off;

  lv_obj_set_pos(menu_container, menu_x_off / 2,
                 bar_y + menu_y_off / 2); // set offset 20, 20 for menu
  lv_obj_set_size(menu_container, menu_x, menu_y);
  lv_obj_set_style_pad_ver(menu_container, menu_y_off / 2,
                           LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_pad_hor(menu_container, menu_x_off / 2,
                           LV_PART_MAIN | LV_STATE_DEFAULT);
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
}

void ui_wx_menu_destroy(ui_wx_menu_t menu) {
  

  lv_obj_del(menu); }

ui_wx_menu_book_t ui_wx_menu_book_create(ui_wx_menu_t menu,
                                         const char *book_title,
                                         bool is_focused, const uint8_t *thumbnail,
                                         int id, ui_t gui) {
  lv_obj_t *book_card = ui_wx_obj_create(menu);  
  lv_obj_set_size(book_card, menu_book_x, menu_book_y);

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
    dsc->header.cf = LV_COLOR_FORMAT_A1;
    dsc->header.w = menu_book_x;
    dsc->header.h = (menu_book_y - menu_book_text_y);
    dsc->data_size = ((dsc->header.w + 7) / 8) * dsc->header.h;
    dsc->data = thumbnail;
    lv_image_set_src(book_img, dsc);
    lv_obj_set_style_border_width(book_img, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(book_img, LV_OBJ_FLAG_CLICK_FOCUSABLE);    
  }

  // Configure book label
  lv_obj_t *book_label = lv_label_create(book_card);
  lv_obj_set_pos(book_label, 0, menu_book_y - (menu_book_text_y * 0.75));
  lv_obj_set_style_text_color(lv_screen_active(), lv_color_black(),
                              LV_PART_MAIN);
  lv_label_set_text(book_label, book_title);

  // Configure not focused border
  lv_obj_set_style_border_width(book_card, 1, LV_PART_MAIN | LV_STATE_DEFAULT);

  // Configure focused border
  lv_obj_set_style_outline_width(book_card, 4, LV_PART_MAIN | LV_STATE_FOCUSED);
  lv_obj_set_style_outline_pad(book_card, 4, LV_PART_MAIN | LV_STATE_FOCUSED);
  lv_obj_set_style_outline_color(book_card, lv_color_hex(0x00A0FF),
                                 LV_PART_MAIN | LV_STATE_FOCUSED);

  // Disable scrolling inside a card
  lv_label_set_long_mode(book_label, LV_LABEL_LONG_WRAP);
  lv_obj_clear_flag(book_label, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_scroll_dir(book_label, LV_DIR_NONE);
  lv_obj_add_flag(book_card, LV_OBJ_FLAG_CLICK_FOCUSABLE);
  lv_obj_clear_flag(book_label, LV_OBJ_FLAG_CLICK_FOCUSABLE);
  lv_obj_clear_flag(book_card, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_scroll_dir(book_card, LV_DIR_NONE);

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
