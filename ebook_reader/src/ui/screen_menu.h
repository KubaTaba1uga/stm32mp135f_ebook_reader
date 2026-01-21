#ifndef UI_SCREEN_MENU_H
#define UI_SCREEN_MENU_H
#include "ui/screen.h"
#include "utils/err.h"

err_t ui_screen_menu_create(ui_screen_t *screen, ui_t ui, books_list_t books,
                            int book_i, int event,
                            void (*event_cb)(lv_event_t *e), lv_group_t *);

err_t ui_screen_menu_focus_book(ui_screen_t screen, int book_i);

#endif // UI_SCREEN_MENU_H
