#include <stdint.h>
#include <stdio.h>

#include "display_driver.h"
#include "drivers/driver.h"
#include "gpio/gpio.h"
#include "spi/spi.h"
#include "utils/err.h"
#include "utils/mem.h"
#include "utils/time.h"

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
  unsigned char *rotation_buf;
};

typedef struct dd_Wvs75V2b *dd_wvs75v2b_t;

static void dd_wvs75v2b_remove(struct dd_DisplayDriver *dd);
static dd_error_t dd_wvs75v2b_clear(struct dd_DisplayDriver *dd, bool white);
static dd_error_t dd_wvs75v2b_write(struct dd_DisplayDriver *dd, unsigned char *buf,
                                    uint32_t buf_len);
static dd_error_t dd_wvs75v2b_set_up_gpio_dc(dd_wvs75v2b_t dd,
                                             const char *gpio_chip_path,
                                             int pin_no);
static dd_error_t dd_wvs75v2b_set_up_gpio_rst(dd_wvs75v2b_t dd,
                                              const char *gpio_chip_path,
                                              int pin_no);
static dd_error_t dd_wvs75v2b_set_up_gpio_bsy(dd_wvs75v2b_t dd,
                                              const char *gpio_chip_path,
                                              int pin_no);
static dd_error_t dd_wvs75v2b_set_up_gpio_pwr(dd_wvs75v2b_t dd,
                                              const char *gpio_chip_path,
                                              int pin_no);
static dd_error_t dd_wvs75v2b_set_up_spi_master(dd_wvs75v2b_t dd,
                                                const char *spidev_path);
static dd_error_t dd_wvs75v2b_ops_reset(dd_wvs75v2b_t dd);
static dd_error_t dd_wvs75v2b_ops_power_on(dd_wvs75v2b_t dd);
static dd_error_t dd_wvs75v2b_ops_clear(dd_wvs75v2b_t dd, bool white);
static dd_error_t dd_wvs75v2b_ops_power_off(dd_wvs75v2b_t dd);
static int dd_wvs75v2b_dd_wvs75v2b_get_bit(int i, unsigned char *buf,
                                           uint32_t buf_len);
static void dd_wvs75v2b_set_bit(int i, int val, unsigned char *buf,
                                uint32_t buf_len);
static int dd_wvs75v2b_get_pixel(int x, int y, int width, unsigned char *buf,
                                 uint32_t buf_len);
static void dd_wvs75v2b_set_pixel(int x, int y, int width, int val,
                                  unsigned char *buf, uint32_t buf_len);
static unsigned char *dd_wvs75v2b_rotate(dd_wvs75v2b_t dd, int width,
                                         int heigth, unsigned char *buf,
                                         uint32_t buf_len);
static dd_error_t dd_wvs75v2b_ops_display_full(dd_wvs75v2b_t dd,
                                               unsigned char *buf,
                                               uint32_t buf_len);
static dd_error_t dd_wvs75V2b_send_cmd(struct dd_Wvs75V2b *dd, uint8_t cmd);
static dd_error_t dd_wvs75V2b_send_data(struct dd_Wvs75V2b *dd, uint8_t *data,
                                        uint32_t len);
static void dd_wvs75V2b_wait(struct dd_Wvs75V2b *display);

dd_error_t dd_wvs75v2b_probe(struct dd_DisplayDriver *driver, void *config) {
  if (!driver || !config) {
    dd_errno = dd_errnos(EINVAL, "`driver` and `config` cannot be NULL");
    goto error;
  }

  struct dd_Wvs75V2b *driver_data = dd_malloc(sizeof(struct dd_Wvs75V2b));
  *driver_data = (struct dd_Wvs75V2b){0};
  driver->driver_data = driver_data;

  struct dd_Wvs75V2bConfig *conf = config;
  if (conf->rotate) {
    driver_data->is_rotated = true;
    driver_data->rotation_buf = dd_malloc(DD_WVS75V2B_BUF_LEN);
  }

  dd_errno = dd_gpio_init(&driver_data->gpio);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);

  dd_errno = dd_wvs75v2b_set_up_gpio_dc(driver_data, conf->dc.gpio_chip_path,
                                        conf->dc.pin_no);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);

  dd_errno = dd_wvs75v2b_set_up_gpio_rst(driver_data, conf->rst.gpio_chip_path,
                                         conf->rst.pin_no);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);

  dd_errno = dd_wvs75v2b_set_up_gpio_bsy(driver_data, conf->bsy.gpio_chip_path,
                                         conf->bsy.pin_no);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);

  dd_errno = dd_wvs75v2b_set_up_gpio_pwr(driver_data, conf->pwr.gpio_chip_path,
                                         conf->pwr.pin_no);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);

  dd_errno = dd_wvs75v2b_set_up_spi_master(driver_data, conf->spi.spidev_path);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);

  dd_errno = dd_wvs75v2b_ops_reset(driver_data);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);

  driver->remove = dd_wvs75v2b_remove;
  driver->clear = dd_wvs75v2b_clear;
  driver->write = dd_wvs75v2b_write;

  return 0;

error_dd_cleanup:
  dd_wvs75v2b_remove(driver);
error:
  *driver = (struct dd_DisplayDriver){0};
  return dd_errno;
};

// Clear screen.
static dd_error_t dd_wvs75v2b_clear(struct dd_DisplayDriver *dd, bool white) {
  dd_wvs75v2b_ops_power_on(dd->driver_data);
  DD_TRY(dd_errno);

  dd_errno = dd_wvs75v2b_ops_clear(dd->driver_data, white);
  DD_TRY_CATCH(dd_errno, error_wvs75v2b_cleanup);

  dd_wvs75v2b_ops_power_off(dd->driver_data);
  DD_TRY(dd_errno);

  return 0;

error_wvs75v2b_cleanup:
  dd_wvs75v2b_ops_power_off(dd->driver_data);
error:
  return dd_errno;
};

// Write buf to screen with full refresh.
static dd_error_t dd_wvs75v2b_write(struct dd_DisplayDriver *dd, unsigned char *buf,
                                    uint32_t buf_len) {
  dd_wvs75v2b_ops_power_on(dd->driver_data);
  DD_TRY(dd_errno);

  dd_errno = dd_wvs75v2b_ops_display_full(dd->driver_data, buf, buf_len);
  DD_TRY_CATCH(dd_errno, error_display_cleanup);

  dd_wvs75v2b_ops_power_off(dd->driver_data);
  DD_TRY(dd_errno);

  return 0;
error_display_cleanup:
  dd_wvs75v2b_ops_power_off(dd->driver_data);
error:
  return dd_errno;
};

static void dd_wvs75v2b_remove(struct dd_DisplayDriver *dd) {
  if (!dd || !dd->driver_data) {
    return;
  }
  struct dd_Wvs75V2b *driver_data = dd->driver_data;

  dd_free(driver_data->rotation_buf);

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

static dd_error_t dd_wvs75v2b_set_up_gpio_dc(dd_wvs75v2b_t dd,
                                             const char *gpio_chip_path,
                                             int pin_no) {
  if (!dd || !gpio_chip_path || pin_no < 0) {
    dd_errno = dd_errnos(EINVAL, "`dd` and `gpio_chip_path` cannot be NULL, "
                                 "`pin_no` has to be positive");
    goto error;
  }

  if (dd->dc) {
    dd_errno = dd_errnos(EINVAL, "Only one dc pin allowed");
    goto error;
  }

  dd_errno = dd_gpio_add_pin(gpio_chip_path, pin_no, &dd->dc, &dd->gpio);
  DD_TRY(dd_errno);

  dd_errno = dd_gpio_set_pin_output(dd->dc,
                                    true // Active-high
  );
  DD_TRY_CATCH(dd_errno, error_pin_cleanup);

  return 0;

error_pin_cleanup:
  dd_gpio_pin_destroy(dd->dc, &dd->gpio);
error:
  return dd_errno;
}

static dd_error_t dd_wvs75v2b_set_up_gpio_rst(dd_wvs75v2b_t dd,
                                              const char *gpio_chip_path,
                                              int pin_no) {

  if (!dd || !gpio_chip_path || pin_no < 0) {
    dd_errno = dd_errnos(EINVAL, "`dd` and `gpio_chip_path` cannot be NULL, "
                                 "`pin_no` has to be positive");
    goto error;
  }

  if (dd->rst) {
    dd_errno = dd_errnos(EINVAL, "Only one rst pin allowed");
    goto error;
  }

  dd_errno = dd_gpio_add_pin(gpio_chip_path, pin_no, &dd->rst, &dd->gpio);
  DD_TRY(dd_errno);

  dd_errno = dd_gpio_set_pin_output(dd->rst,
                                    false // Active-low
  );
  DD_TRY_CATCH(dd_errno, error_pin_cleanup);

  return 0;

error_pin_cleanup:
  dd_gpio_pin_destroy(dd->rst, &dd->gpio);
error:
  return dd_errno;
};

static dd_error_t dd_wvs75v2b_set_up_gpio_bsy(dd_wvs75v2b_t dd,
                                              const char *gpio_chip_path,
                                              int pin_no) {
  if (!dd || !gpio_chip_path || pin_no < 0) {
    dd_errno = dd_errnos(EINVAL, "`dd` and `gpio_chip_path` cannot be NULL, "
                                 "`pin_no` has to be positive");
    goto error;
  }

  if (dd->bsy) {
    dd_errno = dd_errnos(EINVAL, "Only one bsy pin allowed");
    goto error;
  }

  dd_errno = dd_gpio_add_pin(gpio_chip_path, pin_no, &dd->bsy, &dd->gpio);
  DD_TRY(dd_errno);

  dd_errno = dd_gpio_set_pin_input(dd->bsy);
  DD_TRY_CATCH(dd_errno, error_pin_cleanup);

  return 0;

error_pin_cleanup:
  dd_gpio_pin_destroy(dd->bsy, &dd->gpio);
error:
  return dd_errno;
};

static dd_error_t dd_wvs75v2b_set_up_gpio_pwr(dd_wvs75v2b_t dd,
                                              const char *gpio_chip_path,
                                              int pin_no) {

  if (!dd || !gpio_chip_path || pin_no < 0) {
    dd_errno = dd_errnos(EINVAL, "`dd` and `gpio_chip_path` cannot be NULL, "
                                 "`pin_no` has to be positive");
    goto error;
  }

  if (dd->pwr) {
    dd_errno = dd_errnos(EINVAL, "Only one pwr pin allowed");
    goto error;
  }

  dd_errno = dd_gpio_add_pin(gpio_chip_path, pin_no, &dd->pwr, &dd->gpio);
  DD_TRY(dd_errno);

  dd_errno = dd_gpio_set_pin_output(dd->pwr,
                                    true // Active-high
  );
  DD_TRY_CATCH(dd_errno, error_pin_cleanup);

  return 0;

error_pin_cleanup:
  dd_gpio_pin_destroy(dd->pwr, &dd->gpio);

error:
  return dd_errno;
}

static dd_error_t dd_wvs75v2b_set_up_spi_master(dd_wvs75v2b_t dd,
                                                const char *spidev_path) {
  if (!dd || !spidev_path) {
    dd_errno = dd_errnos(EINVAL, "`dd` and `spidev_path` cannot be NULL");
    goto error;
  }

  if (dd->spi.path) {
    dd_errno = dd_errnos(EINVAL, "Only one spi allowed");
    goto error;
  }

  dd_errno = dd_spi_init(spidev_path, &dd->spi);
  DD_TRY(dd_errno);

  return 0;

error:
  return dd_errno;
}

static dd_error_t dd_wvs75v2b_ops_reset(dd_wvs75v2b_t dd) {
  puts(__func__);
  if (!dd || !dd->pwr || !dd->rst) {
    dd_errno =
        dd_errnos(EINVAL, "`dd`, `dd->pwr` and `dd->rst` cannot be NULL");
    goto error;
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

  dd_wvs75V2b_wait(dd); // Give chip time to reset itself

  return 0;

error_rst_cleanup:
  dd_gpio_set_pin(0, dd->rst, &dd->gpio);
error:
  return dd_errno;
};

static dd_error_t dd_wvs75v2b_ops_power_on(dd_wvs75v2b_t dd) {
  puts(__func__);
  if (!dd || !dd->dc || !dd->rst || !dd->bsy || !dd->pwr || !dd->spi.path) {
    dd_errno = dd_errnos(EINVAL, "`dd`, `dd->dc`, `dd->rst`, `dd->bsy`, "
                                 "`dd->pwr` and `dd->spi.path` cannot be NULL");
    goto error;
  }

  if (dd_gpio_read_pin(dd->pwr, &dd->gpio) == 1) {
    goto out;
  }

  dd_errno = dd_gpio_set_pin(1, dd->pwr, &dd->gpio);
  DD_TRY(dd_errno);
  dd_sleep_ms(200);

  dd_errno = dd_wvs75V2b_send_cmd(dd, dd_Wvs75V2bCmd_POWER_SETTING);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_errno = dd_wvs75V2b_send_data(dd,
                                   (uint8_t[]){
                                       0x07, // LDO disabled, VDHR
                                       0x07, // VGH=20V,VGL=-20V
                                       0x3F, // VDH=15V
                                       0x3F, // VDL=-15V
                                   },
                                   4);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  /* dd_wvs75V2b_wait(dd); */

  dd_errno = dd_wvs75V2b_send_cmd(dd, dd_Wvs75V2bCmd_BOOSTER_SOFT_START);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  // I'm not sure what this part does but it is in mainline driver
  dd_errno = dd_wvs75V2b_send_data(dd,
                                   (uint8_t[]){
                                       0x17,
                                       0x17,
                                       0x28,
                                       0x17,
                                   },
                                   4);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  /* dd_wvs75V2b_wait(dd); */

  dd_errno = dd_wvs75V2b_send_cmd(dd, dd_Wvs75V2bCmd_POWER_ON);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_sleep_ms(100);
  /* dd_wvs75V2b_wait(dd); */

  dd_errno = dd_wvs75V2b_send_cmd(dd, dd_Wvs75V2bCmd_PANEL_SETTING);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_errno = dd_wvs75V2b_send_data(
      dd,
      (uint8_t[]){
          0x0F, // Gate scan direction UP, Source Shift Direction Rigth, Booster
                // on, Do not perform soft reset, Red/White/black mode,
                // White/black works very slow
      },
      1);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  /* dd_wvs75V2b_wait(dd); */

  dd_errno = dd_wvs75V2b_send_cmd(dd, dd_Wvs75V2bCmd_RESOLUTION_SETTING);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_errno = dd_wvs75V2b_send_data(dd,
                                   (uint8_t[]){
                                       0x03,
                                       0x20,
                                       0x01,
                                       0xE0,
                                   },
                                   4);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);

  dd_errno = dd_wvs75V2b_send_cmd(dd, dd_Wvs75V2bCmd_DUAL_SPI_MODE);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_errno = dd_wvs75V2b_send_data(dd, (uint8_t[]){0x00}, 1);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  /* dd_wvs75V2b_wait(dd); */

  dd_errno =
      dd_wvs75V2b_send_cmd(dd, dd_Wvs75V2bCmd_VCOM_AND_DATA_INTERVAL_SETTING);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_errno = dd_wvs75V2b_send_data(dd,
                                   (uint8_t[]){
                                       0x11,
                                       0x07,
                                   },
                                   2);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  /* dd_wvs75V2b_wait(dd); */

  dd_errno = dd_wvs75V2b_send_cmd(dd, dd_Wvs75V2bCmd_TCON_SETTING);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_errno = dd_wvs75V2b_send_data(dd, (uint8_t[]){0x22}, 1);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_wvs75V2b_wait(dd);

out:
  return 0;

error_dd_cleanup:
  dd_wvs75v2b_ops_reset(dd);
error:
  return dd_errno;
}

static dd_error_t dd_wvs75v2b_ops_clear(dd_wvs75v2b_t dd, bool white) {
  puts(__func__);
  if (!dd || !dd->dc || !dd->rst || !dd->bsy || !dd->pwr || !dd->spi.path) {
    dd_errno = dd_errnos(EINVAL, "`dd`, `dd->dc`, `dd->rst`, `dd->bsy`, "
                                 "`dd->pwr` and `dd->spi.path` cannot be NULL");
    goto error;
  }

  dd_errno = dd_wvs75V2b_send_cmd(dd, dd_Wvs75V2bCmd_START_TRANSMISSION1);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  for (int i = 0; i < DD_WVS75V2B_HEIGTH * (DD_WVS75V2B_WIDTH / 8); i++) {
    dd_errno = dd_wvs75V2b_send_data(dd,
                                     (uint8_t[]){
                                         white ? 0xFF : 0x00,
                                     },
                                     1);
    DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  }
  dd_wvs75V2b_wait(dd);

  dd_errno = dd_wvs75V2b_send_cmd(dd, dd_Wvs75V2bCmd_START_TRANSMISSION2);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  for (int i = 0; i < DD_WVS75V2B_HEIGTH * (DD_WVS75V2B_WIDTH / 8); i++) {
    dd_errno = dd_wvs75V2b_send_data(dd, (uint8_t[]){0x00}, 1);
    DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  }
  dd_wvs75V2b_wait(dd);

  dd_errno = dd_wvs75V2b_send_cmd(dd, dd_Wvs75V2bCmd_DISPLAY_REFRESH);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_sleep_ms(100);
  dd_wvs75V2b_wait(dd);

  return 0;

error_dd_cleanup:
  dd_wvs75v2b_ops_reset(dd);
error:
  return dd_errno;
}

static dd_error_t dd_wvs75v2b_ops_power_off(dd_wvs75v2b_t dd) {
  puts(__func__);
  if (!dd || !dd->dc || !dd->rst || !dd->bsy || !dd->pwr || !dd->spi.path) {
    dd_errno = dd_errnos(EINVAL, "`dd`, `dd->dc`, `dd->rst`, `dd->bsy`, "
                                 "`dd->pwr` and `dd->spi.path` cannot be NULL");
    goto error;
  }

  dd_errno = dd_wvs75V2b_send_cmd(dd, dd_Wvs75V2bCmd_POWER_OFF);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_wvs75V2b_wait(dd);

  dd_errno = dd_wvs75V2b_send_cmd(dd, dd_Wvs75V2bCmd_DEEP_SLEEP);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_errno = dd_wvs75V2b_send_data(dd, (uint8_t[]){0xA5}, 1);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_sleep_ms(300); // In deep sleep busy does not work so we need to estimate
                    // time required display perform deep sleep ops

  return 0;

error_dd_cleanup:
  dd_wvs75v2b_ops_reset(dd);
error:
  return dd_errno;
}

static int dd_wvs75v2b_get_bit(int i, unsigned char *buf, uint32_t buf_len) {
  if (i < 0 || (uint32_t)i >= buf_len * 8)
    return -1;
  int byte = i / 8;
  int bit = 7 - (i % 8);
  return (buf[byte] >> bit) & 1;
}

static void dd_wvs75v2b_set_bit(int i, int val, unsigned char *buf,
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

static int dd_wvs75v2b_get_pixel(int x, int y, int width, unsigned char *buf,
                                 uint32_t buf_len) {
  if (x < 0 || y < 0) {
    return -1;
  }

  int bit = width * y + x;

  return dd_wvs75v2b_get_bit(bit, buf, buf_len);
}

static void dd_wvs75v2b_set_pixel(int x, int y, int width, int val,
                                  unsigned char *buf, uint32_t buf_len) {
  if (x < 0 || y < 0) {
    return;
  }

  int bit = width * y + x;

  dd_wvs75v2b_set_bit(bit, val, buf, buf_len);
}

static unsigned char *dd_wvs75v2b_rotate(dd_wvs75v2b_t dd, int width,
                                         int heigth, unsigned char *buf,
                                         uint32_t buf_len) {
  int dst_i = 0;
  int v;
  for (int x = width - 1; x >= 0; --x) {
    for (int y = 0; y < heigth; ++y) {
      v = dd_wvs75v2b_get_pixel(x, y, width, buf, buf_len);
      dd_wvs75v2b_set_bit(dst_i++, v, rotated, buf_len);
    }
  }

  return dd->rotation_buf;
}

static dd_error_t dd_wvs75v2b_ops_display_full(dd_wvs75v2b_t dd,
                                               unsigned char *buf,
                                               uint32_t buf_len) {
  puts(__func__);
  if (!dd || !dd->dc || !dd->rst || !dd->bsy || !dd->pwr || !dd->spi.path ||
      !buf) {
    dd_errno =
        dd_errnos(EINVAL, "`dd`, `dd->dc`, `dd->rst`, `dd->bsy`, "
                          "`dd->pwr`, `dd->spi.path` and `buf` cannot be NULL");
    goto error;
  }

  if (dd->is_rotated) {
    buf = dd_wvs75v2b_rotate(dd, DD_WVS75V2B_HEIGTH, DD_WVS75V2B_WIDTH, buf,
                             buf_len);
  }

  // The display does full refresh in about 18 seconds so sending
  // byte by byte does not affect this time much. Real bottleneck is in screen.
  dd_errno = dd_wvs75V2b_send_cmd(dd, dd_Wvs75V2bCmd_START_TRANSMISSION1);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  for (int i = 0; i < buf_len; i++) {
    dd_errno = dd_wvs75V2b_send_data(dd, (uint8_t[]){buf[i]}, 1);
    DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  }
  dd_wvs75V2b_wait(dd);

  dd_errno = dd_wvs75V2b_send_cmd(dd, dd_Wvs75V2bCmd_START_TRANSMISSION2);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  for (int i = 0; i < img_data_len; i++) {
    dd_errno = dd_wvs75V2b_send_data(dd, (uint8_t[]){0x00}, 1);
    DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  }
  dd_wvs75V2b_wait(dd);

  dd_errno = dd_wvs75V2b_send_cmd(dd, dd_Wvs75V2bCmd_DISPLAY_REFRESH);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_wvs75V2b_wait(dd);

  return 0;

error_dd_cleanup:
  dd_wvs75v2b_ops_reset(dd);
error:
  return dd_errno;
};

static dd_error_t dd_wvs75V2b_send_cmd(struct dd_Wvs75V2b *dd, uint8_t cmd) {
  dd_errno = dd_gpio_set_pin(dd_Wvs75V2bDc_CMD, dd->dc, &dd->gpio);
  DD_TRY(dd_errno);

  dd_errno = dd_spi_send_byte(cmd, &dd->spi);
  DD_TRY(dd_errno);

  return 0;

error:
  return dd_errno;
}

static dd_error_t dd_wvs75V2b_send_data(struct dd_Wvs75V2b *dd, uint8_t *data,
                                        uint32_t len) {
  dd_errno = dd_gpio_set_pin(dd_Wvs75V2bDc_DATA, dd->dc, &dd->gpio);
  DD_TRY(dd_errno);

  dd_errno = dd_spi_send_bytes(data, len, &dd->spi);
  DD_TRY(dd_errno);

  return 0;

error:
  return dd_errno;
}

static void dd_wvs75V2b_wait(struct dd_Wvs75V2b *display) {
  puts("Busy waiting");
  while (dd_gpio_read_pin(display->bsy, &display->gpio) !=
         dd_Wvs75V2bBsy_IDLE) {
    dd_sleep_ms(10);
  }
  puts("Waiting done");
}
