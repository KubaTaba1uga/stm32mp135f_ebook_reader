#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "display_driver.h"
#include "drivers/driver.h"
#include "gpio/gpio.h"
#include "spi/spi.h"
#include "utils/err.h"
#include "utils/mem.h"
#include "utils/time.h"

#define DD_WVS75V2_WIDTH 800
#define DD_WVS75V2_HEIGTH 480
#define DD_WVS75V2_BUF_LEN DD_WVS75V2_HEIGTH *DD_WVS75V2_HEIGTH / 8

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

typedef struct dd_Wvs75v2 *dd_wvs75v2_t;
static dd_error_t dd_wvs75v2_set_up_gpio_dc(dd_wvs75v2_t, const char *, int);
static dd_error_t dd_wvs75v2_set_up_gpio_rst(dd_wvs75v2_t, const char *, int);
static dd_error_t dd_wvs75v2_set_up_gpio_bsy(dd_wvs75v2_t, const char *, int);
static dd_error_t dd_wvs75v2_set_up_gpio_pwr(dd_wvs75v2_t, const char *, int);
static dd_error_t dd_wvs75v2_set_up_gpio_dc(dd_wvs75v2_t, const char *, int);
static dd_error_t dd_wvs75v2_set_up_spi_master(dd_wvs75v2_t, const char *);
static void dd_wvs75v2_remove(struct dd_DisplayDriver *);
static dd_error_t dd_wvs75v2_clear(struct dd_DisplayDriver *, bool);
static dd_error_t dd_wvs75v2_display_full(struct dd_DisplayDriver *,
                                          unsigned char *, uint32_t);
static dd_error_t dd_wvs75v2_ops_reset(dd_wvs75v2_t);

dd_error_t dd_wvs75v2_probe(struct dd_DisplayDriver *driver, void *config) {
  assert(driver != NULL);
  assert(config != NULL);

  struct dd_Wvs75v2 *driver_data = dd_malloc(sizeof(struct dd_Wvs75v2));
  *driver_data = (struct dd_Wvs75v2){0};
  *driver = (struct dd_DisplayDriver){
      .driver_data = driver_data,
  };

  struct dd_Wvs75V2Config *conf = config;

  dd_errno = dd_gpio_init(&driver_data->gpio);
  DD_TRY(dd_errno);

  dd_errno = dd_wvs75v2_set_up_gpio_dc(driver_data, conf->dc.gpio_chip_path,
                                       conf->dc.pin_no);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);

  dd_errno = dd_wvs75v2_set_up_gpio_rst(driver_data, conf->rst.gpio_chip_path,
                                        conf->rst.pin_no);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);

  dd_errno = dd_wvs75v2_set_up_gpio_bsy(driver_data, conf->bsy.gpio_chip_path,
                                        conf->bsy.pin_no);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);

  dd_errno = dd_wvs75v2_set_up_gpio_pwr(driver_data, conf->pwr.gpio_chip_path,
                                        conf->pwr.pin_no);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);

  dd_errno = dd_wvs75v2_set_up_spi_master(driver_data, conf->spi.spidev_path);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);

  if (conf->rotate) {
    driver_data->is_rotated = true;
    driver_data->rotation_buf = dd_malloc(DD_WVS75V2_BUF_LEN * 2);
  }

  driver->remove = dd_wvs75v2_remove;
  driver->clear = dd_wvs75v2_clear;
  driver->write = dd_wvs75v2_display_full;

  return 0;

error_dd_cleanup:
  dd_wvs75v2_remove(driver);
error_out:
  dd_free(driver_data);
  *driver = (struct dd_DisplayDriver){0};
  return dd_errno;
};

static dd_error_t dd_wvs75v2_set_up_gpio_dc(dd_wvs75v2_t dd,
                                            const char *gpio_chip_path,
                                            int pin_no) {
  assert(dd != NULL);
  assert(gpio_chip_path != NULL);
  assert(pin_no >= 0);

  if (dd->dc) {
    dd_errno = dd_errnos(EINVAL, "Only one dc pin allowed");
    goto error_out;
  }

  dd_errno = dd_gpio_add_pin(gpio_chip_path, pin_no, &dd->dc, &dd->gpio);
  DD_TRY(dd_errno);

  dd_errno = dd_gpio_set_pin_output(dd->dc, true);
  DD_TRY_CATCH(dd_errno, error_pin_cleanup);

  return 0;

error_pin_cleanup:
  dd_gpio_pin_destroy(dd->dc, &dd->gpio);
error_out:
  return dd_errno;
}

static dd_error_t dd_wvs75v2_set_up_gpio_rst(dd_wvs75v2_t dd,
                                             const char *gpio_chip_path,
                                             int pin_no) {
  assert(dd != NULL);
  assert(gpio_chip_path != NULL);
  assert(pin_no >= 0);

  if (dd->rst) {
    dd_errno = dd_errnos(EINVAL, "Only one rst pin allowed");
    goto error_out;
  }

  dd_errno = dd_gpio_add_pin(gpio_chip_path, pin_no, &dd->rst, &dd->gpio);
  DD_TRY(dd_errno);

  dd_errno = dd_gpio_set_pin_output(dd->rst, false);
  DD_TRY_CATCH(dd_errno, error_pin_cleanup);

  return 0;

error_pin_cleanup:
  dd_gpio_pin_destroy(dd->rst, &dd->gpio);
error_out:
  return dd_errno;
};

static dd_error_t dd_wvs75v2_set_up_gpio_bsy(dd_wvs75v2_t dd,
                                             const char *gpio_chip_path,
                                             int pin_no) {
  assert(dd != NULL);
  assert(gpio_chip_path != NULL);
  assert(pin_no >= 0);

  if (dd->bsy) {
    dd_errno = dd_errnos(EINVAL, "Only one bsy pin allowed");
    goto error_out;
  }

  dd_errno = dd_gpio_add_pin(gpio_chip_path, pin_no, &dd->bsy, &dd->gpio);
  DD_TRY(dd_errno);

  dd_errno = dd_gpio_set_pin_input(dd->bsy);
  DD_TRY_CATCH(dd_errno, error_pin_cleanup);

  return 0;

error_pin_cleanup:
  dd_gpio_pin_destroy(dd->bsy, &dd->gpio);
error_out:
  return dd_errno;
};

static dd_error_t dd_wvs75v2_set_up_gpio_pwr(dd_wvs75v2_t dd,
                                             const char *gpio_chip_path,
                                             int pin_no) {
  assert(dd != NULL);
  assert(gpio_chip_path != NULL);
  assert(pin_no >= 0);

  if (dd->pwr) {
    dd_errno = dd_errnos(EINVAL, "Only one pwr pin allowed");
    goto error_out;
  }

  dd_errno = dd_gpio_add_pin(gpio_chip_path, pin_no, &dd->pwr, &dd->gpio);
  DD_TRY(dd_errno);

  dd_errno = dd_gpio_set_pin_output(dd->pwr, true);
  DD_TRY_CATCH(dd_errno, error_pin_cleanup);

  return 0;

error_pin_cleanup:
  dd_gpio_pin_destroy(dd->pwr, &dd->gpio);

error_out:
  return dd_errno;
}

static dd_error_t dd_wvs75v2_set_up_spi_master(dd_wvs75v2_t dd,
                                               const char *spidev_path) {
  assert(dd != NULL);
  assert(spidev_path != NULL);

  if (dd->spi.path) {
    dd_errno = dd_errnos(EINVAL, "Only one spi allowed");
    goto error_out;
  }

  dd_errno = dd_spi_init(spidev_path, &dd->spi);
  DD_TRY(dd_errno);

  return 0;

error_out:
  return dd_errno;
}

static void dd_wvs75v2_remove(struct dd_DisplayDriver *dd) {
  assert(dd != NULL);
  assert(dd->driver_data != NULL);

  struct dd_Wvs75v2 *driver_data = dd->driver_data;

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
  *dd = (struct dd_DisplayDriver){0};
}

static void dd_wvs75v2_wait(struct dd_Wvs75v2 *display) {
  // puts("Busy waiting");
  while (dd_gpio_read_pin(display->bsy, &display->gpio) != dd_Wvs75v2Bsy_IDLE) {
    dd_sleep_ms(10);
  }
  // puts("Waiting done");
}

static dd_error_t dd_wvs75v2_ops_reset(dd_wvs75v2_t dd) {
  // puts(__func__);
  assert(dd != NULL);
  assert(dd->pwr != NULL);
  assert(dd->rst != NULL);

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

static dd_error_t dd_wvs75v2_ops_power_on(dd_wvs75v2_t dd) {
  // puts(__func__);
  assert(dd != NULL);
  assert(dd->dc != NULL);
  assert(dd->rst != NULL);
  assert(dd->bsy != NULL);
  assert(dd->pwr != NULL);
  assert(dd->spi.path != NULL);

  if (dd_gpio_read_pin(dd->pwr, &dd->gpio) != 1) {
    dd_errno = dd_gpio_set_pin(1, dd->pwr, &dd->gpio);
    DD_TRY(dd_errno);
    dd_sleep_ms(200);
  }

  dd_errno = dd_gpio_set_pin(1, dd->pwr, &dd->gpio);
  DD_TRY(dd_errno);
  dd_sleep_ms(200);

  dd_errno = dd_wvs75v2_ops_reset(dd);
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
  dd_wvs75v2_ops_reset(dd);
error_out:
  return dd_errno;
}

static dd_error_t dd_wvs75v2_ops_clear(dd_wvs75v2_t dd, bool white) {
  // puts(__func__);
  assert(dd != NULL);
  assert(dd->dc != NULL);
  assert(dd->rst != NULL);
  assert(dd->bsy != NULL);
  assert(dd->pwr != NULL);
  assert(dd->spi.path != NULL);

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
  dd_sleep_ms(1000);
  dd_wvs75v2_wait(dd);

  return 0;

error_dd_cleanup:
  dd_wvs75v2_ops_reset(dd);
  return dd_errno;
}

static dd_error_t dd_wvs75v2_ops_power_off(dd_wvs75v2_t dd) {
  // puts(__func__);
  assert(dd != NULL);
  assert(dd->dc != NULL);
  assert(dd->rst != NULL);
  assert(dd->bsy != NULL);
  assert(dd->pwr != NULL);
  assert(dd->spi.path != NULL);

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
  dd_wvs75v2_ops_reset(dd);
  return dd_errno;
}

static dd_error_t dd_wvs75v2_clear(struct dd_DisplayDriver *dd, bool white) {
  dd_wvs75v2_ops_power_on(dd->driver_data);
  DD_TRY(dd_errno);

  dd_errno = dd_wvs75v2_ops_clear(dd->driver_data, white);
  DD_TRY_CATCH(dd_errno, error_wvs75v2_cleanup);

  dd_wvs75v2_ops_power_off(dd->driver_data);
  DD_TRY(dd_errno);

  return 0;

error_wvs75v2_cleanup:
  dd_wvs75v2_ops_power_off(dd->driver_data);
error_out:
  return dd_errno;
};

static int dd_wvs75v2_get_bit(int i, unsigned char *buf, uint32_t buf_len) {
  if (i < 0 || (uint32_t)i >= buf_len * 8)
    return -1;
  int byte = i / 8;
  int bit = 7 - (i % 8);
  return (buf[byte] >> bit) & 1;
}

static void dd_wvs75v2_set_bit(int i, int val, unsigned char *buf,
                               uint32_t buf_len) {
  if (i < 0 || (uint32_t)i >= buf_len * 8)
    return;
  int byte = i / 8;
  int bit = 7 - (i % 8);
  if (val)
    buf[byte] |= (1u << bit);
  else
    buf[byte] &= ~(1u << bit);
}

static int dd_wvs75v2_get_pixel(int x, int y, int width, unsigned char *buf,
                                uint32_t buf_len) {
  if (x < 0 || y < 0) {
    return -1;
  }

  int bit = width * y + x;

  return dd_wvs75v2_get_bit(bit, buf, buf_len);
}

static unsigned char *dd_wvs75v2_rotate(dd_wvs75v2_t dd, int width, int heigth,
                                        unsigned char *buf, uint32_t buf_len) {
  int dst_i = 0;
  int v;
  for (int x = width - 1; x >= 0; --x) {
    for (int y = 0; y < heigth; ++y) {
      v = dd_wvs75v2_get_pixel(x, y, width, buf, buf_len);
      dd_wvs75v2_set_bit(dst_i++, v, dd->rotation_buf, buf_len);
    }
  }

  return dd->rotation_buf;
}

static dd_error_t dd_wvs75v2_ops_display_full(dd_wvs75v2_t dd,
                                              unsigned char *buf,
                                              uint32_t buf_len) {
  // puts(__func__);
  assert(dd != NULL);
  assert(dd->dc != NULL);
  assert(dd->rst != NULL);
  assert(dd->bsy != NULL);
  assert(dd->pwr != NULL);
  assert(dd->spi.path != NULL);

  if (dd->is_rotated) {
    buf = dd_wvs75v2_rotate(dd, DD_WVS75V2_HEIGTH, DD_WVS75V2_WIDTH, buf,
                            buf_len);
  }

  // The display does full refresh in about 18 seconds so sending
  // byte by byte does not affect this time much. Real bottleneck is in screen.
  // TO-DO: However we should use buffer here.
  dd_errno = dd_wvs75v2_send_cmd(dd, dd_Wvs75v2Cmd_START_TRANSMISSION1);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  uint8_t chunk[1024] = {0};
  for (int i = 0; i < buf_len; i += sizeof(chunk)) {
    dd_errno = dd_wvs75v2_send_data(dd, chunk, sizeof(chunk));
    DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  }
  dd_wvs75v2_wait(dd);

  dd_errno = dd_wvs75v2_send_cmd(dd, dd_Wvs75v2Cmd_START_TRANSMISSION2);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  for (int i = 0; i < buf_len; i += sizeof(chunk)) {
    for (int chunk_i = 0; chunk_i < sizeof(chunk); chunk_i++) {
      chunk[chunk_i] = ~(*(buf + i + chunk_i));
    }

    dd_errno = dd_wvs75v2_send_data(dd, chunk, sizeof(chunk));
    DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  }

  dd_wvs75v2_wait(dd);

  dd_errno = dd_wvs75v2_send_cmd(dd, dd_Wvs75v2Cmd_DISPLAY_REFRESH);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_wvs75v2_wait(dd);

  return 0;

error_dd_cleanup:
  dd_wvs75v2_ops_reset(dd);
  return dd_errno;
};

static dd_error_t dd_wvs75v2_display_full(struct dd_DisplayDriver *dd,
                                          unsigned char *buf,
                                          uint32_t buf_len) {
  dd_wvs75v2_ops_power_on(dd->driver_data);
  DD_TRY(dd_errno);

  dd_errno = dd_wvs75v2_ops_display_full(dd->driver_data, buf, buf_len);
  DD_TRY_CATCH(dd_errno, error_wvs75v2_cleanup);

  dd_wvs75v2_ops_power_off(dd->driver_data);
  DD_TRY(dd_errno);

  return 0;

error_wvs75v2_cleanup:
  dd_wvs75v2_ops_power_off(dd->driver_data);
error_out:
  return dd_errno;
};
