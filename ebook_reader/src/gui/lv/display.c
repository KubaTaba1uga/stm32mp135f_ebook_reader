#include <display_driver.h>
#include <lvgl.h>
#include <stdint.h>

#include "display/lv_display.h"
#include "gui/lv/display/display.h"
#include "gui/lv/wvs7in5v2b.h"
#include "gui/lv/x11.h"
#include "misc/lv_color.h"
#include "utils/config.h"
#include "utils/error.h"
#include "utils/log.h"
#include "utils/memory.h"

static lv_display_t *lvgl_driver_init(uint32_t width, uint32_t heigth);

cdk_error_t lvgl_display_init(lvgl_display_t out,
                              void (*callback)(lv_event_t *e), void *data) {
#ifdef EBOOK_READER_X11_ENABLED
  cdk_errno = lvgl_driver_x11_init(CONFIG_DISPLAY_WIDTH, CONFIG_DISPLAY_HEIGTH, &out->x11_driver);
  if (!cdk_errno) {
    goto out;
  }

#endif
  
#ifdef EBOOK_READER_WVS7IN5V2B_ENABLED
  cdk_errno = lvgl_driver_wvs7in5v2b_init(CONFIG_DISPLAY_WIDTH, CONFIG_DISPLAY_HEIGTH, &out->wvs7in5v2b_driver);
  if (!cdk_errno) {
    goto out;
  }
#endif
  

  return cdk_errnos(EINVAL, "No supported lvgl display");

out:
  lvgl_driver_get_display()
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
}

#if defined(EBOOK_READER_X11_ENABLED)
static lv_display_t *lvgl_init_driver(uint32_t width, uint32_t heigth) {
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

#elif defined(EBOOK_READER_WVS7IN5V2B_ENABLED)
#define DD_TRY_CATCH(err, label)                                               \
  if (err) {                                                                   \
    cdk_errno = lvgl_convert_dd_error(err);                                    \
    goto label;                                                                \
  }
#define DD_TRY(err) DD_TRY_CATCH(err, error)
#define BYTES_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_I1))

struct LvglWvs7In5V2b {
  dd_wvs75v2b_t dd;
  struct {
    char *data;
    uint32_t len;
  } buf;
};

static cdk_error_t lvgl_convert_dd_error(dd_error_t err);
static cdk_error_t lvgl_flush_dd_callback(lv_display_t *display,
                                          const lv_area_t *area,
                                          uint8_t *px_map);

static lv_display_t *lvgl_init_driver(uint32_t width, uint32_t heigth) {
  lv_display_t *disp = lv_display_create(width, heigth);
  if (!disp) {
    goto error;
  }

  dd_wvs75v2b_t dd;
  dd_error_t err;

  err = dd_wvs75v2b_init(&dd);
  DD_TRY(err);

  err = dd_wvs75v2b_set_up_gpio_dc(dd, "/dev/gpiochip8", 0);
  DD_TRY_CATCH(err, error_dd_cleanup);

  err = dd_wvs75v2b_set_up_gpio_rst(dd, "/dev/gpiochip2", 2);
  DD_TRY_CATCH(err, error_dd_cleanup);

  err = dd_wvs75v2b_set_up_gpio_bsy(dd, "/dev/gpiochip6", 3);
  DD_TRY_CATCH(err, error_dd_cleanup);

  err = dd_wvs75v2b_set_up_gpio_pwr(dd, "/dev/gpiochip0", 4);
  DD_TRY_CATCH(err, error_dd_cleanup);

  err = dd_wvs75v2b_set_up_spi_master(dd, "/dev/spidev0.0");
  DD_TRY_CATCH(err, error_dd_cleanup);

  err = dd_wvs75v2b_ops_reset(dd);
  DD_TRY_CATCH(err, error_dd_cleanup);

  uint32_t buf_len = (width + heigth) * BYTES_PER_PIXEL;
  char *buf = mem_malloc(buf_len);

  lv_display_set_driver_data(disp, dd);
  lv_display_set_flush_cb(disp, lvgl_flush_dd_callback);
  lv_display_set_buffers(disp, (void *)buf, NULL, buf_len,
                         LV_DISPLAY_RENDER_MODE_FULL);

  return disp;

error_dd_cleanup:
  dd_wvs75v2b_destroy(&dd);
error:
  return NULL;
}

static cdk_error_t lvgl_convert_dd_error(dd_error_t err) {
  const char *buf = dd_error_get_msg(err);
  int code = dd_error_get_code(err);
  return cdk_errnof(code, "dd error - %.*s", EBOOK_READER_ERROR_FSTR_MAX, buf);
};

static void lvgl_flush_dd_callback(lv_display_t *display, const lv_area_t *area,
                                   uint8_t *px_map) {
  log_info("Flushing");
}

#else
static lv_display_t *lvgl_init_driver(uint32_t width, uint32_t heigth) {
  return NULL;
}
#endif // EBOOK_READER_X11_ENABLED
