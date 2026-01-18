#ifndef STM32MP135F_EXAMPLE_H
#define STM32MP135F_EXAMPLE_H
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "display_driver.h"

static dd_display_driver_t dd;

static inline void signal_handler(int signum) {  dd_display_driver_destroy(&dd); }

static inline dd_error_t init_stm32mp135f(void) {
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

  err = dd_display_driver_init( // Reset is done on init in Wvs7in5V2b
      &dd, dd_DisplayDriverEnum_Wvs7in5V2b,
      &(struct dd_Wvs75V2bConfig){
          .dc = {.gpio_chip_path = "/dev/gpiochip8", .pin_no = 0},
          .rst = {.gpio_chip_path = "/dev/gpiochip2", .pin_no = 2},
          .bsy = {.gpio_chip_path = "/dev/gpiochip6", .pin_no = 3},
          .pwr = {.gpio_chip_path = "/dev/gpiochip0", .pin_no = 4},
          .spi = {.spidev_path = "/dev/spidev0.0"},
      });
  if (err) {
    goto error;
  }

  return 0;

error:
  return err;
}

#endif // STM32MP135F_EXAMPLE_H
