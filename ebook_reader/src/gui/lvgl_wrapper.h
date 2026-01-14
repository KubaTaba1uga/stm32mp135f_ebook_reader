#ifndef EBOOK_READER_GUI_LVGL_WRAPPER_H
#define EBOOK_READER_GUI_LVGL_WRAPPER_H

#include "gui/gui.h"
#include <lvgl.h>
#include <stdint.h>

/**
  Managing all LVGL objects directly require a lot of code so we introdouce
  this middle layer that needs to manage look of all widgets use inside our
  app.
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
                                                uint8_t *thumbnail, int id, ebk_gui_t gui);
int ebklv_menu_book_get_id(ebklv_widget_menu_book_t book);
ebk_gui_t ebklv_menu_book_get_gui(ebklv_widget_menu_book_t book);
void ebklv_menu_book_destroy(ebklv_widget_menu_book_t);

#endif // EBOOK_READER_GUI_LVGL_WRAPPER_H
