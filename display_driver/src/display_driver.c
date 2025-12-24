#define _POSIX_C_SOURCE 199309L
#include <errno.h>
#include <signal.h>
#include <stdio.h>

#include "display/wvs75V2b.h"
#include "display_driver.h"
#include "utils/err.h"
#include "utils/list.h"
#include "utils/mem.h"

struct dd_DisplayDriver {
  enum dd_SupportedDisplayEnum display_model;
  struct dd_DisplayWvs75V2b display_wvs_75v2b;
};

// We need this list for cleanup in handler.
static struct dd_List dd_display_drivers; // dd_display_driver_t

static void dd_display_driver_cleanup(void *data);
static void dd_display_driver_signal_handler(int signum);
static dd_error_t dd_display_driver_set_up_signals(void);

dd_error_t dd_global_init(void) {
  dd_errno = dd_display_driver_set_up_signals();
  if (dd_errno) {
    dd_ewrap();
    goto error;
  }

  return 0;

error:
  return dd_errno;
};

dd_error_t dd_display_driver_init(dd_display_driver_t *dd,
                                  enum dd_SupportedDisplayEnum display_model) {
  *dd = dd_malloc(sizeof(struct dd_DisplayDriver));
  **dd = (struct dd_DisplayDriver){0};

  if (display_model == dd_SupportedDisplayEnum_Waveshare_7_5_inch_V2_b) {
    dd_errno = dd_display_wvs75V2b_init(&(*dd)->display_wvs_75v2b);
    if (dd_errno) {
      goto error;
    }
  } else {
    dd_errno = dd_errnos(EINVAL, "Unkown display model");
    goto error;
  }

  (*dd)->display_model = display_model;

  dd_errno = dd_list_append(&dd_display_drivers, *dd);
  if (dd_errno) {
    dd_ewrap();
    goto error_display_cleanup;
  }

  return 0;

error_display_cleanup:
  switch (display_model) {
  case dd_SupportedDisplayEnum_Waveshare_7_5_inch_V2_b:
    dd_display_wvs75V2b_destroy(&(*dd)->display_wvs_75v2b);
    break;
  }
error:
  dd_free(*dd);
  *dd = NULL;
  return dd_errno;
}

dd_error_t dd_display_driver_add_gpio_pin(dd_display_driver_t dd,
                                          enum dd_GpioPinEnum pin_cls,
                                          const char *gpio_chip, int pin_no) {
  if (dd->display_model == dd_SupportedDisplayEnum_Waveshare_7_5_inch_V2_b) {
    switch (pin_cls) {
    case dd_GpioPinEnum_DC:
      dd_errno = dd_display_wvs75V2b_add_gpio_dc(gpio_chip, pin_no,
                                                 &dd->display_wvs_75v2b);
      if (dd_errno) {
        dd_ewrap();
        goto error;
      }
      break;
    case dd_GpioPinEnum_RST:
      dd_errno = dd_display_wvs75V2b_add_gpio_rst(gpio_chip, pin_no,
                                                  &dd->display_wvs_75v2b);
      if (dd_errno) {
        dd_ewrap();
        goto error;
      }
      break;
    case dd_GpioPinEnum_BSY:
      dd_errno = dd_display_wvs75V2b_add_gpio_bsy(gpio_chip, pin_no,
                                                  &dd->display_wvs_75v2b);
      if (dd_errno) {
        dd_ewrap();
        goto error;
      }
      break;
    case dd_GpioPinEnum_PWR:
      dd_errno = dd_display_wvs75V2b_add_gpio_pwr(gpio_chip, pin_no,
                                                  &dd->display_wvs_75v2b);
      if (dd_errno) {
        dd_ewrap();
        goto error;
      }
      break;
    }
  } else {
    dd_errno = dd_errnos(
        EINVAL, "This operation is not supported on this display model");
    goto error;
  }

  return 0;

error:
  return dd_errno;
}

dd_error_t dd_display_driver_add_spi_slave(dd_display_driver_t dd,
                                           const char *spi_chip) {
  /* if (dd->display_model == dd_SupportedDisplayEnum_Waveshare_7_5_inch_V2_b) {
   */
  /*   dd_errno = */
  /*       dd_display_wvs75V2b_add_spi_slave(spi_chip, &dd->display_wvs_75v2b);
   */
  /*   if (dd_errno) { */
  /*     dd_ewrap(); */
  /*     goto error; */
  /*   } */
  /* } */

  return 0;

  /* error: */
  /* return dd_errno; */
}

void dd_display_driver_destroy(dd_display_driver_t *dd) {
  if (!dd || !*dd) {
    return;
  }

  dd_list_pop(&dd_display_drivers, *dd, dd_list_eq, NULL);
  dd_display_driver_cleanup(*dd);
  *dd = NULL;
}

dd_error_t dd_display_driver_reset(dd_display_driver_t dd) {
  if (dd->display_model == dd_SupportedDisplayEnum_Waveshare_7_5_inch_V2_b) {
    dd_errno = dd_display_wvs75V2b_reset(&dd->display_wvs_75v2b);
    if (dd_errno) {
      dd_ewrap();
      goto error;
    }
  } else {
    dd_errno = dd_errnos(
        EINVAL, "This operation is not supported on this display model");
    goto error;
  }

  return 0;

error:
  return dd_errno;
}

int dd_error_get_code(dd_error_t err) { return err->code; }

const char *dd_error_get_msg(dd_error_t err) { return err->msg; }

static void dd_display_driver_signal_handler(int signum) {
  dd_list_destroy(&dd_display_drivers, dd_display_driver_cleanup);
}

static void dd_display_driver_cleanup(void *data) {
  if (!data) {
    return;
  }

  dd_display_driver_t dd = data;

  if (dd->display_model == dd_SupportedDisplayEnum_Waveshare_7_5_inch_V2_b) {
    dd_display_wvs75V2b_destroy(&dd->display_wvs_75v2b);
  }

  dd_free(dd);
}

static dd_error_t dd_display_driver_set_up_signals(void) {
  struct sigaction sa = {0};

  sa.sa_handler = dd_display_driver_signal_handler;
  sigemptyset(&sa.sa_mask);

  int signals[] = {SIGINT, SIGTERM, SIGHUP};

  for (size_t i = 0; i < sizeof(signals) / sizeof(signals[0]); i++) {
    if (sigaction(signals[i], &sa, NULL) == -1) {
      dd_errno = dd_errnos(errno, "Cannot set signal handler"
                                  " for display driver");
      goto error;
    }
  }

  return 0;

error:
  return dd_errno;
}
