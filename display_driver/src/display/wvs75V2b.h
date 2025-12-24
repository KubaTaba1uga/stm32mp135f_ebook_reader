#ifndef DISPLAY_DRIVER_DISPLAY_WVS75V2B_H
#define DISPLAY_DRIVER_DISPLAY_WVS75V2B_H

#include "display_driver.h"
#include "gpio/gpio.h"
#include <stdbool.h>

struct dd_DisplayWvs75V2b {
  struct dd_Gpio gpio;
  struct dd_GpioPin *dc;
  struct dd_GpioPin *rst;
  struct dd_GpioPin *bsy;
  struct dd_GpioPin *pwr;
};

dd_error_t dd_display_wvs75V2b_init(struct dd_DisplayWvs75V2b *display);
void dd_display_wvs75V2b_destroy(struct dd_DisplayWvs75V2b *display);
dd_error_t dd_display_wvs75V2b_add_gpio_dc(const char *chip_path, int pin_no,
                                           struct dd_DisplayWvs75V2b *display);

dd_error_t dd_display_wvs75V2b_add_gpio_rst(const char *chip_path, int pin_no,
                                            struct dd_DisplayWvs75V2b *display);

dd_error_t dd_display_wvs75V2b_add_gpio_bsy(const char *chip_path, int pin_no,
                                            struct dd_DisplayWvs75V2b *display);

dd_error_t dd_display_wvs75V2b_add_gpio_pwr(const char *chip_path, int pin_no,
                                            struct dd_DisplayWvs75V2b *display);

dd_error_t dd_display_wvs75V2b_reset(struct dd_DisplayWvs75V2b *display);


#endif // DISPLAY_DRIVER_DISPLAY_WVS75V2B
