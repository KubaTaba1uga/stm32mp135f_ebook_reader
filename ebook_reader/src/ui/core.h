#ifndef UI_CORE_H
#define UI_CORE_H
#include "book/book.h"
#include "ui/ui.h"
#include "widgets.h"

struct UiMenu {
  ui_wx_menu_t menu;
  ui_wx_menu_book_t *books;
  lv_group_t *group;
  ui_t owner;
};

err_t ui_menu_create(ui_t ui, books_list_t books, int selected_book_i);

#endif // UI_CORE_H
