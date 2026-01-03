#include <display_driver.h>
#include <lvgl.h>
#include <stdint.h>

#include "display/lv_display.h"
#include "gui/lv/display/display.h"
#if defined(EBOOK_READER_X11_ENABLED)
#include "gui/lv/display/x11.h"
#elif defined(EBOOK_READER_WVS7IN5V2B_ENABLED)
#include "gui/lv/display/wvs7in5v2b.h"
#endif
#include "utils/config.h"
#include "utils/error.h"

cdk_error_t lvgl_display_init(lvgl_display_t out,
                              void (*callback)(lv_event_t *e), void *data) {
#if defined(EBOOK_READER_X11_ENABLED)
  if (lvgl_display_driver_x11_probe(&out->driver)) {
    goto out;
  }
#elif defined(EBOOK_READER_WVS7IN5V2B_ENABLED)
  if (lvgl_display_driver_wvs7in5v2b_probe(&out->driver)) {
    goto out;
  }
#else
  #error "No lvgl display enabled during compilation"
#endif

  cdk_errno = cdk_errnos(EINVAL, "No supported lvgl display");

error:
  return cdk_errno;

out:
  out->display = out->driver.init(CONFIG_DISPLAY_WIDTH, CONFIG_DISPLAY_HEIGTH,
                                  &out->driver);
  if (!out->display) {
    cdk_ewrap();
    goto error;
  }

  lv_display_add_event_cb(out->display, callback, LV_EVENT_ALL, data);
  return 0;
}

void lvgl_display_destroy(lvgl_display_t out) {
  if (!out) {
    return;
  }

  if (out->display) {
    lv_display_delete(out->display);
    out->display = NULL;
  }

  if (out->driver.destroy) {
    out->driver.destroy(&out->driver);
    out->driver.destroy = NULL;
  }
}
