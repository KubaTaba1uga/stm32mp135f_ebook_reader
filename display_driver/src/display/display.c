#include "display.h"
#include "gpio/gpio.h"
#include "utils/err.h"

dd_error_t dd_display_init(struct dd_Display *display) {
  if (!display) {
    dd_errno = dd_errnos(EINVAL, "At leat one of func args is invalid");
    goto error;
  }

  *display = (struct dd_Display){0};

  dd_errno = dd_gpio_init(&display->gpio);
  if (dd_errno) {
    dd_ewrap();
    goto error;
  }

  return 0;

error:
  return dd_errno;
}

void dd_display_destroy(struct dd_Display *display) {
  if (!display) {
    return;
  }

  dd_gpio_destroy(&display->gpio);
};

dd_error_t dd_display_add_gpio_dc(const char *chip_path, int pin_no,
                                  struct dd_Display *display) {
  if (!chip_path || pin_no < 0 || !display) {
    dd_errno = dd_errnos(EINVAL, "At leat one of func args is invalid");
    goto error;
  }

  if (display->dc) {
    dd_errno = dd_errnos(EINVAL, "Only one dc pin allowed");
    goto error;
  }

  dd_errno = dd_gpio_add_pin(chip_path, pin_no, &display->dc, &display->gpio);
  if (dd_errno) {
    dd_ewrap();
    goto error;
  }

  dd_errno = dd_gpio_set_pin_output(display->dc,
                                    true // Active-high
  );
  if (dd_errno) {
    dd_ewrap();
    goto error_pin_cleanup;
  }

  return 0;

error_pin_cleanup:
  dd_gpio_pin_destroy(display->dc, &display->gpio);
error:
  return dd_errno;
}

dd_error_t dd_display_add_gpio_rst(const char *chip_path, int pin_no,
                                   struct dd_Display *display) {
  if (!chip_path || pin_no < 0 || !display) {
    dd_errno = dd_errnos(EINVAL, "At leat one of func args is invalid");
    goto error;
  }

  if (display->rst) {
    dd_errno = dd_errnos(EINVAL, "Only one rst pin allowed");
    goto error;
  }

  dd_errno = dd_gpio_add_pin(chip_path, pin_no, &display->rst, &display->gpio);
  if (dd_errno) {
    dd_ewrap();
    goto error;
  }

  dd_errno = dd_gpio_set_pin_output(display->rst,
                                    false // Active-low
  );
  if (dd_errno) {
    dd_ewrap();
    goto error_pin_cleanup;
  }

  return 0;

error_pin_cleanup:
  dd_gpio_pin_destroy(display->rst, &display->gpio);
error:
  return dd_errno;
}

dd_error_t dd_display_add_gpio_bsy(const char *chip_path, int pin_no,
                                   struct dd_Display *display) {
  if (!chip_path || pin_no < 0 || !display) {
    dd_errno = dd_errnos(EINVAL, "At leat one of func args is invalid");
    goto error;
  }

  if (display->bsy) {
    dd_errno = dd_errnos(EINVAL, "Only one bsy pin allowed");
    goto error;
  }

  dd_errno = dd_gpio_add_pin(chip_path, pin_no, &display->bsy, &display->gpio);
  if (dd_errno) {
    dd_ewrap();
    goto error;
  }

  dd_errno = dd_gpio_set_pin_input(display->bsy);
  if (dd_errno) {
    dd_ewrap();
    goto error_pin_cleanup;
  }

  return 0;

error_pin_cleanup:
  dd_gpio_pin_destroy(display->bsy, &display->gpio);
error:
  return dd_errno;
}

dd_error_t dd_display_add_gpio_pwr(const char *chip_path, int pin_no,
                                   struct dd_Display *display) {
  if (!chip_path || pin_no < 0 || !display) {
    dd_errno = dd_errnos(EINVAL, "At leat one of func args is invalid");
    goto error;
  }

  if (display->pwr) {
    dd_errno = dd_errnos(EINVAL, "Only one pwr pin allowed");
    goto error;
  }

  dd_errno = dd_gpio_add_pin(chip_path, pin_no, &display->pwr, &display->gpio);
  if (dd_errno) {
    dd_ewrap();
    goto error;
  }

  dd_errno = dd_gpio_set_pin_output(display->pwr,
                                    true // Active-high
  );
  if (dd_errno) {
    dd_ewrap();
    goto error_pin_cleanup;
  }

  return 0;

error_pin_cleanup:
  dd_gpio_pin_destroy(display->pwr, &display->gpio);
error:
  return dd_errno;
}
