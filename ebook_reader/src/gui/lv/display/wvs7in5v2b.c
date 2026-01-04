#include <display_driver.h>
#include <stdint.h>
#include <stdio.h>

#include "draw/lv_draw_buf.h"
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
  lv_display_t *disp = lv_display_create(width,heigth);
  /* lv_display_t *disp = lv_display_create(heigth, width); */
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
  /* uint32_t stride  = (width + 7) / 8; */
  uint32_t buf_len = (width * heigth / 8) + 8; // 1 pixel per color and + colour palette size
  *driver = (struct LvglWvs7In5V2b){
      .dd = dd,
      .buf = {.data = mem_malloc(buf_len), .len = buf_len},
  };

  printf("buf_len: %d\n", buf_len);
  lv_display_set_color_format(disp, LV_COLOR_FORMAT_I1);
  lv_display_set_driver_data(disp, out);
  lv_display_set_flush_cb(disp, wvs7in5v2b_flush_dd_callback);
  lv_display_set_buffers(disp, driver->buf.data, NULL, driver->buf.len,
                         LV_DISPLAY_RENDER_MODE_FULL);

printf("disp cf: %d\n", lv_display_get_color_format(disp));
printf("bpp: %d\n", lv_color_format_get_bpp(lv_display_get_color_format(disp)));


 /* printf("draw size: %d\n", lv_draw_buf_get_size(width, heigth, LV_COLOR_FORMAT_I1)); */
/* ) */
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

  struct LvglDisplayDriver *out = lv_display_get_driver_data(display);
  struct LvglWvs7In5V2b *driver = out->data;
  dd_image_t img;
  printf("area->x2=%d, area->x1=%d, area->size=%d\n", area->x2, area->x1,
         lv_area_get_size(area));
  dd_error_t err =
      dd_image_init(&img, px_map, 48000,
                    (struct dd_ImagePoint){.x = 479, .y = 799});
  if (err) {
    goto out;
  }
  err = dd_wvs75v2b_ops_power_on(driver->dd);
  if (err) {
    goto img_cleanup;
  }

  err = dd_wvs75v2b_ops_display_full(driver->dd, img);
  if (err) {
    goto img_cleanup;
  }

  err = dd_wvs75v2b_ops_power_off(driver->dd);
  if (err) {
    goto img_cleanup;
  }
  

  (void)display;
  (void)area;
  (void)px_map;
img_cleanup:
  dd_image_destroy(&img);
out:
  lv_display_flush_ready(display);

  if (err) {
    puts("DD ERRoR");
    char buf[4096];
    dd_error_dumps(err, sizeof(buf), buf);
    puts(buf);
  }
}

static void wvs7in5v2b_destroy(struct LvglDisplayDriver *out) {
  puts(__func__);
  if (!out || !out->data) {
    return;
  }

  struct LvglWvs7In5V2b *driver = out->data;
  dd_wvs75v2b_destroy(&driver->dd);
  mem_free(driver->buf.data);
  mem_free(driver);
  out->data = NULL;
}
