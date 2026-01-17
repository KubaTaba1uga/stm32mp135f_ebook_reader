#ifndef UI_SCREEN_H
#define UI_SCREEN_H
#include <lvgl.h>
#include <stdbool.h>
#include <stdint.h>

#include "ui/ui.h"
#include "utils/err.h"

typedef struct UiScreen *ui_screen_t;

/* enum UiScreenEnum { */
/*   UiScreenEnum_MENU = 0, */
/*   UiScreenEnum_READER, */
/*   UiScreenEnum_MAX, */
/* }; */

err_t ui_screen_init(ui_screen_t *, ui_t, void (*destroy)(void *), void *);
void ui_screen_destroy(ui_screen_t *);
ui_t ui_screen_get_ui(ui_screen_t);

#endif // UI_SCREEN_H
