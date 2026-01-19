#include <stdio.h>
#include <assert.h>

#include "display_driver.h"
#include "drivers/driver.h"
#include "gpio/gpio.h"
#include "spi/spi.h"
#include "utils/err.h"
#include "utils/mem.h"

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
  dd_Wvs75v2Cmd_DUAL_SPI_MODE = 0x15,

  dd_Wvs75v2Cmd_VCOM_AND_DATA_INTERVAL_SETTING = 0x50,
  dd_Wvs75v2Cmd_TCON_SETTING = 0x60,
  dd_Wvs75v2Cmd_RESOLUTION_SETTING = 0x61,
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
};

typedef struct dd_Wvs75v2 *dd_wvs75v2_t;

dd_error_t dd_wvs75v2_probe(struct dd_DisplayDriver *driver, void *config) {
  assert(driver != NULL);
  assert(config != NULL);

  struct dd_Wvs75v2 *driver_data = dd_malloc(sizeof(struct dd_Wvs75v2));
  *driver_data = (struct dd_Wvs75v2){0};
  driver->driver_data = driver_data;

  struct dd_Wvs75v2Config *conf = config;

  /* dd_errno = dd_gpio_init(&driver_data->gpio); */
  /* DD_TRY_CATCH(dd_errno, error_dd_cleanup); */

  
  return 0;

error_out:
  *driver = (struct dd_DisplayDriver){0};
  return dd_errno;
};

