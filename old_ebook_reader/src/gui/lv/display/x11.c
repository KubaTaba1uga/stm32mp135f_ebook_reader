#include <lvgl.h>

#include "gui/lv/display/driver.h"
#include "gui/lv/display/x11.h"
#include "utils/error.h"

static lv_display_t *x11_init(uint32_t width, uint32_t heigth,
                              struct LvglDisplayDriver *out) {
  lv_display_t *disp = lv_x11_window_create("ebook_reader", width, heigth);

  if (!disp) {
    cdk_errno = cdk_errnos(EINVAL, "Cannot initialize X11 display");
    goto error_out;
  }

  lv_x11_inputs_create(disp, NULL);

  return disp;

error_out:
  return NULL;
};

static void x11_destroy(struct LvglDisplayDriver *out) {}

bool lvgl_display_driver_x11_probe(struct LvglDisplayDriver *out) {
  out->init = x11_init;
  out->destroy = x11_destroy;
  return true;
}
