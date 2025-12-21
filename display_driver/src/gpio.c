#include <ctype.h>
#include <errno.h>
#include <gpiod.h>
#include <stdio.h>
#include <stdlib.h>

#include "gpio.h"
#include "utils/err.h"
#include "utils/mem.h"

struct dd_gpio_bank {
  struct gpiod_chip *chip;
  char bank;
};

struct dd_gpio_pin {
  struct gpiod_line *line;
  struct dd_gpio_bank *bank;
  int pin;
};

int dd_gpio_bank_init(char bank, dd_gpio_bank_t *out) {
  dd_gpio_bank_t gpio_bank = *out = dd_malloc(sizeof(struct dd_gpio_bank));

  int bank_number = -1;
  if (isupper(bank)) {
    bank_number = bank - 'A';
  } else if (islower(bank)) {
    bank_number = bank - 'a';
  }

  if (bank_number == -1) {
    dd_errno = dd_errnof(EINVAL, "Invalid bank: `%c`", bank);
    goto error;
  };

  gpio_bank->bank = bank;

  char chip_path[255] = {0};
  sprintf(chip_path, "/dev/gpiochip%d", bank_number);

  gpio_bank->chip = gpiod_chip_open(chip_path);
  if (gpio_bank->chip == NULL) {
    dd_errno = dd_errnof(EINVAL, "Cannot open: %s", chip_path);
    goto error;
  }

  return 0;

error:
  *out = NULL;
  dd_free(gpio_bank);
  return dd_ereturn(-1);
};

void dd_gpio_bank_destroy(dd_gpio_bank_t *out) {
  if (!out || !*out) {
    return;
  }

  gpiod_chip_close((*out)->chip);
  dd_free(*out);
  *out = NULL;
};

int dd_gpio_pin_init(int pin, enum dd_gpio_dir dir, dd_gpio_bank_t bank,
                     dd_gpio_pin_t *out) {
  dd_gpio_pin_t gpio_pin = dd_malloc(sizeof(struct dd_gpio_pin));
  int ret;

  *gpio_pin = (struct dd_gpio_pin){
      .bank = bank,
      .pin = pin,
  };

  *out = gpio_pin;

  (*out)->line = gpiod_chip_get_line(bank->chip, pin);
  if (!(*out)->line) {
    dd_errno = dd_errnof(-1, "Unable to get line for %c%d", (*out)->bank->bank,
                         (*out)->pin);
    goto error;
  }

  switch (dir) {
  case dd_gpio_dir_input:
    ret = gpiod_line_request_input_flags(
        (*out)->line, "gpio", GPIOD_LINE_REQUEST_FLAG_BIAS_PULL_DOWN);
    if (ret != 0) {
      dd_errno = dd_errnof(-1, "Unable to set direction line for %c%d: input",
                           (*out)->bank->bank, (*out)->pin);
      goto error;
    }
    break;

  case dd_gpio_dir_output:
    ret = gpiod_line_request_output((*out)->line, "gpio", 0);
    if (ret != 0) {
      dd_errno = dd_errnof(-1, "Unable to set direction line for %c%d: output",
                           (*out)->bank->bank, (*out)->pin);
      goto error;
    }
  }

  return 0;

error:
  return dd_ereturn(-1);
};

void dd_gpio_pin_destroy(dd_gpio_pin_t *out) {
  if (!out || !*out) {
    return;
  }

  gpiod_line_release((*out)->line);

  *out = NULL;
};

int dd_gpio_pin_read(dd_gpio_pin_t pin) {
  pin->line = gpiod_chip_get_line(pin->bank->chip, pin->pin);
  if (!pin->line) {
    dd_errno =
        dd_errnof(-1, "Unable to get line for %c%d", pin->bank->bank, pin->pin);
    goto error;
  }

  int ret = gpiod_line_get_value(pin->line);
  if (ret != 0) {
    dd_errno = dd_errnof(-1, "Unable to get value line for %c%d",
                         pin->bank->bank, pin->pin);
    goto error;
  }

  return ret;

error:
  return dd_ereturn(-1);
}

int dd_gpio_pin_write(dd_gpio_pin_t pin, int value) {
  pin->line = gpiod_chip_get_line(pin->bank->chip, pin->pin);
  if (!pin->line) {
    dd_errno =
        dd_errnof(-1, "Unable to get line for %c%d", pin->bank->bank, pin->pin);
    goto error;
  }

  int ret = gpiod_line_set_value(pin->line, value);
  if (ret != 0) {
    dd_errno = dd_errnof(-1, "Unable to set value line for %c%d: %d",
                         pin->bank->bank, pin->pin, value);
    goto error;
  }

  return 0;

error:
  return dd_ereturn(-1);
}
