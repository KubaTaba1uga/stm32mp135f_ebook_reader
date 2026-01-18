#ifndef UI_DISPLAY_X11_H
#define UI_DISPLAY_X11_H
#include <lvgl.h>
#include <stdbool.h>

#include "ui/ui.h"
#include "utils/err.h"
#include "ui/display.h"

err_t ui_display_x11_create(ui_display_t *, ui_t);

#endif // UI_DISPLAY_X11_H
