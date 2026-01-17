#ifndef UI_SCREEN_H
#define UI_SCREEN_H
#include <lvgl.h>
#include <stdbool.h>
#include <stdint.h>

#include "ui/ui.h"
#include "utils/err.h"

typedef struct UiScreen *ui_screen_t;

enum UiScreenEnum {
  UiScreenEnum_MENU = 0,
  UiScreenEnum_READER,
  UiScreenEnum_MAX,
};

struct UiScreen {
  void (*destroy)(ui_screen_t);
  void (*panic)(ui_screen_t);
  void *private;
};

extern const int ui_screen_color_format;

err_t ui_screen_init(ui_screen_t *, uint32_t, ui_t);
err_t ui_screens_render(ui_screen_t *, unsigned char *);
err_t ui_screens_render_cleanup(ui_screen_t *, unsigned char *);
err_t ui_screens_destroy(ui_screen_t, ui_t);

#endif // UI_SCREEN_H
