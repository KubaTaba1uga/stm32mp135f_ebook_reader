#include "display/lv_display.h"
#if !EBK_DISPLAY_WVS7IN5V2 && !EBK_DISPLAY_WVS7IN5V2B
#include <assert.h>
#include <display_driver.h>
#include <stdint.h>

#include "ui/display.h"

err_t ui_display_dd_init(ui_display_t __, ui_t ___) {
  return err_errnos(EINVAL, "Generic display driver is not supported!");
};

#else
#include <assert.h>
#include <display_driver.h>
#include <stdint.h>

#include "indev/lv_indev.h"
#include "ui/display.h"
#include "utils/log.h"
#include "utils/mem.h"
#include "utils/settings.h"

#define ERR_FROM_DD(err)                                                       \
  err_errnos(dd_error_get_code(err), dd_error_get_msg(err))

typedef struct UiDisplayDD *ui_display_dd_t;

struct UiDisplayDD {
  dd_display_driver_t dd;
  ui_display_t display;
  struct {
    unsigned char *buf;
    int len;
  } render;
};

static int ui_dd_color_format = LV_COLOR_FORMAT_I1;

static void ui_display_dd_flush_callback(lv_display_t *, const lv_area_t *,
                                         uint8_t *);
static void ui_display_dd_flush_callback_partial(lv_display_t *,
                                                 const lv_area_t *, uint8_t *);
static err_t ui_display_dd_render(void *, unsigned char *, uint32_t);
static void ui_display_dd_destroy(void *);
static void ui_display_dd_panic(void *);

err_t ui_display_dd_init(ui_display_t ui_display, ui_t ui) {
  ui_display_dd_t ui_dd;
  lv_display_t *lv_disp;
  lv_indev_t *lv_indev;
  lv_group_t *lv_group;
  dd_error_t dd_err;

  ui_dd = mem_malloc(sizeof(struct UiDisplayDD));
  *ui_dd = (struct UiDisplayDD){
      .display = ui_display,
  };

  switch (settings_display_model) {
  case DisplayModelEnum_WVS7IN5V2:
    dd_err = dd_display_driver_init(
        &ui_dd->dd, dd_DisplayDriverEnum_Wvs7in5V2,
        &(struct dd_Wvs75V2Config){
            .dc = {.gpio_chip_path = "/dev/gpiochip8", .pin_no = 0},
            .rst = {.gpio_chip_path = "/dev/gpiochip2", .pin_no = 2},
            .bsy = {.gpio_chip_path = "/dev/gpiochip6", .pin_no = 3},
            .pwr = {.gpio_chip_path = "/dev/gpiochip0", .pin_no = 4},
            .spi = {.spidev_path = "/dev/spidev0.0"},
            .rotate = true,
        });
    if (dd_err) {
      err_o = ERR_FROM_DD(dd_err);
      goto error_out;
    }
    break;
  case DisplayModelEnum_WVS7IN5V2B:
    dd_err = dd_display_driver_init(
        &ui_dd->dd, dd_DisplayDriverEnum_Wvs7in5V2b,
        &(struct dd_Wvs75V2bConfig){
            .dc = {.gpio_chip_path = "/dev/gpiochip8", .pin_no = 0},
            .rst = {.gpio_chip_path = "/dev/gpiochip2", .pin_no = 2},
            .bsy = {.gpio_chip_path = "/dev/gpiochip6", .pin_no = 3},
            .pwr = {.gpio_chip_path = "/dev/gpiochip0", .pin_no = 4},
            .spi = {.spidev_path = "/dev/spidev0.0"},
            .rotate = true,
        });
    if (dd_err) {
      err_o = ERR_FROM_DD(dd_err);
      goto error_out;
    }
    break;
  default:
    assert(false);
  }

  lv_disp = lv_display_create(dd_display_driver_get_x(ui_dd->dd),
                              dd_display_driver_get_y(ui_dd->dd));
  if (!lv_disp) {
    goto error_dd_cleanup;
  }

  lv_indev = lv_evdev_create(LV_INDEV_TYPE_KEYPAD, settings_input_path);
  lv_indev_set_display(lv_indev, lv_disp);
  lv_group = lv_group_create();
  lv_indev_set_group(lv_indev, lv_group);

  ui_dd->render.len = (dd_display_driver_get_x(ui_dd->dd) *
                       dd_display_driver_get_y(ui_dd->dd) / 8) +
                      8;
  ui_dd->render.buf = mem_malloc(ui_dd->render.len);

  lv_display_set_color_format(lv_disp, ui_dd_color_format);
  lv_display_set_driver_data(lv_disp, ui_dd);
  lv_display_set_flush_cb(lv_disp, ui_display_dd_flush_callback);
  lv_display_set_buffers(lv_disp, ui_dd->render.buf, NULL, ui_dd->render.len,
                         LV_DISPLAY_RENDER_MODE_FULL);
  *ui_display = (struct UiDisplay){
      .render = ui_display_dd_render,
      .destroy = ui_display_dd_destroy,
      .panic = ui_display_dd_panic,
      .lv_ingroup = lv_group,
      .lv_indv = lv_indev,
      .lv_disp = lv_disp,
      .owner = ui_dd,
      .ui = ui,
  };

  return 0;

error_dd_cleanup:
  dd_display_driver_destroy(&ui_dd->dd);
error_out:
  mem_free(ui_dd);
  return err_o;
}

static void ui_display_dd_flush_callback(lv_display_t *display,
                                         const lv_area_t *area,
                                         uint8_t *px_map) {
  ui_display_dd_t ui_dd = lv_display_get_driver_data(display);

  dd_error_t dd_err =
      dd_display_driver_write(ui_dd->dd, px_map + 8,
                              dd_display_driver_get_x(ui_dd->dd) *
                                  dd_display_driver_get_y(ui_dd->dd) / 8);
  if (dd_err) {
    err_o = ERR_FROM_DD(dd_err);
    goto error_out;
  }

  lv_display_flush_ready(display);
  return;

error_out:
  lv_display_flush_ready(display);
  log_error(err_o);
}

static void ui_display_dd_destroy(void *display) {
  ui_display_dd_t ui_dd = display;
  dd_display_driver_destroy(&ui_dd->dd);
  mem_free(ui_dd->render.buf);
  mem_free(ui_dd);
}

static err_t ui_display_dd_render(void *display, unsigned char *buf,
                                  uint32_t len) {
  ui_display_dd_t ui_dd = display;
  dd_error_t dd_err = dd_display_driver_write(ui_dd->dd, buf, len);

  if (dd_err) {
    err_o = ERR_FROM_DD(dd_err);
    goto error_out;
  }

  return 0;

error_out:
  return err_o;
}

static void ui_display_dd_panic(void *display) {
  ui_display_dd_t ui_dd = display;
  dd_display_driver_destroy(&ui_dd->dd);
}

#endif
