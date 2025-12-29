#ifndef DISPLAY_DRIVER_GPIO_H
#define DISPLAY_DRIVER_GPIO_H

#include "display_driver.h"
#include "utils/list.h"
#include <stdbool.h>

struct dd_GpioChip {
  const char *path;
  void *private;
};

struct dd_GpioPin {
  struct dd_GpioChip *chip;
  void *private;
  bool is_out;
  int pin_no;
};

struct dd_Gpio {
  struct dd_List chips; // struct dd_GpioChip *
  struct dd_List pins;  // struct dd_GpioPin *
};

dd_error_t dd_gpio_init(struct dd_Gpio *gpio);
void dd_gpio_destroy(struct dd_Gpio *gpio);

dd_error_t dd_gpio_add_pin(const char *chip_path, int pin_no,
                           struct dd_GpioPin **out, struct dd_Gpio *gpio);
dd_error_t dd_gpio_set_pin_output(struct dd_GpioPin *pin, bool is_active_high);
dd_error_t dd_gpio_set_pin_input(struct dd_GpioPin *pin);
int dd_gpio_read_pin(struct dd_GpioPin *pin, struct dd_Gpio *gpio);
dd_error_t dd_gpio_set_pin(int value, struct dd_GpioPin *pin,
                           struct dd_Gpio *gpio);
void dd_gpio_pin_destroy(struct dd_GpioPin *pin, struct dd_Gpio *gpio);

#endif // DISPLAY_DRIVER_H
