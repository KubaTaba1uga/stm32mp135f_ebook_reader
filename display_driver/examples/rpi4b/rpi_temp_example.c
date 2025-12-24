#include "display_driver.h"
#include "utils/err.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  dd_display_driver_t dd;
  dd_error_t err;

  puts("Starting driver");
  
  err = dd_global_init();
  if (err) {
    goto error;
  }
  
  err = dd_display_driver_init(&dd,
                               dd_SupportedDisplayEnum_Waveshare_7_5_inch_V2_b);
  if (err) {
    goto error;
  }

  puts("Configuring gpio");

  err = dd_display_driver_add_gpio_pin(dd, dd_GpioPinEnum_DC, "/dev/gpiochip0",
                                       25);
  if (err) {
    goto error_dd_cleanup;
  }

  err = dd_display_driver_add_gpio_pin(dd, dd_GpioPinEnum_RST, "/dev/gpiochip0",
                                       17);
  if (err) {
    goto error_dd_cleanup;
  }

  err = dd_display_driver_add_gpio_pin(dd, dd_GpioPinEnum_BSY, "/dev/gpiochip0",
                                       24);
  if (err) {
    goto error_dd_cleanup;
  }

  err = dd_display_driver_add_gpio_pin(dd, dd_GpioPinEnum_PWR, "/dev/gpiochip0",
                                       18);
  if (err) {
    goto error_dd_cleanup;
  }

  puts("Configuring spi");
  err = dd_display_driver_add_spi_master(dd, "/dev/spidev0.0");
  if (err) {
    goto error_dd_cleanup;
  }
  
  puts("Working");
  err = dd_display_driver_read_temp(dd);
  if (err) {
    goto error_dd_cleanup;
  }
  
  puts("I'm done");

  dd_display_driver_destroy(&dd);

  return EXIT_SUCCESS;
  
error_dd_cleanup:
  dd_display_driver_destroy(&dd);
error: {
  char buf[1024];
  dd_error_dumps(err, 1024, buf);
  puts(buf);
}
  return EXIT_FAILURE;
}
