#include <assert.h>
#include <stdbool.h>
#include <unity.h>

#include "conftest.h"
#include "gpio.h"
static dd_gpio_bank_t gpio_bank_a;

void setUp(void) {
  enable_gpiod_chip_close_mock = true;
  enable_gpiod_chip_open_mock = true;

  gpio_bank_a = NULL;
}

void tearDown(void){
    dd_gpio_bank_destroy(&gpio_bank_a);
  }


void test_gpio_bank_init(void) {
  int err = dd_gpio_bank_init('A', &gpio_bank_a);
  TEST_ASSERT_EQUAL(0, err);
  TEST_ASSERT_EQUAL(1, gpiod_chip_open_mock_called);
};
