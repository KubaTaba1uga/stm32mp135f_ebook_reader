#ifndef UI_DISPLAY_WVS7IN5V2_H
#define UI_DISPLAY_WVS7IN5V2_H
#include <lvgl.h>
#include <stdbool.h>

#include "utils/err.h"
#include "ui/display.h"

err_t ui_display_wvs7in5v2_create(ui_display_t *, ui_t);
lv_group_t * ui_display_wvs7in5v2_get_input_group(ui_display_t );

#endif // UI_DISPLAY_WVS7IN5V2_H
