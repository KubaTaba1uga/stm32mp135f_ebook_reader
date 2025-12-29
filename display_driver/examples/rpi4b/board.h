#ifndef RPI4B_EXAMPLE_H
#define RPI4B_EXAMPLE_H
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "display_driver.h"

static dd_wvs75v2b_t dd;

static inline void signal_handler(int signum) { dd_wvs75v2b_destroy(&dd); }

static inline dd_error_t init_rpi4b(void) {
  dd = NULL;
  struct sigaction sa = {0};
  sa.sa_handler = signal_handler;
  sigemptyset(&sa.sa_mask);
  int signals[] = {SIGINT, SIGTERM, SIGHUP};
  for (size_t i = 0; i < sizeof(signals) / sizeof(signals[0]); i++) {
    if (sigaction(signals[i], &sa, NULL) == -1) {
      puts("Cannot set signal handler"
           " for display driver");
      exit(1);
    }
  }

  dd_error_t err;

  err = dd_wvs75v2b_init(&dd);
  if (err) {
    goto error;
  }

  puts("Configuring gpio");
  err = dd_wvs75v2b_set_up_gpio_dc(dd, "/dev/gpiochip0", 25);
  if (err) {
    goto error_dd_cleanup;
  }

  err = dd_wvs75v2b_set_up_gpio_rst(dd, "/dev/gpiochip0", 17);
  if (err) {
    goto error_dd_cleanup;
  }

  err = dd_wvs75v2b_set_up_gpio_bsy(dd, "/dev/gpiochip0", 24);
  if (err) {
    goto error_dd_cleanup;
  }

  err = dd_wvs75v2b_set_up_gpio_pwr(dd, "/dev/gpiochip0", 18);
  if (err) {
    goto error_dd_cleanup;
  }

  puts("Configuring spi");
  err = dd_wvs75v2b_set_up_spi_master(dd, "/dev/spidev0.0");
  if (err) {
    goto error_dd_cleanup;
  }

  return 0;

error_dd_cleanup:
  dd_wvs75v2b_destroy(&dd);
error:
  return err;
}

#endif // RPI4B_EXAMPLE_H
