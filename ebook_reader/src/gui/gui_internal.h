#ifndef EBOOK_READER_GUI_INTERNAL_H
#define EBOOK_READER_GUI_INTERNAL_H
#include <lvgl.h>
#include <stdbool.h>

struct ebk_GuiMenu {
  lv_obj_t *menu;
  lv_obj_t *menu_books_table;
  int32_t *row_dsc;
  int32_t *col_dsc;
  lv_obj_t **books;
};

#endif // EBOOK_READER_GUI_INTERNAL_H
