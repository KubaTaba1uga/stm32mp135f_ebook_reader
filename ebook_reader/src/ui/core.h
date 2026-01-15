#ifndef UI_CORE_H
#define UI_CORE_H
#include "widgets.h"

struct UiMenu {
  ui_wx_menu_t menu;
  ui_wx_menu_book_t *books;
  lv_group_t *group;
};

#endif // UI_CORE_H
