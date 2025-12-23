#include "display_driver.h"
#include "gpio/gpio.h"
#include "utils/err.h"
#include "utils/mem.h"

struct dd_DisplayDriver {
  enum dd_SupportedDisplayEnum display;
  struct dd_Gpio gpio;
};

dd_error_t dd_display_driver_init(dd_display_driver_t *dd,
                                  enum dd_SupportedDisplayEnum display) {
  *dd = dd_malloc(sizeof(struct dd_DisplayDriver));

  (*dd)->display = display;
  
  dd_errno = dd_gpio_init(&(*dd)->gpio);
  if (dd_errno) {
    dd_ewrap();
    goto error;
  }

  return NULL;

error:
  dd_free(*dd);
  *dd = NULL;
  return dd_errno;
}

dd_error_t dd_display_driver_add_gpio_pin(dd_display_driver_t dd,
                                          enum dd_GpioPinEnum pin_cls,
                                          const char *gpio_chip, int pin_no) {
  return NULL;
}

dd_error_t dd_display_driver_add_spi_slave(dd_display_driver_t dd,
                                           const char *spi_chip) {

  return NULL;
}

void dd_display_driver_destroy(dd_display_driver_t *dd) {}

int dd_error_get_code(dd_error_t err) { return err->code; }

const char *dd_error_get_msg(dd_error_t err) { return err->msg; }
