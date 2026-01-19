#include <display_driver.h>
#include <stdint.h>
#include <stdio.h>

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
};

static int wvs7in5v2_color_format = LV_COLOR_FORMAT_I1;
static int wvs7in5v2_heigth = 800;
static int wvs7in5v2_width = 480;

static err_t ui_display_wvs7in5v2_render(void *, unsigned char *, uint32_t);
static void ui_display_wvs7in5v2_destroy(void *);
static void ui_display_wvs7in5v2_panic(void *);
static void ui_display_wvs7in5v2_flush_dd_callback(lv_display_t *,
                                                   const lv_area_t *,
                                                   uint8_t *);

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

  lv_display_t *disp = lv_display_create(wvs7in5v2_width, wvs7in5v2_heigth);
  if (!disp) {
    goto error_wvs_cleanup;
  }

  // 1 pixel per color and + colour palette size
  wvs->render_buf.len = (wvs7in5v2_width * wvs7in5v2_heigth / 8) + 8;
  wvs->render_buf.data = mem_malloc(wvs->render_buf.len);
  lv_display_set_color_format(disp, wvs7in5v2_color_format);
  lv_display_set_driver_data(disp, wvs);
  lv_display_set_flush_cb(disp, ui_display_wvs7in5v2_flush_dd_callback);
  lv_display_set_buffers(disp, wvs->render_buf.data, NULL, wvs->render_buf.len,
                         LV_DISPLAY_RENDER_MODE_FULL);

  err_o = ui_display_create(module, disp, ui, ui_display_wvs7in5v2_render, NULL,
                            ui_display_wvs7in5v2_destroy,
                            ui_display_wvs7in5v2_panic, wvs);
  ERR_TRY_CATCH(err_o, error_display_cleanup);

  wvs->owner = *module;

  return 0;

error_display_cleanup:
  lv_display_delete(disp);
error_wvs_cleanup:
  mem_free(wvs);
error_out:
  return err_o;
}

static void ui_display_wvs7in5v2_destroy(void *display) {
  wvs7in5v2_t wvs = display;
  mem_free(wvs->render_buf.data);
  mem_free(wvs);
}

static void ui_display_wvs7in5v2_flush_dd_callback(lv_display_t *display,
                                                   const lv_area_t *area,
                                                   uint8_t *px_map) {
  printf("wvs hardcoded len=%d\n", wvs7in5v2_width * wvs7in5v2_heigth / 8);
  printf("%d * %d / 8 = %d\n", area->x2 - area->x1, area->y2 - area->y1,
         (area->x2 - area->x1) * (area->y2 - area->y1) / 8);
  wvs7in5v2_t wvs = lv_display_get_driver_data(display);
  dd_error_t dd_err = dd_display_driver_write(
      wvs->dd, px_map + 8, wvs7in5v2_width * wvs7in5v2_heigth / 8);

  if (dd_err) {
    err_o = ERR_FROM_DD(dd_err);
    goto error_out;
  }

  return;

error_out:
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

#endif
