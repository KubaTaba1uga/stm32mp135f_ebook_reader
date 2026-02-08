#ifndef UI_WIDGETS_H
#define UI_WIDGETS_H
#include <lvgl.h>
#include <stdint.h>

#include "ui/ui.h"

/**
  Managing all LVGL objects directly quickly become messy, all objs are of the
  same type and it's easy to lose creation of the object among multiple settings
  we are adjusting, so we introduce this middle layer.

  Because lvgl havily uses concept of generic obj we distinguish different objs
  based on it's role in our app. For example we have bar object wich stands for
  upper bar with date, battery status and clock.

  There is no precise definition telling what should be placed in this wrapper
  other files can use lvgl directly, i would say that as a rule of thumb, if you
  are creating multiple lvgl objects it may be idea to group them as a widget
  and place them in this file.
*/

extern const int bar_y;
extern const int menu_x_off;
extern const int menu_y_off;
extern const int menu_book_x;
extern const int menu_book_y;
extern const int menu_book_text_y;

typedef lv_obj_t *ui_wx_bar_t;
ui_wx_bar_t ui_wx_bar_create(void);
void ui_wx_bar_destroy(ui_wx_bar_t);

typedef lv_obj_t *ui_wx_menu_t;
ui_wx_menu_t ui_wx_menu_create(void);
void ui_wx_menu_destroy(ui_wx_menu_t);

typedef lv_obj_t *ui_wx_menu_book_t;
ui_wx_menu_book_t ui_wx_menu_book_create(ui_wx_menu_t menu,
                                         const char *book_title,
                                         bool is_focused,
                                         const uint8_t *thumbnail, int id,
                                         ui_t ui);
int ui_wx_menu_book_get_id(ui_wx_menu_book_t);
ui_t ui_wx_menu_book_get_ui(ui_wx_menu_book_t);
void ui_wx_menu_book_destroy(ui_wx_menu_book_t);

typedef lv_obj_t *ui_wx_reader_t;
ui_wx_reader_t ui_wx_reader_create(int page_len, const unsigned char *page_buf);
void ui_wx_reader_destroy(ui_wx_reader_t reader);

typedef lv_obj_t *ui_wx_reader_settings_t;
ui_wx_reader_settings_t
ui_wx_reader_settings_create(void);
void ui_wx_reader_settings_destroy(ui_wx_reader_settings_t reader_settings);

typedef lv_obj_t *ui_wx_reader_settings_field_t;
ui_wx_reader_settings_field_t ui_wx_reader_settings_add_field(ui_wx_reader_settings_t reader_settings, const char *field, int i, void *data);
void ui_wx_reader_settings_field_destroy(ui_wx_reader_settings_field_t field);
void *ui_wx_reader_settings_field_get_data(ui_wx_reader_settings_field_t field);
int *ui_wx_reader_settings_field_get_id(ui_wx_reader_settings_field_t field);

typedef lv_obj_t *ui_wx_reader_set_hor_num_t;
ui_wx_reader_set_hor_num_t
ui_wx_reader_set_hor_num_create(double hor_num);
void ui_wx_reader_set_hor_num_destroy(ui_wx_reader_set_hor_num_t );

typedef lv_obj_t *ui_wx_reader_set_ver_num_t;
ui_wx_reader_set_ver_num_t
ui_wx_reader_set_ver_num_create(double ver_num);
void ui_wx_reader_set_ver_num_destroy(ui_wx_reader_set_ver_num_t );


#endif // UI_WIDGETS_H
