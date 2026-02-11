#ifndef EBOOK_READER_MENU_SCREEN_CORE_H
#define EBOOK_READER_MENU_SCREEN_CORE_H
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
extern const int menu_book_text_y;
extern const int menu_book_y;
extern const int bar_clock_x;


typedef lv_obj_t *wx_bar_t;
wx_bar_t wx_bar_create(void);
void wx_bar_destroy(wx_bar_t);

typedef lv_obj_t *wx_menu_t;
wx_menu_t wx_menu_create(void);
void wx_menu_destroy(wx_menu_t);

typedef lv_obj_t *wx_menu_book_t;
wx_menu_book_t wx_menu_book_create(wx_menu_t menu,
                                         const char *book_title,
                                         bool is_focused,
                                         const uint8_t *thumbnail, void *data);
void *wx_menu_book_get_data(wx_menu_book_t);
ui_t wx_menu_book_get_ui(wx_menu_book_t);
void wx_menu_book_destroy(wx_menu_book_t);

#endif // EBOOK_READER_MENU_SCREEN_CORE_H
