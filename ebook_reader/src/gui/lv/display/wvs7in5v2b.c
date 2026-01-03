#include <display_driver.h>
#include <stdint.h>

#include "gui/lv/display/driver.h"
#include "utils/error.h"
#include "utils/log.h"
#include "utils/memory.h"

#define DD_TRY_CATCH(err, label)                                               \
  if (err) {                                                                   \
    cdk_errno = wvs7in5v2b_convert_dd_error(err);                              \
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

static lv_display_t *wvs7in5v2b_init(uint32_t width, uint32_t heigth,
                                     struct LvglDisplayDriver *out);
static void wvs7in5v2b_destroy(struct LvglDisplayDriver *out);
static cdk_error_t wvs7in5v2b_convert_dd_error(dd_error_t err);
static void wvs7in5v2b_flush_dd_callback(lv_display_t *display,
                                         const lv_area_t *area,
                                         uint8_t *px_map);

bool lvgl_display_driver_wvs7in5v2b_probe(struct LvglDisplayDriver *out) {
  out->init = wvs7in5v2b_init;
  out->destroy = wvs7in5v2b_destroy;
  return true;
}

static lv_display_t *wvs7in5v2b_init(uint32_t width, uint32_t heigth,
                                     struct LvglDisplayDriver *out) {
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

  struct LvglWvs7In5V2b *driver = out->data =
      mem_malloc(sizeof(struct LvglWvs7In5V2b));
  uint32_t buf_len = (width + heigth) * BYTES_PER_PIXEL;
  *driver = (struct LvglWvs7In5V2b){
      .dd = dd,
      .buf = {.data = mem_malloc(buf_len), .len = buf_len},
  };

  lv_display_set_driver_data(disp, driver);
  lv_display_set_flush_cb(disp, wvs7in5v2b_flush_dd_callback);
  lv_display_set_buffers(disp, driver->buf.data, NULL, driver->buf.len,
                         LV_DISPLAY_RENDER_MODE_FULL);

  return disp;

error_dd_cleanup:
  dd_wvs75v2b_destroy(&dd);
error:
  return NULL;
}

static cdk_error_t wvs7in5v2b_convert_dd_error(dd_error_t err) {
  const char *buf = dd_error_get_msg(err);
  int code = dd_error_get_code(err);
  return cdk_errnof(code, "dd error - %.*s", EBOOK_READER_ERROR_FSTR_MAX, buf);
};

static void wvs7in5v2b_flush_dd_callback(lv_display_t *display,
                                         const lv_area_t *area,
                                         uint8_t *px_map) {
  log_info("Flushing");

  (void)display;
  (void)area;
  (void)px_map;

  lv_display_flush_ready(display);
}

static void wvs7in5v2b_destroy(struct LvglDisplayDriver *out) {

  if (!out) {
    return;
  }

  struct LvglWvs7In5V2b *driver = out->data;
  dd_wvs75v2b_destroy(&driver->dd);
}
