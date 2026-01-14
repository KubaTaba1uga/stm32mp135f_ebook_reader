#ifndef EBOOK_READER_GUI_INTERNAL_H
#define EBOOK_READER_GUI_INTERNAL_H
#include <lvgl.h>
#include <stdbool.h>

struct ebk_GuiMenu {
  lv_obj_t *menu;
  lv_obj_t **books;
  lv_group_t *group;  
};

#endif // EBOOK_READER_GUI_INTERNAL_H
