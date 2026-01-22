#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>

#include "display_driver.h"
#include "drivers/driver.h"
#include "gpio/gpio.h"
#include "spi/spi.h"
#include "utils/err.h"
#include "utils/graphic.h"
#include "utils/mem.h"
#include "utils/time.h"

#define DD_WVS75V2_WIDTH 800
#define DD_WVS75V2_HEIGTH 480
#define DD_WVS75V2_BUF_LEN DD_WVS75V2_HEIGTH *DD_WVS75V2_HEIGTH / 8

typedef struct dd_Wvs75v2 *dd_wvs75v2_t;

enum dd_Wvs75v2Bsy {
  dd_Wvs75v2Bsy_BUSY = 0,
  dd_Wvs75v2Bsy_IDLE,
};

enum dd_Wvs75v2Dc {
  dd_Wvs75v2Dc_CMD = 0,
  dd_Wvs75v2Dc_DATA,
};

enum dd_Wvs75v2Cmd {
  dd_Wvs75v2Cmd_PANEL_SETTING = 0x00,
  dd_Wvs75v2Cmd_POWER_SETTING = 0x01,
  dd_Wvs75v2Cmd_POWER_OFF = 0x02,
  dd_Wvs75v2Cmd_POWER_OFF_SEQUENCE_SETTING = 0x03,
  dd_Wvs75v2Cmd_POWER_ON = 0x04,
  dd_Wvs75v2Cmd_POWER_ON_MEASURE = 0x05,
  dd_Wvs75v2Cmd_BOOSTER_SOFT_START = 0x06,
  dd_Wvs75v2Cmd_DEEP_SLEEP = 0x07,

  dd_Wvs75v2Cmd_START_TRANSMISSION1 = 0x10,
  dd_Wvs75v2Cmd_DATA_STOP = 0x11,
  dd_Wvs75v2Cmd_DISPLAY_REFRESH = 0x12,
  dd_Wvs75v2Cmd_START_TRANSMISSION2 = 0x13,

  dd_Wvs75v2Cmd_LUT_OPT = 0x15,

  dd_Wvs75v2Cmd_PLL_CONTROL = 0x30,

  dd_Wvs75v2Cmd_TEMPERATURE_CALIBRATION = 0x41,

  dd_Wvs75v2Cmd_VCOM_AND_DATA_INTERVAL_SETTING = 0x50,
  dd_Wvs75v2Cmd_TCON_SETTING = 0x60,
  dd_Wvs75v2Cmd_RESOLUTION_SETTING = 0x61,

  dd_Wvs75v2Cmd_VCM_DC_SETTING = 0x82,

  dd_Wvs75v2Cmd_PARTIAL_WINDOW = 0x90,
  dd_Wvs75v2Cmd_PARTIAL_IN = 0x91,
  dd_Wvs75v2Cmd_PARTIAL_OUT = 0x92,

  dd_Wvs75v2Cmd_CASCADE_SETTING = 0xe0,

  dd_Wvs75v2Cmd_FLASH_MODE = 0xe5,
};

struct dd_Wvs75v2 {
  // GPIO
  struct dd_Gpio gpio;
  struct dd_GpioPin *dc;
  struct dd_GpioPin *rst;
  struct dd_GpioPin *bsy;
  struct dd_GpioPin *pwr;

  // SPI
  struct dd_Spi spi;

  // Settings
  bool is_rotated;
  unsigned char *rotation_buf;
};

static dd_error_t dd_driver_wvs75v2_write_part(void *, unsigned char *, int,
                                               int, int, int, int);
static dd_error_t dd_driver_wvs75v2_write_fast(void *, unsigned char *, int);
static dd_error_t dd_driver_wvs75v2_write(void *, unsigned char *, int);
static dd_error_t dd_driver_wvs75v2_clear(void *, bool);
static void dd_driver_wvs75v2_remove(void *);
static dd_error_t dd_driver_wvs75v2_ops_reset(dd_wvs75v2_t);
static dd_error_t dd_driver_wvs75v2_ops_power_on(dd_wvs75v2_t);
static dd_error_t dd_driver_wvs75v2_ops_power_off(dd_wvs75v2_t);
static dd_error_t dd_driver_wvs75v2_ops_clear(dd_wvs75v2_t, bool);
static dd_error_t dd_driver_wvs75v2_ops_display_full(dd_wvs75v2_t,
                                                     unsigned char *, int);
static dd_error_t dd_driver_wvs75v2_ops_display_partial(dd_wvs75v2_t,
                                                        unsigned char *, int,
                                                        int, int, int, int);

dd_error_t dd_driver_wvs7in5v2_init(dd_display_driver_t out, void *config) {
  dd_wvs75v2_t wvs = dd_malloc(sizeof(struct dd_Wvs75v2));
  *wvs = (struct dd_Wvs75v2){0};

  struct dd_Wvs75V2Config *conf = config;

  int stride = 800 / 8;
  int x = 800;
  int y = 480;
  if (conf->rotate) {
    stride = 480 / 8;
    x = 480;
    y = 800;
    wvs->is_rotated = true;
  }

  dd_errno = dd_gpio_init(&wvs->gpio);
  DD_TRY(dd_errno);

  dd_errno = dd_spi_init(conf->spi.spidev_path, &wvs->spi);
  DD_TRY_CATCH(dd_errno, error_gpio_cleanup);

  dd_errno = dd_gpio_add_pin(conf->dc.gpio_chip_path, conf->dc.pin_no, &wvs->dc,
                             &wvs->gpio);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_errno = dd_gpio_set_pin_output(wvs->dc, true);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);

  dd_errno = dd_gpio_add_pin(conf->rst.gpio_chip_path, conf->rst.pin_no,
                             &wvs->rst, &wvs->gpio);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_errno = dd_gpio_set_pin_output(wvs->rst, false);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);

  dd_errno = dd_gpio_add_pin(conf->bsy.gpio_chip_path, conf->bsy.pin_no,
                             &wvs->bsy, &wvs->gpio);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_errno = dd_gpio_set_pin_input(wvs->bsy);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);

  dd_errno = dd_gpio_add_pin(conf->pwr.gpio_chip_path, conf->pwr.pin_no,
                             &wvs->pwr, &wvs->gpio);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_errno = dd_gpio_set_pin_output(wvs->pwr, true);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);

  *out = (struct dd_DisplayDriver){
    .write_fast = dd_driver_wvs75v2_write_fast,
    .write_part = dd_driver_wvs75v2_write_part,
    .destroy = dd_driver_wvs75v2_remove,
    .write = dd_driver_wvs75v2_write,
      .clear = dd_driver_wvs75v2_clear,
      .driver_data = wvs,
      .stride = stride,
      .x = x,
      .y = y,
  };

  return 0;

error_dd_cleanup:
  dd_driver_wvs75v2_remove(wvs);
error_gpio_cleanup:
  dd_gpio_destroy(&wvs->gpio);
error_out:
  dd_free(wvs);
  return dd_errno;
};

static dd_error_t dd_driver_wvs75v2_clear(void *driver, bool is_white) {
  dd_wvs75v2_t wvs = driver;
  dd_driver_wvs75v2_ops_power_on(wvs);
  DD_TRY(dd_errno);

  dd_errno = dd_driver_wvs75v2_ops_clear(wvs, is_white);
  DD_TRY_CATCH(dd_errno, error_wvs75v2_cleanup);

  dd_driver_wvs75v2_ops_power_off(wvs);
  DD_TRY(dd_errno);

  return 0;

error_wvs75v2_cleanup:
  dd_driver_wvs75v2_ops_power_off(wvs);
error_out:
  return dd_errno;
}

static dd_error_t dd_driver_wvs75v2_write(void *dd, unsigned char *buf,
                                          int buf_len) {
  dd_wvs75v2_t wvs = dd;
  dd_driver_wvs75v2_ops_power_on(wvs);
  DD_TRY(dd_errno);

  dd_errno = dd_driver_wvs75v2_ops_display_full(wvs, buf, buf_len);
  DD_TRY_CATCH(dd_errno, error_wvs75v2_cleanup);

  dd_driver_wvs75v2_ops_power_off(wvs);
  DD_TRY(dd_errno);

  return 0;

error_wvs75v2_cleanup:
  dd_driver_wvs75v2_ops_power_off(wvs);
error_out:
  return dd_errno;
}

static void dd_wvs75v2_wait(struct dd_Wvs75v2 *display) {
  /* puts("Busy waiting"); */
  while (dd_gpio_read_pin(display->bsy, &display->gpio) != dd_Wvs75v2Bsy_IDLE) {
    dd_sleep_ms(10);
  }
  /* puts("Waiting done"); */
}

static void dd_driver_wvs75v2_remove(void *dd) {
  struct dd_Wvs75v2 *wvs = dd;

  if (wvs->pwr) {
    dd_gpio_set_pin(0, wvs->pwr, &wvs->gpio);
  }
  if (wvs->rst) {
    dd_gpio_set_pin(0, wvs->rst, &wvs->gpio);
  }
  if (wvs->dc) {
    dd_gpio_set_pin(0, wvs->dc, &wvs->gpio);
  }

  dd_spi_destroy(&wvs->spi);
  dd_gpio_destroy(&wvs->gpio);

  dd_free(wvs);
}

static dd_error_t dd_driver_wvs75v2_ops_reset(dd_wvs75v2_t dd) {
  if (dd_gpio_read_pin(dd->pwr, &dd->gpio) != 1) {
    dd_errno = dd_gpio_set_pin(1, dd->pwr, &dd->gpio);
    DD_TRY(dd_errno);
    dd_sleep_ms(200);
  }

  dd_errno = dd_gpio_set_pin(0, dd->rst, &dd->gpio);
  DD_TRY(dd_errno);
  dd_sleep_ms(200);

  dd_errno = dd_gpio_set_pin(1, dd->rst, &dd->gpio);
  DD_TRY_CATCH(dd_errno, error_rst_cleanup);
  dd_sleep_ms(10);

  dd_errno = dd_gpio_set_pin(0, dd->rst, &dd->gpio);
  DD_TRY_CATCH(dd_errno, error_rst_cleanup);
  dd_sleep_ms(200);

  dd_wvs75v2_wait(dd); // Give chip time to reset itself

  return 0;

error_rst_cleanup:
  dd_gpio_set_pin(0, dd->rst, &dd->gpio);
error_out:
  return dd_errno;
};

static dd_error_t dd_wvs75v2_send_cmd(struct dd_Wvs75v2 *dd, uint8_t cmd) {
  dd_errno = dd_gpio_set_pin(dd_Wvs75v2Dc_CMD, dd->dc, &dd->gpio);
  DD_TRY(dd_errno);

  dd_errno = dd_spi_send_byte(cmd, &dd->spi);
  DD_TRY(dd_errno);

  return 0;

error_out:
  return dd_errno;
}

static dd_error_t dd_wvs75v2_send_data(struct dd_Wvs75v2 *dd, uint8_t *data,
                                       uint32_t len) {
  dd_errno = dd_gpio_set_pin(dd_Wvs75v2Dc_DATA, dd->dc, &dd->gpio);
  DD_TRY(dd_errno);

  dd_errno = dd_spi_send_bytes(data, len, &dd->spi);
  DD_TRY(dd_errno);

  return 0;

error_out:
  return dd_errno;
}

static dd_error_t dd_driver_wvs75v2_ops_power_on(dd_wvs75v2_t dd) {
  if (dd_gpio_read_pin(dd->pwr, &dd->gpio) != 1) {
    dd_errno = dd_gpio_set_pin(1, dd->pwr, &dd->gpio);
    DD_TRY(dd_errno);
    dd_sleep_ms(200);
  }

  dd_errno = dd_gpio_set_pin(1, dd->pwr, &dd->gpio);
  DD_TRY(dd_errno);
  dd_sleep_ms(200);

  dd_errno = dd_driver_wvs75v2_ops_reset(dd);
  DD_TRY(dd_errno);

  dd_errno = dd_wvs75v2_send_cmd(dd, dd_Wvs75v2Cmd_POWER_SETTING);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_errno = dd_wvs75v2_send_data(dd,
                                  (uint8_t[]){
                                      0x07,
                                      0x07,
                                      0x3f,
                                      0x3f,
                                  },
                                  4);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);

  dd_errno = dd_wvs75v2_send_cmd(dd, dd_Wvs75v2Cmd_BOOSTER_SOFT_START);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  // I'm not sure what this part does but it is in mainline driver
  dd_errno = dd_wvs75v2_send_data(dd,
                                  (uint8_t[]){
                                      0x17,
                                      0x17,
                                      0x28,
                                      0x17,
                                  },
                                  4);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);

  dd_errno = dd_wvs75v2_send_cmd(dd, dd_Wvs75v2Cmd_POWER_ON);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_sleep_ms(100);
  dd_wvs75v2_wait(dd);

  dd_errno = dd_wvs75v2_send_cmd(dd, dd_Wvs75v2Cmd_PANEL_SETTING);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_errno = dd_wvs75v2_send_data(dd,
                                  (uint8_t[]){
                                      0x1F,
                                  },
                                  1);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);

  dd_errno = dd_wvs75v2_send_cmd(dd, dd_Wvs75v2Cmd_RESOLUTION_SETTING);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_errno = dd_wvs75v2_send_data(dd,
                                  (uint8_t[]){
                                      0x03,
                                      0x20,
                                      0x01,
                                      0xE0,
                                  },
                                  4);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);

  dd_errno = dd_wvs75v2_send_cmd(dd, dd_Wvs75v2Cmd_LUT_OPT);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_errno = dd_wvs75v2_send_data(dd,
                                  (uint8_t[]){
                                      0x00,
                                  },
                                  1);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);

  dd_errno =
      dd_wvs75v2_send_cmd(dd, dd_Wvs75v2Cmd_VCOM_AND_DATA_INTERVAL_SETTING);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_errno = dd_wvs75v2_send_data(dd,
                                  (uint8_t[]){
                                      0x10,
                                      0x07,
                                  },
                                  2);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);

  dd_errno = dd_wvs75v2_send_cmd(dd, dd_Wvs75v2Cmd_TCON_SETTING);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_errno = dd_wvs75v2_send_data(dd,
                                  (uint8_t[]){
                                      0x22,
                                  },
                                  1);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);

  dd_wvs75v2_wait(dd);

  return 0;

error_dd_cleanup:
  dd_driver_wvs75v2_ops_reset(dd);
error_out:
  return dd_errno;
}

static dd_error_t dd_driver_wvs75v2_ops_clear(dd_wvs75v2_t dd, bool white) {
  dd_errno = dd_wvs75v2_send_cmd(dd, dd_Wvs75v2Cmd_START_TRANSMISSION1);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  for (int i = 0; i < DD_WVS75V2_HEIGTH * (DD_WVS75V2_WIDTH / 8); i++) {
    dd_errno = dd_wvs75v2_send_data(dd,
                                    (uint8_t[]){
                                        white ? 0x00 : 0xFF,
                                    },
                                    1);
    DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  }
  dd_wvs75v2_wait(dd);

  dd_errno = dd_wvs75v2_send_cmd(dd, dd_Wvs75v2Cmd_START_TRANSMISSION2);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  for (int i = 0; i < DD_WVS75V2_HEIGTH * (DD_WVS75V2_WIDTH / 8); i++) {
    dd_errno = dd_wvs75v2_send_data(dd,
                                    (uint8_t[]){
                                        white ? 0x00 : 0xFF,
                                    },
                                    1);
    DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  }
  dd_wvs75v2_wait(dd);

  dd_errno = dd_wvs75v2_send_cmd(dd, dd_Wvs75v2Cmd_DISPLAY_REFRESH);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_sleep_ms(100);
  dd_wvs75v2_wait(dd);

  return 0;

error_dd_cleanup:
  dd_driver_wvs75v2_ops_reset(dd);
  return dd_errno;
}

static dd_error_t dd_driver_wvs75v2_ops_power_off(dd_wvs75v2_t dd) {
  dd_errno = dd_wvs75v2_send_cmd(dd, dd_Wvs75v2Cmd_POWER_OFF);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_wvs75v2_wait(dd);

  dd_errno = dd_wvs75v2_send_cmd(dd, dd_Wvs75v2Cmd_DEEP_SLEEP);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_errno = dd_wvs75v2_send_data(dd, (uint8_t[]){0xA5}, 1);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_sleep_ms(300); // In deep sleep busy does not work so we need to estimate
                    // time required display to perform deep sleep operation.

  return 0;

error_dd_cleanup:
  dd_driver_wvs75v2_ops_reset(dd);
  return dd_errno;
}

static unsigned char *dd_wvs75v2_rotate(dd_wvs75v2_t dd, int width, int heigth,
                                        unsigned char *buf, uint32_t buf_len) {
  int dst_i = 0;
  int v;

  unsigned char *dst = dd_malloc(buf_len);
  for (int x = width - 1; x >= 0; --x) {
    for (int y = 0; y < heigth; ++y) {
      v = dd_graphic_get_pixel(x, y, width, buf, buf_len);
      dd_graphic_set_bit(dst_i++, v, dst, buf_len);
    }
  }

  return dst;
}

static dd_error_t dd_driver_wvs75v2_ops_display_full(dd_wvs75v2_t dd,
                                                     unsigned char *buf,
                                                     int buf_len) {
  if (dd->is_rotated) {
    buf = dd_wvs75v2_rotate(dd, DD_WVS75V2_HEIGTH, DD_WVS75V2_WIDTH, buf,
                            buf_len);
  }

  dd_errno = dd_wvs75v2_send_cmd(dd, dd_Wvs75v2Cmd_START_TRANSMISSION1);
  DD_TRY_CATCH(dd_errno, out);
  uint8_t chunk[1024] = {0};
  for (int i = 0; i < buf_len; i += sizeof(chunk)) {
    int chunk_size = sizeof(chunk);
    if (i + chunk_size > buf_len) {
      chunk_size = buf_len - i;
    }

    dd_errno = dd_wvs75v2_send_data(dd, chunk, chunk_size);
    DD_TRY_CATCH(dd_errno, out);
  }
  dd_wvs75v2_wait(dd);

  dd_errno = dd_wvs75v2_send_cmd(dd, dd_Wvs75v2Cmd_START_TRANSMISSION2);
  DD_TRY_CATCH(dd_errno, out);

  for (int i = 0; i < buf_len; i += sizeof(chunk)) {
    int chunk_size = sizeof(chunk);
    if (i + chunk_size > buf_len) {
      chunk_size = buf_len - i;
    }

    for (int chunk_i = 0; chunk_i < chunk_size; chunk_i++) {
      chunk[chunk_i] = ~(*(buf + i + chunk_i));
    }

    dd_errno = dd_wvs75v2_send_data(dd, chunk, chunk_size);
    DD_TRY_CATCH(dd_errno, out);
  }

  dd_wvs75v2_wait(dd);

  dd_errno = dd_wvs75v2_send_cmd(dd, dd_Wvs75v2Cmd_DISPLAY_REFRESH);
  DD_TRY_CATCH(dd_errno, out);
  dd_wvs75v2_wait(dd);

out:
  if (dd->is_rotated) {
    dd_free(buf);
  }
  if (dd_errno) {
    dd_driver_wvs75v2_ops_reset(dd);
  }
  return dd_errno;
};

static dd_error_t dd_driver_wvs75v2_ops_power_on_part(dd_wvs75v2_t dd) {
  if (dd_gpio_read_pin(dd->pwr, &dd->gpio) != 1) {
    dd_errno = dd_gpio_set_pin(1, dd->pwr, &dd->gpio);
    DD_TRY(dd_errno);
    dd_sleep_ms(200);
  }

  dd_errno = dd_gpio_set_pin(1, dd->pwr, &dd->gpio);
  DD_TRY(dd_errno);
  dd_sleep_ms(200);

  dd_errno = dd_driver_wvs75v2_ops_reset(dd);
  DD_TRY(dd_errno);

  dd_errno = dd_wvs75v2_send_cmd(dd, dd_Wvs75v2Cmd_PANEL_SETTING);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_errno = dd_wvs75v2_send_data(dd,
                                  (uint8_t[]){
                                      0x1F,
                                  },
                                  1);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);

  dd_errno = dd_wvs75v2_send_cmd(dd, dd_Wvs75v2Cmd_POWER_ON);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_sleep_ms(100);
  dd_wvs75v2_wait(dd);

  dd_errno = dd_wvs75v2_send_cmd(dd, dd_Wvs75v2Cmd_CASCADE_SETTING);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_errno = dd_wvs75v2_send_data(dd,
                                  (uint8_t[]){
                                      0x02,
                                  },
                                  1);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);

  dd_errno = dd_wvs75v2_send_cmd(dd, dd_Wvs75v2Cmd_FLASH_MODE);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_errno = dd_wvs75v2_send_data(dd,
                                  (uint8_t[]){
                                      0x6e,
                                  },
                                  1);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);

  dd_wvs75v2_wait(dd);

  return 0;

error_dd_cleanup:
  dd_driver_wvs75v2_ops_reset(dd);
error_out:
  return dd_errno;
}

static dd_error_t dd_driver_wvs75v2_write_part(void *dd, unsigned char *buf,
                                               int buf_len, int x1, int x2,
                                               int y1, int y2) {
  dd_wvs75v2_t wvs = dd;

  dd_driver_wvs75v2_ops_power_on_part(wvs);
  DD_TRY(dd_errno);
  dd_errno =
      dd_driver_wvs75v2_ops_display_partial(wvs, buf, buf_len, x1, x2, y1, y2);
  DD_TRY_CATCH(dd_errno, error_wvs75v2_cleanup);

  dd_driver_wvs75v2_ops_power_off(wvs);
  DD_TRY(dd_errno);

  return 0;

error_wvs75v2_cleanup:
  dd_driver_wvs75v2_ops_power_off(wvs);
error_out:
  return dd_errno;
}

/**
   @todo Rotation out of the box does not work with partial. We need to adjust
         rotate func to use it in the app. However i noticed during tests that
         partial leaves a lot of shadows, it may be the case that shadowing
         in such big degree disqualifies partial usage in this display.
  */
static dd_error_t dd_driver_wvs75v2_ops_display_partial(dd_wvs75v2_t dd,
                                                        unsigned char *buf,
                                                        int buf_len, int x1,
                                                        int x2, int y1,
                                                        int y2) {
  puts(__func__);

  if (dd->is_rotated) {
    dd_errno =
        dd_errnos(EINVAL, "Rotation is not supported in partial display");
    goto error_out;
  }

  dd_errno =
      dd_wvs75v2_send_cmd(dd, dd_Wvs75v2Cmd_VCOM_AND_DATA_INTERVAL_SETTING);
  DD_TRY(dd_errno);
  dd_errno = dd_wvs75v2_send_data(dd,
                                  (uint8_t[]){
                                      0xA9,
                                      0x07,
                                  },
                                  2);
  DD_TRY(dd_errno);

  dd_errno = dd_wvs75v2_send_cmd(dd, dd_Wvs75v2Cmd_PARTIAL_IN);
  DD_TRY(dd_errno);

  dd_errno = dd_wvs75v2_send_cmd(dd, dd_Wvs75v2Cmd_PARTIAL_WINDOW);
  DD_TRY(dd_errno);

  /**
     Not all pictures are 8bit aligned, think about 150x100 image it can't be
     aligned to byte without changing img content. If we change 150 to 144
     we need to repack bits as if the image were actually 144 bits wide.
     It is actual problem for this display but it is not problem for lvgl as
     lvgl respect byte stride.
  */
  dd_errno = dd_wvs75v2_send_data(dd,
                                  (uint8_t[]){
                                      x1 / 256,
                                      x1 % 256,
                                      (x2 - 1) / 256,
                                      (x2 - 1) % 256,
                                      y1 / 256,
                                      y1 % 256,
                                      (y2 - 1) / 256,
                                      (y2 - 1) % 256,
                                      0x01,
                                  },
                                  9);
  DD_TRY(dd_errno);

  dd_errno = dd_wvs75v2_send_cmd(dd, dd_Wvs75v2Cmd_START_TRANSMISSION1);
  DD_TRY(dd_errno);

  for (int i = 0; i < buf_len; i++) {
    dd_errno = dd_wvs75v2_send_data(dd, (uint8_t[]){~buf[i]}, 1);
    DD_TRY(dd_errno);
  }

  dd_errno = dd_wvs75v2_send_cmd(dd, dd_Wvs75v2Cmd_START_TRANSMISSION2);
  DD_TRY(dd_errno);

  for (int i = 0; i < buf_len; i++) {
    dd_errno = dd_wvs75v2_send_data(dd, (uint8_t[]){buf[i]}, 1);
    DD_TRY(dd_errno);
  }
  dd_errno = dd_wvs75v2_send_cmd(dd, dd_Wvs75v2Cmd_DISPLAY_REFRESH);
  DD_TRY(dd_errno);
  dd_sleep_ms(100);
  dd_wvs75v2_wait(dd);

  return 0;

error_out:
  dd_driver_wvs75v2_ops_reset(dd);
  return dd_errno;
}

static dd_error_t dd_driver_wvs75v2_ops_power_on_fast(dd_wvs75v2_t dd) {
  if (dd_gpio_read_pin(dd->pwr, &dd->gpio) != 1) {
    dd_errno = dd_gpio_set_pin(1, dd->pwr, &dd->gpio);
    DD_TRY(dd_errno);
    dd_sleep_ms(200);
  }

  dd_errno = dd_gpio_set_pin(1, dd->pwr, &dd->gpio);
  DD_TRY(dd_errno);
  dd_sleep_ms(200);

  dd_errno = dd_driver_wvs75v2_ops_reset(dd);
  DD_TRY(dd_errno);

  dd_errno = dd_wvs75v2_send_cmd(dd, dd_Wvs75v2Cmd_PANEL_SETTING);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_errno = dd_wvs75v2_send_data(dd,
                                  (uint8_t[]){
                                      0x1F,
                                  },
                                  1);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);

  dd_errno =
      dd_wvs75v2_send_cmd(dd, dd_Wvs75v2Cmd_VCOM_AND_DATA_INTERVAL_SETTING);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_errno = dd_wvs75v2_send_data(dd,
                                  (uint8_t[]){
                                      0x10,
                                      0x07,
                                  },
                                  2);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);

  dd_errno = dd_wvs75v2_send_cmd(dd, dd_Wvs75v2Cmd_POWER_ON);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_sleep_ms(100);
  dd_wvs75v2_wait(dd);

  dd_errno = dd_wvs75v2_send_cmd(dd, dd_Wvs75v2Cmd_BOOSTER_SOFT_START);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  // I'm not sure what this part does but it is in mainline driver
  dd_errno = dd_wvs75v2_send_data(dd,
                                  (uint8_t[]){
                                      0x27,
                                      0x27,
                                      0x18,
                                      0x17,
                                  },
                                  4);

  dd_errno = dd_wvs75v2_send_cmd(dd, dd_Wvs75v2Cmd_CASCADE_SETTING);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_errno = dd_wvs75v2_send_data(dd,
                                  (uint8_t[]){
                                      0x02,
                                  },
                                  1);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);

  dd_errno = dd_wvs75v2_send_cmd(dd, dd_Wvs75v2Cmd_FLASH_MODE);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_errno = dd_wvs75v2_send_data(dd,
                                  (uint8_t[]){
                                      0x5A,
                                  },
                                  1);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);

  dd_wvs75v2_wait(dd);

  return 0;

error_dd_cleanup:
  dd_driver_wvs75v2_ops_reset(dd);
error_out:
  return dd_errno;
}

static dd_error_t dd_driver_wvs75v2_write_fast(void *dd, unsigned char *buf,
                                               int buf_len) {
  dd_wvs75v2_t wvs = dd;

  dd_driver_wvs75v2_ops_power_on_fast(wvs);
  DD_TRY(dd_errno);
  dd_errno = dd_driver_wvs75v2_ops_display_full(wvs, buf, buf_len);
  DD_TRY_CATCH(dd_errno, error_wvs75v2_cleanup);

  dd_driver_wvs75v2_ops_power_off(wvs);
  DD_TRY(dd_errno);

  return 0;

error_wvs75v2_cleanup:
  dd_driver_wvs75v2_ops_power_off(wvs);
error_out:
  return dd_errno;
}
