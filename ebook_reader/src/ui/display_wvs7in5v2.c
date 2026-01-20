#include <display_driver.h>
#include <stdint.h>
#include <stdio.h>

#include "drivers/evdev/lv_evdev.h"
#include "indev/lv_indev.h"
#include "ui/display.h"
#include "ui/ui.h"
#include "utils/err.h"
#include "utils/log.h"
#include "utils/mem.h"

#if !EBK_DISPLAY_WVS7IN5V2
err_t ui_display_wvs7in5v2_create(ui_display_t __, ui_t ___) {
  return err_errnos(EINVAL, "WVS7IN5V2 is not supported!");
};
#else

#define ERR_FROM_DD(err)                                                       \
  err_errnos(dd_error_get_code(err), dd_error_get_msg(err))

typedef struct UiDisplayWvs7in5V2 *wvs7in5v2_t;

struct UiDisplayWvs7in5V2 {
  dd_display_driver_t dd;
  ui_display_t owner;
  struct {
    unsigned char *data;
    uint32_t len;
  } render_buf;
  lv_group_t * wx_group;
};

static int wvs7in5v2_color_format = LV_COLOR_FORMAT_I1;
static err_t ui_display_wvs7in5v2_render(void *, unsigned char *, uint32_t);
static void ui_display_wvs7in5v2_destroy(void *);
static void ui_display_wvs7in5v2_panic(void *);
static void ui_display_wvs7in5v2_flush_dd_callback(lv_display_t *,
                                                   const lv_area_t *,
                                                   uint8_t *);
/* static void discovery_cb(lv_indev_t *indev, lv_evdev_type_t type, */
/*                          void *user_data) { */
/*   LV_LOG_USER("new '%s' device discovered", type == LV_EVDEV_TYPE_REL   ? "REL" */
/*                                             : type == LV_EVDEV_TYPE_ABS ? "ABS" */
/*                                             : type == LV_EVDEV_TYPE_KEY */
/*                                                 ? "KEY" */
/*                                                 : "unknown"); */

/*   if (type == LV_EVDEV_TYPE_REL) { */
/*     lv_evdev_discovery_stop(); */
/*   } */
/* } */

err_t ui_display_wvs7in5v2_create(ui_display_t *module, ui_t ui) {
  puts(__func__);

  wvs7in5v2_t wvs = mem_malloc(sizeof(struct UiDisplayWvs7in5V2));
  *wvs = (struct UiDisplayWvs7in5V2){0};

  dd_error_t err = dd_display_driver_init(
      &wvs->dd, dd_DisplayDriverEnum_Wvs7in5V2,
      &(struct dd_Wvs75V2Config){
          .dc = {.gpio_chip_path = "/dev/gpiochip8", .pin_no = 0},
          .rst = {.gpio_chip_path = "/dev/gpiochip2", .pin_no = 2},
          .bsy = {.gpio_chip_path = "/dev/gpiochip6", .pin_no = 3},
          .pwr = {.gpio_chip_path = "/dev/gpiochip0", .pin_no = 4},
          .spi = {.spidev_path = "/dev/spidev0.0"},
          .rotate = true,
      });
  if (err) {
    err_o = ERR_FROM_DD(err);
    goto error_out;
  }

  lv_display_t *disp = lv_display_create(dd_display_driver_get_x(wvs->dd),
                                         dd_display_driver_get_y(wvs->dd));
  if (!disp) {
    goto error_wvs_cleanup;
  }

  lv_indev_t *indev = lv_evdev_create(LV_INDEV_TYPE_KEYPAD,
  settings_input_path);
  lv_indev_set_display(indev, disp);
  lv_group_t * wx_group = wvs->wx_group = lv_group_create();
  lv_indev_set_group(indev, wx_group);

  // 1 pixel per color and + colour palette size
  wvs->render_buf.len = (dd_display_driver_get_x(wvs->dd) *
                         dd_display_driver_get_y(wvs->dd) / 8) +
                        8;
  wvs->render_buf.data = mem_malloc(wvs->render_buf.len);
  lv_display_set_color_format(disp, wvs7in5v2_color_format);
  lv_display_set_driver_data(disp, wvs);
  lv_display_set_flush_cb(disp, ui_display_wvs7in5v2_flush_dd_callback);
  lv_display_set_buffers(disp, wvs->render_buf.data, NULL, wvs->render_buf.len,
                         LV_DISPLAY_RENDER_MODE_FULL);

  err_o = ui_display_create(module, disp, ui, ui_display_wvs7in5v2_render, NULL,
                            ui_display_wvs7in5v2_destroy,
                            ui_display_wvs7in5v2_panic, NULL, wvs);
  ERR_TRY_CATCH(err_o, error_display_cleanup);

  wvs->owner = *module;

  return 0;

error_display_cleanup:
  lv_display_delete(disp);
  lv_group_delete(wx_group);
error_wvs_cleanup:
  mem_free(wvs);
error_out:
  return err_o;
}

static void ui_display_wvs7in5v2_destroy(void *display) {
  wvs7in5v2_t wvs = display;
  dd_display_driver_destroy(&wvs->dd);
  lv_group_delete(wvs->wx_group);
  mem_free(wvs->render_buf.data);
  mem_free(wvs);
}

static void ui_display_wvs7in5v2_flush_dd_callback(lv_display_t *display,
                                                   const lv_area_t *area,
                                                   uint8_t *px_map) {
  wvs7in5v2_t wvs = lv_display_get_driver_data(display);
  dd_error_t dd_err = dd_display_driver_write(
      wvs->dd, px_map + 8,
      dd_display_driver_get_x(wvs->dd) * dd_display_driver_get_y(wvs->dd) / 8);

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

static err_t ui_display_wvs7in5v2_render(void *display, unsigned char *buf,
                                         uint32_t len) {
  log_info("buf_len=%d\n", len);
  wvs7in5v2_t wvs = display;
  dd_error_t dd_err = dd_display_driver_write(wvs->dd, buf, len);

  if (dd_err) {
    err_o = ERR_FROM_DD(dd_err);
    goto error_out;
  }

  return 0;

error_out:
  return err_o;
}

static void ui_display_wvs7in5v2_panic(void *display) {
  wvs7in5v2_t wvs = display;
  dd_display_driver_destroy(&wvs->dd);
}

lv_group_t * ui_display_wvs7in5v2_get_input_group(ui_display_t display){
  wvs7in5v2_t wvs = lv_display_get_driver_data(ui_display_get_lv_obj(display));
  return wvs->wx_group;

};

#endif
