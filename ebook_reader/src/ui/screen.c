#include "ui/screen.h"
#include <string.h>

void ui_screen_destroy(ui_screen_t out) {
  puts(__func__);
  if (!out) {
    return;
  }

  if (out->destroy) {
    out->destroy(out->screen_data);
  }

  memset(out, 0, sizeof(struct UiScreen));
};
