#include <display_driver.h>
#include <stdio.h>

#include "ui/display.h"
#include "utils/err.h"
#include "utils/log.h"
#include "utils/mem.h"

#if !EBK_DISPLAY_WVS7IN5V2B
err_t ui_display_wvs7in5v2b_init(ui_display_t __, ui_t ___) {
  return err_errnos(EINVAL, "Waveshare 7in5v2b is not supported!");
};
#else

#define ERR_FROM_DD(err)                                                       \
  err_errnos(dd_error_get_code(err), dd_error_get_msg(err))

typedef struct UiDisplayWvs7in5V2b *wvs7in5v2b_t;

struct UiDisplayWvs7in5V2b {
  dd_display_driver_t dd;
  struct {
    char *data;
    uint32_t len;
  } render_buf;
};

static void ui_display_wvs7in5v2b_destroy(ui_display_t);
static void ui_display_wvs7in5v2b_flush_dd_callback(lv_display_t *,
                                                    const lv_area_t *,
                                                    uint8_t *);

err_t ui_display_wvs7in5v2b_init(ui_display_t module) {
  puts(__func__);

  wvs7in5v2b_t wvs = mem_malloc(sizeof(struct UiDisplayWvs7in5V2b));
  *wvs = (struct UiDisplayWvs7in5V2b){0};

  dd_error_t err = dd_display_driver_init(
      &wvs->dd, dd_DisplayDriverEnum_Wvs7in5V2b,
      &(struct dd_Wvs75V2bConfig){
          .dc = {.gpio_chip_path = "/dev/gpiochip8", .pin_no = 0},
          .rst = {.gpio_chip_path = "/dev/gpiochip2", .pin_no = 2},
          .bsy = {.gpio_chip_path = "/dev/gpiochip6", .pin_no = 3},
          .pwr = {.gpio_chip_path = "/dev/gpiochip0", .pin_no = 4},
          .spi = {.spidev_path = "/dev/spidev0.0"},
      });
  if (err) {
    err_o = ERR_FROM_DD(err);
    goto error_out;
  }

  lv_display_t *disp = lv_display_create(480, 800);
  if (!disp) {
    goto error_wvs_cleanup;
  }

  // 1 pixel per color and + colour palette size
  wvs->render_buf.len = (480 * 800 / 8) + 8;
  lv_display_set_color_format(disp, LV_COLOR_FORMAT_I1);
  lv_display_set_driver_data(disp, module);
  lv_display_set_flush_cb(disp, ui_display_wvs7in5v2b_flush_dd_callback);
  lv_display_set_buffers(disp, wvs->render_buf.data, NULL, wvs->render_buf.len,
                         LV_DISPLAY_RENDER_MODE_FULL);

  module->destroy = ui_display_wvs7in5v2b_destroy;
  module->private = wvs;

  return 0;

error_wvs_cleanup:
  mem_free(wvs);
error_out:
  return err_o;
}

static void ui_display_wvs7in5v2b_destroy(ui_display_t module) {
  if (!module || !module->private) {
    return;
  }

  mem_free(module->private);
  module->private = NULL;
}

static void ui_display_wvs7in5v2b_flush_dd_callback(lv_display_t *display,
                                                    const lv_area_t *area,
                                                    uint8_t *px_map) {
  ui_display_t appdisplay = lv_display_get_driver_data(display);
  wvs7in5v2b_t wvs = appdisplay->private;
  dd_error_t dd_err = dd_display_driver_write(wvs->dd, px_map, 48000);

  if (dd_err) {
    err_o = ERR_FROM_DD(dd_err);
    goto error_out;
  }

  return;

error_out:
  log_error(err_o);
}

#endif
