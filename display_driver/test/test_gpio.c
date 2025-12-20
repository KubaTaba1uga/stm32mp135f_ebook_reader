#include <assert.h>
#include <stdbool.h>
#include <unity.h>

#include "conftest.h"
#include "gpio.h"

void setUp(void) {
  enable_gpiod_chip_close_mock = true;
  enable_gpiod_chip_open_mock = true;
}

void test_gpio_bank_init(void) {
  dd_gpio_bank_t gpio_bank_a;

  int err = dd_gpio_bank_init('A', &gpio_bank_a);
  TEST_ASSERT_EQUAL(0, err);
};
