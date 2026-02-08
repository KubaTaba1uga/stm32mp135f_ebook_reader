#ifndef UI_SCREEN_H
#define UI_SCREEN_H
/**
    Each screen is in 1:1 relation with app module.
    So we have screen for menu module, we have state for reader module etc.
    Role of modules is to manage the screen memory, role of the screen itself
    is to present module's data in consistent way.
*/

#include <lvgl.h>
#include <stdbool.h>
#include <stdint.h>

#include "ui/ui.h"
#include "utils/err.h"

typedef struct UiScreen *ui_screen_t;

struct UiScreen {
  void (*destroy)(void *);
  void *screen_data;
  ui_t owner;
};

void ui_screen_destroy(ui_screen_t);

err_t ui_screen_menu_init(ui_screen_t, ui_t, books_list_t, int, int,
                          void (*)(lv_event_t *), lv_group_t *);
err_t ui_screen_menu_focus_book(ui_screen_t, int);

err_t ui_screen_reader_init(ui_screen_t out, ui_t ui, book_t book, int event,
                            void (*event_cb)(lv_event_t *e, void *data,
                                             ui_t ui),
                            lv_group_t *group);

err_t ui_screen_reader_settings_init(ui_screen_t screen, const char **fields,
                                     int fields_len, lv_group_t *group);
void ui_screen_reader_settings_destroy(ui_screen_t screen);

err_t ui_screen_reader_set_scale_init(ui_screen_t screen, book_t book,
                                      lv_group_t *group);
void ui_screen_reader_set_scale_destroy(ui_screen_t screen);

err_t ui_screen_reader_set_x_off_init(ui_screen_t screen, book_t book,
                                      lv_group_t *group);
void ui_screen_reader_set_x_off_destroy(ui_screen_t screen);

err_t ui_screen_reader_set_y_off_init(ui_screen_t screen, book_t book,
                                      lv_group_t *group);
void ui_screen_reader_set_y_off_destroy(ui_screen_t screen);


#endif // UI_SCREEN_H
