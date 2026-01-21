#ifndef UI_DISPLAY_H
#define UI_DISPLAY_H
#include <lvgl.h>
#include <stdbool.h>
#include <stdint.h>

#include "ui/ui.h"
#include "utils/err.h"
#include "utils/settings.h"

typedef struct UiDisplay *ui_display_t;

struct UiDisplay {
  err_t (*render)(void *, unsigned char *, uint32_t);
  void (*destroy)(void *);
  void (*panic)(void *);
  lv_group_t *lv_ingroup;
  lv_display_t *lv_disp;
  lv_indev_t *lv_indv;
  void *owner;
  ui_t ui;
};

err_t ui_display_init(ui_display_t, ui_t, enum DisplayModelEnum);
err_t ui_display_render(ui_display_t, unsigned char *, uint32_t);
void ui_display_destroy(ui_display_t);
void ui_display_panic(ui_display_t);
lv_group_t *ui_display_get_input_group(ui_display_t);
err_t ui_display_show_boot_img(ui_display_t);

err_t ui_display_dd_init(ui_display_t, ui_t);
err_t ui_display_x11_init(ui_display_t, ui_t);

#endif // UI_DISPLAY_H
