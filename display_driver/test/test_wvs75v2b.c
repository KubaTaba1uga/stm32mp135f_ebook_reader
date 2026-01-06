#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <unity.h>

#include "conftest.h"
#include "display_driver.h"
#include "utils/err.h"

static struct dd_Wvs75V2bConfig mk_cfg(bool rotate) {
  return (struct dd_Wvs75V2bConfig){
      .rotate = rotate,
      .dc = {.gpio_chip_path = "/dev/gpiochip0", .pin_no = 10},
      .rst = {.gpio_chip_path = "/dev/gpiochip0", .pin_no = 11},
      .bsy = {.gpio_chip_path = "/dev/gpiochip0", .pin_no = 12},
      .pwr = {.gpio_chip_path = "/dev/gpiochip0", .pin_no = 13},
      .spi = {.spidev_path = "/dev/spidev0.0"},
  };
}

static dd_display_driver_t g_dd = NULL;

void setUp(void) {
  dd_errno = 0;
  g_dd = NULL;

  // enable all mocks
  enable_gpiod_chip_open_mock = true;
  enable_gpiod_chip_close_mock = true;
  enable_gpiod_chip_get_line_mock = true;
  enable_gpiod_line_request_output_mock = true;
  enable_gpiod_line_request_output_flags_mock = true;
  enable_gpiod_line_request_input_mock = true;
  enable_gpiod_line_release_mock = true;
  enable_gpiod_line_get_value_mock = true;
  enable_gpiod_line_set_value_mock = true;
  enable_dd_sleep_ms_mock = true;
  enable_open_mock = true;
  enable_close_mock = true;
  enable_ioctl_mock = true;

  // reset counters
  gpiod_chip_open_mock_called = 0;
  gpiod_chip_close_mock_called = 0;
  gpiod_chip_get_line_mock_called = 0;
  gpiod_line_request_output_mock_called = 0;
  gpiod_line_request_output_flags_mock_called = 0;
  gpiod_line_request_input_mock_called = 0;
  gpiod_line_release_mock_called = 0;
  gpiod_line_get_value_mock_called = 0;
  gpiod_line_set_value_mock_called = 0;

  open_mock_called = 0;
  close_mock_called = 0;
  ioctl_mock_called = 0;
  ioctl_mock_fail_after = -1;
  ioctl_mock_errno = EINVAL;

  open_mock_return = 42;

  gpiod_mock_reset_lines_pool();

  // Make busy-wait finish immediately:
  // driver uses enum value 1 for IDLE :contentReference[oaicite:0]{index=0}
  gpiod_line_get_value_mock_return = 1;
}

void tearDown(void) {
  if (g_dd) {
    dd_display_driver_destroy(&g_dd);
  }
}

// -----------------------------------------------------------------------------
// Tests
// -----------------------------------------------------------------------------

void test_init_calls_probe_and_sets_up_gpio_and_spi(void) {
  struct dd_Wvs75V2bConfig cfg = mk_cfg(false);

  dd_error_t err =
      dd_display_driver_init(&g_dd, dd_DisplayDriverEnum_Wvs7in5V2b, &cfg);
  TEST_ASSERT_EQUAL(0, err);
  TEST_ASSERT_NOT_NULL(g_dd);

  TEST_ASSERT_TRUE(gpiod_chip_open_mock_called == 1);
  TEST_ASSERT_EQUAL(4, gpiod_chip_get_line_mock_called); // dc,rst,bsy,pwr
  TEST_ASSERT_EQUAL(3,
                    gpiod_line_request_output_flags_mock_called); // dc,rst,pwr
  TEST_ASSERT_EQUAL(1, gpiod_line_request_input_mock_called);     // bsy
  TEST_ASSERT_TRUE(open_mock_called >= 1);
  TEST_ASSERT_TRUE(ioctl_mock_called >= 1);
}

void test_clear_uses_public_api_and_toggles_pins_and_spi(void) {
  struct dd_Wvs75V2bConfig cfg = mk_cfg(false);

  dd_error_t err =
      dd_display_driver_init(&g_dd, dd_DisplayDriverEnum_Wvs7in5V2b, &cfg);
  TEST_ASSERT_EQUAL(0, err);

  int prev_set = gpiod_line_set_value_mock_called;
  int prev_get = gpiod_line_get_value_mock_called;
  int prev_ioc = ioctl_mock_called;

  err = dd_display_driver_clear(g_dd, true);
  TEST_ASSERT_EQUAL(0, err);

  // Toggle dc/rst/pwr
  TEST_ASSERT_EQUAL(prev_set + 3, gpiod_line_set_value_mock_called);

  // Should have waited on busy at least once
  TEST_ASSERT_TRUE(gpiod_line_get_value_mock_called > prev_get);

  // SPI bytes go through ioctl(SPI_IOC_MESSAGE) in dd_spi_send_bytes
  TEST_ASSERT_TRUE(ioctl_mock_called > prev_ioc);
}

void test_init_fails_when_spidev_open_fails(void) {
  struct dd_Wvs75V2bConfig cfg = mk_cfg(false);

  open_mock_return = -1;
  errno = ENOENT;

  dd_error_t err =
      dd_display_driver_init(&g_dd, dd_DisplayDriverEnum_Wvs7in5V2b, &cfg);
  TEST_ASSERT_NOT_EQUAL(0, err);
  TEST_ASSERT_NULL(g_dd);
}

void test_destroy_releases_gpio_lines_and_chip_close(void) {
  struct dd_Wvs75V2bConfig cfg = mk_cfg(false);
  TEST_ASSERT_EQUAL(
      0, dd_display_driver_init(&g_dd, dd_DisplayDriverEnum_Wvs7in5V2b, &cfg));

  dd_display_driver_destroy(&g_dd);
  TEST_ASSERT_NULL(g_dd);

  TEST_ASSERT_EQUAL(4, gpiod_line_release_mock_called);
  TEST_ASSERT_EQUAL(1, gpiod_chip_close_mock_called);
}
