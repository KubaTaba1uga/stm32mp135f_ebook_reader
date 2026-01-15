#ifndef UI_DISPLAY_H
#define UI_DISPLAY_H
#include <lvgl.h>
#include <stdbool.h>
#include <stdint.h>

#include "misc/lv_color.h"
#include "ui/ui.h"
#include "utils/err.h"

typedef struct UiDisplay *ui_display_t;

enum UiDisplayEnum {
  UiDisplayEnum_X11 = 0,
  UiDisplayEnum_WVS7IN5V2B,
  UiDisplayEnum_MAX,
};

struct UiDisplay {
  lv_display_t *(*get_lv_display)(ui_display_t);
  err_t (*render)(ui_display_t, unsigned char *);
  void (*render_cleanup)(ui_display_t);  
  int (*get_render_size)(ui_display_t);
  int (*get_render_x)(ui_display_t);
  int (*get_render_y)(ui_display_t);
  int (*get_color_format)(ui_display_t);
  void (*destroy)(ui_display_t);
  void *private;
};

extern const int ui_display_color_format;

err_t ui_display_x11_init(ui_display_t, ui_t);

#endif // UI_DISPLAY_H
