#include <unity.h>

#include "conftest.h"
#include "gpio/gpio.h"

static struct dd_Gpio gpio;

void setUp(void) {
  enable_gpiod_chip_close_mock = true;
  enable_gpiod_chip_open_mock = true;
  gpio = (struct dd_Gpio){0};
}

void tearDown(void) {
  dd_gpio_destroy(&gpio);
}

void test_dd_gpio_init(void) {
  dd_error_t err = dd_gpio_init(&gpio);
  TEST_ASSERT_EQUAL(0, err);
}
