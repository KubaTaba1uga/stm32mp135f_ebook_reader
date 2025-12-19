#include <ctype.h>
#include <errno.h>
#include <gpiod.h>
#include <stdio.h>

#include "error.h"
#include "gpio.h"
#include "utils/mem.h"

struct gpio_bank {
  struct gpiod_chip *chip;
  char bank;
};

struct gpio_pin {
  struct gpio_bank *bank;
  struct gpiod_line *line;
  int pin;
};

int gpio_bank_init(char bank, gpio_bank_t *out) {
  gpio_bank_t gpio_bank = mem_alloc(sizeof(struct gpio_bank));

  int bank_number = -1;
  if (isupper(bank)) {
    bank_number = bank - 'A';
  } else if (islower(bank)) {
    bank_number = bank - 'a';
  }

  if (bank_number == -1) {
    cdk_errno = cdk_errnof(EINVAL, "Invalid bank: `%c`", bank);
    goto error;
  };

  gpio_bank->bank = bank;

  char chip_path[255] = {0};
  sprintf(chip_path, "/dev/gpiochip%d", bank_number);

  gpio_bank->chip = gpiod_chip_open(chip_path);
  if (*out == NULL) {
    cdk_errno = cdk_errnof(EINVAL, "Cannot open: %s", chip_path);
    goto error;
  }

  return 0;

error:
  mem_free(gpio_bank);
  return cdk_ereturn(-1);
};

void gpio_bank_destroy(gpio_bank_t *out) {
  if (!out || !*out) {
    return;
  }

  gpiod_chip_close((*out)->chip);
  mem_free(*out);
  *out = NULL;
};

int gpio_pin_init(int pin, gpio_bank_t bank, gpio_pin_t *out) {  
  gpio_pin_t gpio_pin = mem_alloc(sizeof(struct gpio_pin));
  
  *gpio_pin = (struct gpio_pin){
      .bank = bank,
      .pin = pin,
  };

  *out = gpio_pin;

  return 0;
};

void gpio_pin_destroy(gpio_pin_t *out) {
  if (!out || !*out) {
    return;
  }

  if ((*out)->line) {
    gpiod_line_release((struct gpiod_line *)out->line);
  }

  *out = NULL;
};

int gpio_pin_read( gpio_pin_t pin) {
  struct gpiod_line *gpioline =
      gpiod_chip_get_line((struct gpiod_chip *)pin->bank, pin->pin);
  if (gpioline == NULL) {
    cdk_errno = cdk_errnof(-1, "Unable to get line for %c%d", pin->bank->bank, pin->pin);
    return -1;
  }

  ret = gpiod_line_set_value(gpioline, value);
  if (ret != 0) {
    STM_GPIOD_Debug("failed to write value! : Pin%d\n", Pin);
    return -1;
  }
}

int gpio_write(struct gpio *pin) {}
