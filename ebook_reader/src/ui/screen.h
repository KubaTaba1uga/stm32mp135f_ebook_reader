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

err_t ui_screen_create(ui_screen_t *, ui_t, void (*destroy)(void *), void *);
void ui_screen_destroy(ui_screen_t *);
ui_t ui_screen_get_ui(ui_screen_t);

#endif // UI_SCREEN_H
