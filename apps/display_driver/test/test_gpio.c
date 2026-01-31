#include <unity.h>
#include <stdbool.h>
#include <errno.h>

#include "gpio/gpio.h"
#include "utils/err.h"
#include "conftest.h"

/* ============================================================
 * Fixture
 * ============================================================ */
static struct dd_Gpio gpio;

static void enable_all_gpio_mocks(void) {
  enable_gpiod_chip_open_mock = true;
  enable_gpiod_chip_close_mock = true;
  enable_gpiod_chip_get_line_mock = true;
  enable_gpiod_line_request_output_mock = true;
  enable_gpiod_line_request_input_mock = true;
  enable_gpiod_line_release_mock = true;
}

static void reset_all_mock_counters(void) {
  gpiod_chip_open_mock_called = 0;
  gpiod_chip_close_mock_called = 0;
  gpiod_chip_get_line_mock_called = 0;
  gpiod_line_request_output_mock_called = 0;
  gpiod_line_request_input_mock_called = 0;
  gpiod_line_release_mock_called = 0;
}

void setUp(void) {
  dd_errno = 0;

  enable_all_gpio_mocks();
  reset_all_mock_counters();
  gpiod_mock_reset_lines_pool();

  TEST_ASSERT_EQUAL(0, dd_gpio_init(&gpio));
}

void tearDown(void) {
  dd_gpio_destroy(&gpio);
}

void test_dd_gpio_init_init_lists_heads(void) {
  /* poison memory to ensure init actually writes */
  memset(&gpio, 0xAA, sizeof(gpio));

  dd_error_t err = dd_gpio_init(&gpio);
  TEST_ASSERT_EQUAL(0, err);
  TEST_ASSERT_EQUAL(0, dd_errno);

  /* DD_LIST_INITIALIZER should result in empty lists */
  TEST_ASSERT_NULL(gpio.chips.head);
  TEST_ASSERT_NULL(gpio.pins.head);
}

void test_dd_gpio_add_pin_opens_chip_and_gets_line_once(void) {
  struct dd_GpioPin *pin = NULL;

  dd_error_t err = dd_gpio_add_pin("/dev/gpiochip0", 5, &pin, &gpio);
  TEST_ASSERT_EQUAL(0, err);
  TEST_ASSERT_NOT_NULL(pin);

  TEST_ASSERT_EQUAL(1, gpiod_chip_open_mock_called);
  TEST_ASSERT_EQUAL(1, gpiod_chip_get_line_mock_called);

  TEST_ASSERT_EQUAL(5, pin->pin_no);
  TEST_ASSERT_NOT_NULL(pin->chip);
  TEST_ASSERT_EQUAL_STRING("/dev/gpiochip0", pin->chip->path);
  TEST_ASSERT_NOT_NULL(pin->private); /* gpiod_line* from pool */
}

void test_dd_gpio_add_pin_reuses_chip_for_same_path_only_opens_once(void) {
  struct dd_GpioPin *p1 = NULL;
  struct dd_GpioPin *p2 = NULL;

  TEST_ASSERT_EQUAL(0, dd_gpio_add_pin("/dev/gpiochip0", 1, &p1, &gpio));
  TEST_ASSERT_EQUAL(0, dd_gpio_add_pin("/dev/gpiochip0", 2, &p2, &gpio));

  TEST_ASSERT_NOT_NULL(p1);
  TEST_ASSERT_NOT_NULL(p2);

  TEST_ASSERT_EQUAL(1, gpiod_chip_open_mock_called);
  TEST_ASSERT_EQUAL(2, gpiod_chip_get_line_mock_called);

  TEST_ASSERT_EQUAL_PTR(p1->chip, p2->chip);
}

void test_dd_gpio_add_pin_opens_new_chip_for_different_path(void) {
  struct dd_GpioPin *p1 = NULL;
  struct dd_GpioPin *p2 = NULL;

  TEST_ASSERT_EQUAL(0, dd_gpio_add_pin("/dev/gpiochip0", 1, &p1, &gpio));
  TEST_ASSERT_EQUAL(0, dd_gpio_add_pin("/dev/gpiochip9", 1, &p2, &gpio));

  TEST_ASSERT_EQUAL(2, gpiod_chip_open_mock_called);
  TEST_ASSERT_EQUAL(2, gpiod_chip_get_line_mock_called);

  TEST_ASSERT_NOT_EQUAL(p1->chip, p2->chip);
}

void test_dd_gpio_set_pin_input_calls_gpiod_request_input_and_sets_is_out_false(void) {
  struct dd_GpioPin *pin = NULL;
  TEST_ASSERT_EQUAL(0, dd_gpio_add_pin("/dev/gpiochip0", 3, &pin, &gpio));

  dd_error_t err = dd_gpio_set_pin_input(pin);
  TEST_ASSERT_EQUAL(0, err);

  TEST_ASSERT_EQUAL(1, gpiod_line_request_input_mock_called);
  TEST_ASSERT_FALSE(pin->is_out);
}

void test_dd_gpio_pin_destroy_releases_line_and_removes_pin(void) {
  struct dd_GpioPin *p1 = NULL;
  struct dd_GpioPin *p2 = NULL;

  TEST_ASSERT_EQUAL(0, dd_gpio_add_pin("/dev/gpiochip0", 1, &p1, &gpio));
  TEST_ASSERT_EQUAL(0, dd_gpio_add_pin("/dev/gpiochip0", 2, &p2, &gpio));

  /* destroy p1 */
  dd_gpio_pin_destroy(p1, &gpio);

  TEST_ASSERT_EQUAL(1, gpiod_line_release_mock_called);

  /* ensure p2 still valid: destroy it too -> release called twice */
  dd_gpio_pin_destroy(p2, &gpio);
  TEST_ASSERT_EQUAL(2, gpiod_line_release_mock_called);
}

void test_dd_gpio_destroy_closes_each_chip_once(void) {
  struct dd_GpioPin *p1 = NULL;
  struct dd_GpioPin *p2 = NULL;
  struct dd_GpioPin *p3 = NULL;

  TEST_ASSERT_EQUAL(0, dd_gpio_add_pin("/dev/gpiochip0", 1, &p1, &gpio));
  TEST_ASSERT_EQUAL(0, dd_gpio_add_pin("/dev/gpiochip0", 2, &p2, &gpio));
  TEST_ASSERT_EQUAL(0, dd_gpio_add_pin("/dev/gpiochip7", 3, &p3, &gpio));

  /* 2 unique chips => close called 2x on destroy */
  dd_gpio_destroy(&gpio);
  TEST_ASSERT_EQUAL(2, gpiod_chip_close_mock_called);
}
