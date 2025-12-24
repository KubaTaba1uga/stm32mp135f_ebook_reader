#ifndef DISPLAY_DRIVER_DISPLAY_H
#define DISPLAY_DRIVER_DISPLAY_H

#include "display_driver.h"
#include "gpio/gpio.h"
#include <stdbool.h>

struct dd_Display {
  struct dd_Gpio gpio;
  struct dd_GpioPin *dc;
  struct dd_GpioPin *rst;
  struct dd_GpioPin *bsy;
  struct dd_GpioPin *pwr;
  /* struct dd_SPI spi;   */
};

dd_error_t dd_display_init(struct dd_Display *display);
void dd_display_destroy(struct dd_Display *display);

dd_error_t dd_display_add_gpio_dc(const char *chip_path, int pin_no,
                                  struct dd_Display *display);

dd_error_t dd_display_add_gpio_rst(const char *chip_path, int pin_no,struct dd_Display *display);

dd_error_t dd_display_add_gpio_bsy(const char *chip_path, int pin_no,
                                   struct dd_Display *display);

dd_error_t dd_display_add_gpio_pwr(const char *chip_path, int pin_no,
                                   struct dd_Display *display);

#endif // DISPLAY_DRIVER_H
