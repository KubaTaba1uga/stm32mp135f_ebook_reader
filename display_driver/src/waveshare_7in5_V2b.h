#ifndef DISPLAY_DRIVER_WAVESHARE_7IN5_V2B_H
#define DISPLAY_DRIVER_WAVESHARE_7IN5_V2B_H

#include <stdint.h>

#include "display_driver.h"
#include "gpio/gpio.h"
#include "spi/spi.h"
#include "utils/time.h"

struct dd_Wvs75V2b {
  // GPIO
  struct dd_Gpio gpio;
  struct dd_GpioPin *dc;
  struct dd_GpioPin *rst;
  struct dd_GpioPin *bsy;
  struct dd_GpioPin *pwr;

  // SPI
  struct dd_Spi spi;
};

enum dd_Wvs75V2bBusy {
  dd_Wvs75V2bBusy_BUSY = 0,
  dd_Wvs75V2bBusy_IDLE,
};

enum dd_Wvs75V2bCmd {
  dd_Wvs75V2bCmd_PANEL_SETTING                      = 0x00,
  dd_Wvs75V2bCmd_POWER_SETTING                      = 0x01,
  dd_Wvs75V2bCmd_POWER_OFF                          = 0x02,
  dd_Wvs75V2bCmd_POWER_OFF_SEQUENCE_SETTING         = 0x03,
  dd_Wvs75V2bCmd_POWER_ON                           = 0x04,
  dd_Wvs75V2bCmd_POWER_ON_MEASURE                   = 0x05,
  dd_Wvs75V2bCmd_BOOSTER_SOFT_START                 = 0x06,
  dd_Wvs75V2bCmd_DEEP_SLEEP                         = 0x07,

  dd_Wvs75V2bCmd_START_TRANSMISSION1                = 0x10,
  dd_Wvs75V2bCmd_DATA_STOP                          = 0x11,
  dd_Wvs75V2bCmd_DISPLAY_REFRESH                    = 0x12,
  dd_Wvs75V2bCmd_START_TRANSMISSION2                = 0x13,
  dd_Wvs75V2bCmd_DUAL_SPI_MODE                      = 0x15,

  dd_Wvs75V2bCmd_VCOM_AND_DATA_INTERVAL_SETTING     = 0x50,
  dd_Wvs75V2bCmd_TCON_SETTING                       = 0x60,
  dd_Wvs75V2bCmd_RESOLUTION_SETTING                 = 0x61,
};

static inline dd_error_t dd_wvs75V2b_send_cmd(struct dd_Wvs75V2b *dd, uint8_t cmd) {
  dd_errno = dd_gpio_set_pin(0, dd->dc, &dd->gpio);
  DD_TRY(dd_errno);

  dd_errno = dd_spi_send_byte(cmd, &dd->spi);
  DD_TRY(dd_errno);

  return 0;

error:
  return dd_errno;
}

static inline dd_error_t dd_wvs75V2b_send_data(struct dd_Wvs75V2b *dd, uint8_t *data,
                                        uint32_t len) {
  dd_errno = dd_gpio_set_pin(1, dd->dc, &dd->gpio);
  DD_TRY(dd_errno);

  dd_errno = dd_spi_send_bytes(data, len, &dd->spi);
  DD_TRY(dd_errno);

  return 0;

error:
  return dd_errno;
}

static void inline dd_wvs75V2b_wait(struct dd_Wvs75V2b *display) {
  puts("Busy waiting");
  while (dd_gpio_read_pin(display->bsy, &display->gpio) !=
         dd_Wvs75V2bBusy_IDLE) {
    dd_sleep_ms(10);
  }
  puts("Waiting done");
}


#endif // DISPLAY_DRIVER_WAVESHARE_7IN5_V2B_H
