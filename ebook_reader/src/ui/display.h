#ifndef UI_DISPLAY_H
#define UI_DISPLAY_H
#include <lvgl.h>
#include <stdbool.h>
#include <stdint.h>

#include "ui/ui.h"
#include "utils/err.h"
#include "utils/settings.h"

typedef struct UiDisplay *ui_display_t;

err_t ui_display_supported_create(ui_display_t *, ui_t, enum DisplayModelEnum);
err_t ui_display_create(ui_display_t *, lv_display_t *, ui_t,
                        err_t (*render_create)(void *, unsigned char *,
                                               uint32_t),
                        void (*render_destroy)(void *), void (*destroy)(void *),
                        void (*panic)(void *), void *);
err_t ui_display_render_create(ui_display_t , unsigned char *, uint32_t);
void ui_display_render_destroy(ui_display_t);
void ui_display_panic(ui_display_t);
void ui_display_destroy(ui_display_t *);
ui_t ui_display_get_ui(ui_display_t);
lv_display_t *ui_display_get_lv_obj(ui_display_t);
err_t ui_display_show_boot_img(ui_display_t, const char *);

#endif // UI_DISPLAY_H
