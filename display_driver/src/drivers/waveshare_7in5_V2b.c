#include <stdint.h>
#include <stdio.h>

#include "display_driver.h"
#include "drivers/driver.h"
#include "gpio/gpio.h"
#include "spi/spi.h"
#include "utils/err.h"
#include "utils/mem.h"
#include "utils/time.h"
#include "utils/graphic.h"

#define DD_WVS75V2B_WIDTH 800
#define DD_WVS75V2B_HEIGTH 480
#define DD_WVS75V2B_BUF_LEN DD_WVS75V2B_HEIGTH *DD_WVS75V2B_HEIGTH / 8

enum dd_Wvs75V2bBsy {
  dd_Wvs75V2bBsy_BUSY = 0,
  dd_Wvs75V2bBsy_IDLE,
};

enum dd_Wvs75V2bDc {
  dd_Wvs75V2bDc_CMD = 0,
  dd_Wvs75V2bDc_DATA,
};

enum dd_Wvs75V2bCmd {
  dd_Wvs75V2bCmd_PANEL_SETTING = 0x00,
  dd_Wvs75V2bCmd_POWER_SETTING = 0x01,
  dd_Wvs75V2bCmd_POWER_OFF = 0x02,
  dd_Wvs75V2bCmd_POWER_OFF_SEQUENCE_SETTING = 0x03,
  dd_Wvs75V2bCmd_POWER_ON = 0x04,
  dd_Wvs75V2bCmd_POWER_ON_MEASURE = 0x05,
  dd_Wvs75V2bCmd_BOOSTER_SOFT_START = 0x06,
  dd_Wvs75V2bCmd_DEEP_SLEEP = 0x07,

  dd_Wvs75V2bCmd_START_TRANSMISSION1 = 0x10,
  dd_Wvs75V2bCmd_DATA_STOP = 0x11,
  dd_Wvs75V2bCmd_DISPLAY_REFRESH = 0x12,
  dd_Wvs75V2bCmd_START_TRANSMISSION2 = 0x13,
  dd_Wvs75V2bCmd_DUAL_SPI_MODE = 0x15,

  dd_Wvs75V2bCmd_VCOM_AND_DATA_INTERVAL_SETTING = 0x50,
  dd_Wvs75V2bCmd_TCON_SETTING = 0x60,
  dd_Wvs75V2bCmd_RESOLUTION_SETTING = 0x61,
};

struct dd_Wvs75V2b {
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
};

typedef struct dd_Wvs75V2b *dd_wvs75v2b_t;


static dd_error_t dd_wvs75v2b_write(void *, unsigned char *, int);
static dd_error_t dd_wvs75v2b_clear(void *, bool);
static void dd_wvs75v2b_remove(void *);
static dd_error_t dd_wvs75v2b_ops_reset(dd_wvs75v2b_t);
static dd_error_t dd_wvs75v2b_ops_power_on(dd_wvs75v2b_t);
static dd_error_t dd_wvs75v2b_ops_power_off(dd_wvs75v2b_t);
static dd_error_t dd_wvs75v2b_ops_clear(dd_wvs75v2b_t, bool);
static dd_error_t dd_wvs75v2b_ops_display_full(dd_wvs75v2b_t, unsigned char *,
                                               int);

dd_error_t dd_driver_wvs7in5v2b_init(dd_display_driver_t out, void *config) {
  struct dd_Wvs75V2b *wvs = dd_malloc(sizeof(struct dd_Wvs75V2b));
  *wvs = (struct dd_Wvs75V2b){0};

  struct dd_Wvs75V2bConfig *conf = config;
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

  dd_errno = dd_wvs75v2b_ops_reset(wvs);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);

  *out = (struct dd_DisplayDriver){
      .write = dd_wvs75v2b_write,
      .clear = dd_wvs75v2b_clear,
      .destroy = dd_wvs75v2b_remove,
      .driver_data = wvs,
      .stride = stride,
      .x = x,
      .y = y,
  };

  return 0;

error_dd_cleanup:
  dd_wvs75v2b_remove(wvs);
error_gpio_cleanup:
  dd_gpio_destroy(&wvs->gpio);
error_out:
  dd_free(wvs);
  return dd_errno;
};

// Clear screen.
static dd_error_t dd_wvs75v2b_clear(void *dd, bool white) {
  dd_wvs75v2b_t driver_data = dd;
  dd_wvs75v2b_ops_power_on(driver_data);
  DD_TRY(dd_errno);

  dd_errno = dd_wvs75v2b_ops_clear(driver_data, white);
  DD_TRY_CATCH(dd_errno, error_wvs75v2b_cleanup);

  dd_wvs75v2b_ops_power_off(driver_data);
  DD_TRY(dd_errno);

  return 0;

error_wvs75v2b_cleanup:
  dd_wvs75v2b_ops_power_off(driver_data);
error_out:
  return dd_errno;
};

// Write buf to screen with full refresh.
static dd_error_t dd_wvs75v2b_write(void *dd, unsigned char *buf,
                                    int buf_len) {
  dd_wvs75v2b_t driver_data = dd;
  dd_wvs75v2b_ops_power_on(driver_data);
  DD_TRY(dd_errno);

  dd_errno = dd_wvs75v2b_ops_display_full(driver_data, buf, buf_len);
  DD_TRY_CATCH(dd_errno, error_display_cleanup);

  dd_wvs75v2b_ops_power_off(driver_data);
  DD_TRY(dd_errno);

  return 0;
error_display_cleanup:
  dd_wvs75v2b_ops_power_off(driver_data);
error_out:
  return dd_errno;
};

static void dd_wvs75v2b_remove(void *dd) {
  dd_wvs75v2b_t driver_data = dd;

  if (driver_data->pwr) {
    dd_gpio_set_pin(0, driver_data->pwr, &driver_data->gpio);
  }
  if (driver_data->rst) {
    dd_gpio_set_pin(0, driver_data->rst, &driver_data->gpio);
  }
  if (driver_data->dc) {
    dd_gpio_set_pin(0, driver_data->dc, &driver_data->gpio);
  }

  dd_spi_destroy(&driver_data->spi);
  dd_gpio_destroy(&driver_data->gpio);

  dd_free(driver_data);
}

static dd_error_t dd_wvs75v2b_send_cmd(struct dd_Wvs75V2b *dd, uint8_t cmd) {
  dd_errno = dd_gpio_set_pin(dd_Wvs75V2bDc_CMD, dd->dc, &dd->gpio);
  DD_TRY(dd_errno);

  dd_errno = dd_spi_send_byte(cmd, &dd->spi);
  DD_TRY(dd_errno);

  return 0;

error_out:
  return dd_errno;
}

static dd_error_t dd_wvs75v2b_send_data(struct dd_Wvs75V2b *dd, uint8_t *data,
                                        int len) {
  dd_errno = dd_gpio_set_pin(dd_Wvs75V2bDc_DATA, dd->dc, &dd->gpio);
  DD_TRY(dd_errno);

  dd_errno = dd_spi_send_bytes(data, len, &dd->spi);
  DD_TRY(dd_errno);

  return 0;

error_out:
  return dd_errno;
}

static void dd_wvs75v2b_wait(struct dd_Wvs75V2b *display) {
  /* puts("Busy waiting"); */
  while (dd_gpio_read_pin(display->bsy, &display->gpio) !=
         dd_Wvs75V2bBsy_IDLE) {
    dd_sleep_ms(10);
  }
  /* puts("Waiting done"); */
}

static dd_error_t dd_wvs75v2b_ops_reset(dd_wvs75v2b_t dd) {
  puts(__func__);
  if (!dd || !dd->pwr || !dd->rst) {
    dd_errno =
        dd_errnos(EINVAL, "`dd`, `dd->pwr` and `dd->rst` cannot be NULL");
    goto error_out;
  }

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
  DD_TRY(dd_errno);
  dd_sleep_ms(200);

  dd_wvs75v2b_wait(dd); // Give chip time to reset itself

  return 0;

error_rst_cleanup:
  dd_gpio_set_pin(0, dd->rst, &dd->gpio);
error_out:
  return dd_errno;
};

static dd_error_t dd_wvs75v2b_ops_power_on(dd_wvs75v2b_t dd) {
  puts(__func__);
  if (!dd || !dd->dc || !dd->rst || !dd->bsy || !dd->pwr || !dd->spi.path) {
    dd_errno = dd_errnos(EINVAL, "`dd`, `dd->dc`, `dd->rst`, `dd->bsy`, "
                                 "`dd->pwr` and `dd->spi.path` cannot be NULL");
    goto error_out;
  }

  if (dd_gpio_read_pin(dd->pwr, &dd->gpio) != 1) {
    dd_errno = dd_gpio_set_pin(1, dd->pwr, &dd->gpio);
    DD_TRY(dd_errno);
    dd_sleep_ms(200);
  }

  dd_errno = dd_gpio_set_pin(1, dd->pwr, &dd->gpio);
  DD_TRY(dd_errno);
  dd_sleep_ms(200);

  dd_errno = dd_wvs75v2b_ops_reset(dd);
  DD_TRY(dd_errno);

  dd_errno = dd_wvs75v2b_send_cmd(dd, dd_Wvs75V2bCmd_POWER_SETTING);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_errno = dd_wvs75v2b_send_data(dd,
                                   (uint8_t[]){
                                       0x07, // LDO disabled, VDHR
                                       0x07, // VGH=20V,VGL=-20V
                                       0x3F, // VDH=15V
                                       0x3F, // VDL=-15V
                                   },
                                   4);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);

  dd_errno = dd_wvs75v2b_send_cmd(dd, dd_Wvs75V2bCmd_BOOSTER_SOFT_START);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  // I'm not sure what this part does but it is in mainline driver
  dd_errno = dd_wvs75v2b_send_data(dd,
                                   (uint8_t[]){
                                       0x17,
                                       0x17,
                                       0x28,
                                       0x17,
                                   },
                                   4);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);

  dd_errno = dd_wvs75v2b_send_cmd(dd, dd_Wvs75V2bCmd_POWER_ON);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_sleep_ms(100);

  dd_errno = dd_wvs75v2b_send_cmd(dd, dd_Wvs75V2bCmd_PANEL_SETTING);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_errno = dd_wvs75v2b_send_data(
      dd,
      (uint8_t[]){
          0x0F, // Gate scan direction UP, Source Shift Direction Rigth, Booster
                // on, Do not perform soft reset, Red/White/black mode,
                // White/black mode works very slow
      },
      1);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);

  dd_errno = dd_wvs75v2b_send_cmd(dd, dd_Wvs75V2bCmd_RESOLUTION_SETTING);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_errno = dd_wvs75v2b_send_data(dd,
                                   (uint8_t[]){
                                       0x03,
                                       0x20,
                                       0x01,
                                       0xE0,
                                   },
                                   4);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);

  dd_errno = dd_wvs75v2b_send_cmd(dd, dd_Wvs75V2bCmd_DUAL_SPI_MODE);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_errno = dd_wvs75v2b_send_data(dd, (uint8_t[]){0x00}, 1);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);

  dd_errno =
      dd_wvs75v2b_send_cmd(dd, dd_Wvs75V2bCmd_VCOM_AND_DATA_INTERVAL_SETTING);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_errno = dd_wvs75v2b_send_data(dd,
                                   (uint8_t[]){
                                       0x11,
                                       0x07,
                                   },
                                   2);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);

  dd_errno = dd_wvs75v2b_send_cmd(dd, dd_Wvs75V2bCmd_TCON_SETTING);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_errno = dd_wvs75v2b_send_data(dd, (uint8_t[]){0x22}, 1);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_wvs75v2b_wait(dd);

  return 0;

error_dd_cleanup:
  dd_wvs75v2b_ops_reset(dd);
error_out:
  return dd_errno;
}

static dd_error_t dd_wvs75v2b_ops_clear(dd_wvs75v2b_t dd, bool white) {
  puts(__func__);
  if (!dd || !dd->dc || !dd->rst || !dd->bsy || !dd->pwr || !dd->spi.path) {
    dd_errno = dd_errnos(EINVAL, "`dd`, `dd->dc`, `dd->rst`, `dd->bsy`, "
                                 "`dd->pwr` and `dd->spi.path` cannot be NULL");
    goto error_out;
  }

  dd_errno = dd_wvs75v2b_send_cmd(dd, dd_Wvs75V2bCmd_START_TRANSMISSION1);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  for (int i = 0; i < DD_WVS75V2B_HEIGTH * (DD_WVS75V2B_WIDTH / 8); i++) {
    dd_errno = dd_wvs75v2b_send_data(dd,
                                     (uint8_t[]){
                                         white ? 0xFF : 0x00,
                                     },
                                     1);
    DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  }
  dd_wvs75v2b_wait(dd);

  dd_errno = dd_wvs75v2b_send_cmd(dd, dd_Wvs75V2bCmd_START_TRANSMISSION2);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  for (int i = 0; i < DD_WVS75V2B_HEIGTH * (DD_WVS75V2B_WIDTH / 8); i++) {
    dd_errno = dd_wvs75v2b_send_data(dd, (uint8_t[]){0x00}, 1);
    DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  }
  dd_wvs75v2b_wait(dd);

  dd_errno = dd_wvs75v2b_send_cmd(dd, dd_Wvs75V2bCmd_DISPLAY_REFRESH);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_sleep_ms(100);
  dd_sleep_ms(1000);
  dd_wvs75v2b_wait(dd);

  return 0;

error_dd_cleanup:
  dd_wvs75v2b_ops_reset(dd);
error_out:
  return dd_errno;
}

static dd_error_t dd_wvs75v2b_ops_power_off(dd_wvs75v2b_t dd) {
  puts(__func__);
  if (!dd || !dd->dc || !dd->rst || !dd->bsy || !dd->pwr || !dd->spi.path) {
    dd_errno = dd_errnos(EINVAL, "`dd`, `dd->dc`, `dd->rst`, `dd->bsy`, "
                                 "`dd->pwr` and `dd->spi.path` cannot be NULL");
    goto error_out;
  }

  dd_errno = dd_wvs75v2b_send_cmd(dd, dd_Wvs75V2bCmd_POWER_OFF);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_wvs75v2b_wait(dd);

  dd_errno = dd_wvs75v2b_send_cmd(dd, dd_Wvs75V2bCmd_DEEP_SLEEP);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_errno = dd_wvs75v2b_send_data(dd, (uint8_t[]){0xA5}, 1);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_sleep_ms(300); // In deep sleep busy does not work so we need to estimate
                    // time required display perform deep sleep ops

  return 0;

error_dd_cleanup:
  dd_wvs75v2b_ops_reset(dd);
error_out:
  return dd_errno;
}


static unsigned char *dd_wvs75v2b_rotate(dd_wvs75v2b_t dd, int width,
                                         int heigth, unsigned char *buf,
                                         int buf_len) {
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

static dd_error_t dd_wvs75v2b_ops_display_full(dd_wvs75v2b_t dd,
                                               unsigned char *buf,
                                               int buf_len) {
  puts(__func__);
  if (dd->is_rotated) {
    buf = dd_wvs75v2b_rotate(dd, DD_WVS75V2B_HEIGTH, DD_WVS75V2B_WIDTH, buf,
                             buf_len);
  }

  // The display does full refresh in about 18 seconds so sending
  // byte by byte does not affect this time much. Real bottleneck is in screen.
  // TO-DO: However we should use buffer here.
  dd_errno = dd_wvs75v2b_send_cmd(dd, dd_Wvs75V2bCmd_START_TRANSMISSION1);
  DD_TRY_CATCH(dd_errno, out);
  for (int i = 0; i < buf_len; i++) {
    dd_errno = dd_wvs75v2b_send_data(dd, (uint8_t[]){buf[i]}, 1);
    DD_TRY_CATCH(dd_errno, out);
  }
  dd_wvs75v2b_wait(dd);

  dd_errno = dd_wvs75v2b_send_cmd(dd, dd_Wvs75V2bCmd_START_TRANSMISSION2);
  DD_TRY_CATCH(dd_errno, out);
  for (int i = 0; i < buf_len; i++) {
    dd_errno = dd_wvs75v2b_send_data(dd, (uint8_t[]){0x00}, 1);
    DD_TRY_CATCH(dd_errno, out);
  }
  dd_wvs75v2b_wait(dd);

  dd_errno = dd_wvs75v2b_send_cmd(dd, dd_Wvs75V2bCmd_DISPLAY_REFRESH);
  DD_TRY_CATCH(dd_errno, out);
  dd_wvs75v2b_wait(dd);

out:
  if (dd->is_rotated) {
    dd_free(buf);
  }

  if (dd_errno) {

    dd_wvs75v2b_ops_reset(dd);
  }

  return dd_errno;
};

