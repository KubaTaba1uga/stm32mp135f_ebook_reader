#define _GNU_SOURCE
#include "gpio.h"
#include "display_driver.h"
#include "gpiod.h"
#include "utils/err.h"
#include "utils/list.h"
#include "utils/mem.h"
#include <string.h>

dd_error_t dd_gpio_init(struct dd_Gpio *gpio) {
  if (!gpio) {
    dd_errno = dd_errnos(EINVAL, "`gpio` cannot be NULL");
    goto error;
  }

  *gpio = (struct dd_Gpio){
      .chips = DD_LIST_INITIALIZER,
      .pins = DD_LIST_INITIALIZER,
  };

  return 0;

error:
  return dd_errno;
};

static void dd_gpio_chip_destroy(void *data) {
  if (!data) {

    return;
  }

  struct dd_GpioChip *chip = data;
  if (chip->private) {
    gpiod_chip_close(chip->private);
  }
  if (chip->path) {
    dd_free((void *)chip->path);
  }

  dd_free(chip);
}

static void dd_gpio_pin_destroy(void *data) {
  if (!data) {
    return;
  }

  struct dd_GpioPin *pin = data;
  if (pin->private) {
    gpiod_line_release(pin->private);
  }

  dd_free(pin);
}

void dd_gpio_destroy(struct dd_Gpio *gpio) {
  if (!gpio) {
    return;
  }

  dd_list_destroy(&gpio->pins, dd_gpio_pin_destroy);
  dd_list_destroy(&gpio->chips, dd_gpio_chip_destroy);
};

static int dd_gpio_find_chip_by_path(void *node_data, void *data) {
  return strcmp(((struct dd_GpioChip *)node_data)->path, (char *)data);
}

dd_error_t dd_gpio_add_pin(const char *chip_path, int pin_no,
                           struct dd_GpioPin **out, struct dd_Gpio *gpio) {
  if (!chip_path || pin_no < 0 || !out || !gpio) {
    dd_errno = dd_errnos(EINVAL, "At leat one of func args is invalid");
    goto error;
  }

  struct dd_GpioChip *chip = dd_list_get_value(&gpio->chips, (void *)chip_path,
                                               dd_gpio_find_chip_by_path);

  if (!chip) {
    chip = dd_malloc(sizeof(struct dd_GpioChip));
    chip->path = strdup(chip_path);
    dd_list_append(&gpio->chips, chip);
  }

  struct gpiod_chip *gchip = chip->private;
  if (!gchip) {
    gchip = chip->private = gpiod_chip_open(chip_path);
    if (!gchip) {
      dd_errno = dd_errnof(EINVAL, "Cannot open: %s", chip_path);
      goto error;
    }
  }

  struct gpiod_line *gline = gpiod_chip_get_line(gchip, pin_no);
  if (!gline) {
    dd_errno = dd_errnof(EINVAL, "Cannot open: %s:%d", chip->path, pin_no);
    goto error;
  }

  struct dd_GpioPin *pin = *out = dd_malloc(sizeof(struct dd_GpioPin));
  *pin = (struct dd_GpioPin){
      .chip = chip,
      .pin_no = pin_no,
      .private = gline,
  };

  dd_errno = dd_list_append(&gpio->pins, pin);
  if (dd_errno) {
    goto error_pin_cleanup;
  };

  return 0;

error_pin_cleanup:
  dd_free(pin);
error:
  *out = NULL;
  return dd_errno;
}

dd_error_t dd_gpio_set_pin_direction(struct dd_GpioPin *pin, bool is_output) {
  int ret;

  if (is_output) {
    ret = gpiod_line_request_output(pin->private, "gpio", 0);
    if (ret) {
      dd_errno = dd_errnof(errno, "Unable to set direction for %c%d: output",
                           pin->chip->path, pin->pin_no);
      goto error;
    }

  } else {
    ret = gpiod_line_request_input(pin->private, "gpio");
    /* ret = gpiod_line_request_input_flags( */
    /*     (*out)->line, "gpio", GPIOD_LINE_REQUEST_FLAG_BIAS_PULL_DOWN); */
    if (ret) {
      dd_errno = dd_errnof(errno, "Unable to set direction for %c%d: input",
                           pin->chip->path, pin->pin_no);
      goto error;
    }
  }

  pin->is_out = is_output;

  return 0;

error:
  return dd_errno;
};

int dd_gpio_read_pin(struct dd_GpioPin *pin, struct dd_Gpio *gpio) {
  int ret = gpiod_line_get_value(pin->private);
  if (ret) {
    dd_errno = dd_errnof(errno, "Unable to get value for: %c:%d",
                         pin->chip->path, pin->pin_no);
    goto error;
  }

  return ret;

error:
  return -1;
}

dd_error_t dd_gpio_set_pin(int value, struct dd_GpioPin *pin,
                           struct dd_Gpio *gpio) {
  int ret = gpiod_line_set_value(pin->private, value);
  if (ret) {
    dd_errno = dd_errnof(errno, "Unable to set value for: %c:%d=%d",
                         pin->chip->path, pin->pin_no, value);
    goto error;
  }

  return 0;

error:
  return dd_errno;
}
