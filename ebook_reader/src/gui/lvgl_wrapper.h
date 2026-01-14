#ifndef EBOOK_READER_GUI_LVGL_WRAPPER_H
#define EBOOK_READER_GUI_LVGL_WRAPPER_H

#include <lvgl.h>

/**
  Managing all LVGL objects directly require a lot of code so we introdouce
  this middle layer that needs to manage look of all widgets use inside our
  app.
*/

typedef lv_obj_t *ebklv_widget_bar_t;
ebklv_widget_bar_t ebklv_bar_create(void);
void ebklv_bar_destroy(ebklv_widget_bar_t);

lv_obj_t *ebklv_obj_create(void *parent);

typedef lv_obj_t *ebklv_widget_menu_t;
ebklv_widget_menu_t ebklv_menu_create(void);
void ebklv_menu_destroy(ebklv_widget_menu_t);

#endif // EBOOK_READER_GUI_LVGL_WRAPPER_H
