#include <lvgl.h>

#include "gui/lv/display.h"
#include "display/lv_display.h"
#include "utils/error.h"
#include "utils/config.h"

static lv_display_t *lvgl_init_x11_driver(uint32_t width, uint32_t heigth);

cdk_error_t lvgl_display_init(lvgl_display_t out, void (*callback)(lv_event_t *e), void *data) {
  out->display =
      lvgl_init_x11_driver(CONFIG_DISPLAY_WIDTH, CONFIG_DISPLAY_HEIGTH);
  if (out->display) {
    goto out;
  }

  return cdk_errnos(EINVAL, "No supported lvgl display");
  
out:
  lv_display_add_event_cb(out->display, callback, LV_EVENT_ALL, data);  
  return 0;
}

void lvgl_display_destroy(lvgl_display_t out) {
  if (!out){
    return;
  }

  if (out->display){
    lv_display_delete(out->display);
    out->display = NULL;
  }
}

#ifdef EBOOK_READER_X11_ENABLED
static lv_display_t *lvgl_init_x11_driver(uint32_t width, uint32_t heigth) {
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
#else
static lv_display_t *lvgl_init_x11_driver(uint32_t width, uint32_t heigth) {
  return NULL;
}
#endif // EBOOK_READER_X11_ENABLED
