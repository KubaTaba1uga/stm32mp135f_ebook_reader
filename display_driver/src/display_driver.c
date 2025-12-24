#define _POSIX_C_SOURCE 199309L
#include <errno.h>
#include <signal.h>

#include "utils/list.h"
#include "display_driver.h"
#include "gpio/gpio.h"
#include "utils/err.h"
#include "utils/mem.h"

struct dd_DisplayDriver {
  enum dd_SupportedDisplayEnum display;
  struct dd_Gpio gpio;
};

// We need this list for cleanup in handler.
static struct dd_List dd_display_drivers; // dd_display_driver_t

static void dd_display_driver_signal_handler(int signum) {

}

dd_error_t dd_display_driver_init(dd_display_driver_t *dd,
                                  enum dd_SupportedDisplayEnum display) {
  { // Set signal handlers
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
  }

  *dd = dd_malloc(sizeof(struct dd_DisplayDriver));

  (*dd)->display = display;

  dd_errno = dd_gpio_init(&(*dd)->gpio);
  if (dd_errno) {
    dd_ewrap();
    goto error;
  }

  dd_errno = dd_list_append(&dd_display_drivers, *dd);
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

static int dd_display_driver_find_by_ptr(void *rval, void *lval) {
  return rval == lval;
}  

void dd_display_driver_destroy(dd_display_driver_t *dd) {
  if (!dd || !*dd) {
    return;
  }

  dd_list_pop(&dd_display_drivers, *dd, dd_display_driver_find_by_ptr);
  dd_gpio_destroy(&(*dd)->gpio);
  dd_free(*dd);
}

int dd_error_get_code(dd_error_t err) { return err->code; }

const char *dd_error_get_msg(dd_error_t err) { return err->msg; }
