#ifndef EBOOK_READER_GUI_LVGL_WRAPPER_H
#define EBOOK_READER_GUI_LVGL_WRAPPER_H

#include "gui/gui.h"
#include <lvgl.h>
#include <stdint.h>

/**
  Managing all LVGL objects directly quickly become messy, all objs are of the
  same type and it's easy to lose creation of the object among multiple settings
  we are adjusting, so we introduce this middle layer.

  Because lvgl havily uses concept of generic obj we distinguish different objs
  based on it's role in our app. For example we have bar object wich stands for
  upper bar with date, battery status and clock. We also have menu object wich
  stands for the are under the bar wich is used when we are opening menu.

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

typedef lv_obj_t *ebklv_widget_bar_t;
ebklv_widget_bar_t ebklv_bar_create(void);
void ebklv_bar_destroy(ebklv_widget_bar_t);

lv_obj_t *ebklv_obj_create(void *parent);

typedef lv_obj_t *ebklv_widget_menu_t;
ebklv_widget_menu_t ebklv_menu_create(void);
void ebklv_menu_destroy(ebklv_widget_menu_t);

typedef lv_obj_t *ebklv_widget_menu_book_t;
ebklv_widget_menu_book_t ebklv_menu_book_create(ebklv_widget_menu_t menu,
                                                const char *book_title,
                                                bool is_focused,
                                                uint8_t *thumbnail, int id,
                                                ebk_gui_t gui);
int ebklv_menu_book_get_id(ebklv_widget_menu_book_t book);
ebk_gui_t ebklv_menu_book_get_gui(ebklv_widget_menu_book_t book);
void ebklv_menu_book_destroy(ebklv_widget_menu_book_t);

#endif // EBOOK_READER_GUI_LVGL_WRAPPER_H
