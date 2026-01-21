#ifndef UI_SCREEN_MENU_H
#define UI_SCREEN_MENU_H
#include "book/book.h"
#include "ui/screen.h"
#include "utils/err.h"


err_t ui_screen_reader_create(ui_screen_t *screen, ui_t ui, book_t book,
                            int book_i, int event,
                            void (*event_cb)(lv_event_t *e));

#endif // UI_SCREEN_MENU_H
