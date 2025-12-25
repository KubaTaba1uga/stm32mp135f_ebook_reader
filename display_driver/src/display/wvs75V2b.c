// TO-DO: use enum fo commands names
//        use different enums for different commands values
#include "display/wvs75V2b.h"
#include "display_driver.h"
#include "gpio/gpio.h"
#include "spi/spi.h"
#include "utils/err.h"
#include "utils/time.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define DD_DISPLAY_IDLE 1
#define DD_DISPLAY_BUSY 0

dd_error_t dd_display_wvs75V2b_init(struct dd_DisplayWvs75V2b *display) {
  if (!display) {
    dd_errno = dd_errnos(EINVAL, "At leat one of func args is invalid");
    goto error;
  }

  *display = (struct dd_DisplayWvs75V2b){0};

  dd_errno = dd_gpio_init(&display->gpio);
  if (dd_errno) {
    dd_ewrap();
    goto error;
  }

  return 0;

error:
  return dd_errno;
}

void dd_display_wvs75V2b_destroy(struct dd_DisplayWvs75V2b *display) {
  if (!display) {
    return;
  }

  dd_gpio_set_pin(0, display->pwr, &display->gpio);
  dd_gpio_set_pin(0, display->rst, &display->gpio);
  dd_gpio_set_pin(0, display->dc, &display->gpio);
  dd_gpio_destroy(&display->gpio);
};

dd_error_t dd_display_wvs75V2b_add_gpio_dc(const char *chip_path, int pin_no,
                                           struct dd_DisplayWvs75V2b *display) {
  if (!chip_path || pin_no < 0 || !display) {
    dd_errno = dd_errnos(EINVAL, "At leat one of func args is invalid");
    goto error;
  }

  if (display->dc) {
    dd_errno = dd_errnos(EINVAL, "Only one dc pin allowed");
    goto error;
  }

  dd_errno = dd_gpio_add_pin(chip_path, pin_no, &display->dc, &display->gpio);
  DD_TRY(dd_errno);

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

dd_error_t
dd_display_wvs75V2b_add_gpio_rst(const char *chip_path, int pin_no,
                                 struct dd_DisplayWvs75V2b *display) {
  if (!chip_path || pin_no < 0 || !display) {
    dd_errno = dd_errnos(EINVAL, "At leat one of func args is invalid");
    goto error;
  }

  if (display->rst) {
    dd_errno = dd_errnos(EINVAL, "Only one rst pin allowed");
    goto error;
  }

  dd_errno = dd_gpio_add_pin(chip_path, pin_no, &display->rst, &display->gpio);
  DD_TRY(dd_errno);

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

dd_error_t
dd_display_wvs75V2b_add_gpio_bsy(const char *chip_path, int pin_no,
                                 struct dd_DisplayWvs75V2b *display) {
  if (!chip_path || pin_no < 0 || !display) {
    dd_errno = dd_errnos(EINVAL, "At leat one of func args is invalid");
    goto error;
  }

  if (display->bsy) {
    dd_errno = dd_errnos(EINVAL, "Only one bsy pin allowed");
    goto error;
  }

  dd_errno = dd_gpio_add_pin(chip_path, pin_no, &display->bsy, &display->gpio);
  DD_TRY(dd_errno);

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

dd_error_t
dd_display_wvs75V2b_add_gpio_pwr(const char *chip_path, int pin_no,
                                 struct dd_DisplayWvs75V2b *display) {
  if (!chip_path || pin_no < 0 || !display) {
    dd_errno = dd_errnos(EINVAL, "At leat one of func args is invalid");
    goto error;
  }

  if (display->pwr) {
    dd_errno = dd_errnos(EINVAL, "Only one pwr pin allowed");
    goto error;
  }

  dd_errno = dd_gpio_add_pin(chip_path, pin_no, &display->pwr, &display->gpio);
  DD_TRY(dd_errno);

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

static void dd_display_wvs75V2b_wait(struct dd_DisplayWvs75V2b *display) {
  puts("Busy waiting");
  while (dd_gpio_read_pin(display->bsy, &display->gpio) != DD_DISPLAY_IDLE) {
    dd_sleep_ms(10);
  }
  puts("Waiting done");
}

dd_error_t dd_display_wvs75V2b_reset(struct dd_DisplayWvs75V2b *display) {
  if (!display || !display->pwr || !display->rst) {
    dd_errno = dd_errnos(
        EINVAL, "`display`, `display->pwr` and `display->rst` cannot be NULL");
    goto error;
  }

  if (dd_gpio_read_pin(display->pwr, &display->gpio) != 1) {
    dd_errno = dd_gpio_set_pin(1, display->pwr, &display->gpio);
    DD_TRY(dd_errno);
    dd_sleep_ms(100);
  }

  dd_errno = dd_gpio_set_pin(0, display->rst, &display->gpio);
  DD_TRY(dd_errno);
  dd_sleep_ms(200);

  dd_errno = dd_gpio_set_pin(1, display->rst, &display->gpio);
  DD_TRY(dd_errno);
  dd_sleep_ms(5);

  dd_errno = dd_gpio_set_pin(0, display->rst, &display->gpio);
  DD_TRY(dd_errno);
  dd_sleep_ms(200);

  dd_display_wvs75V2b_wait(display); // Give chip time to reset itself

  return 0;

error:
  return dd_errno;
};

dd_error_t
dd_display_wvs75V2b_add_spi_master(const char *spidev_path,
                                   struct dd_DisplayWvs75V2b *display) {
  if (!spidev_path || !display) {
    dd_errno = dd_errnos(EINVAL, "`display` and `spidev_path` cannot be NULL");
    goto error;
  }

  dd_errno = dd_spi_init(spidev_path, &display->spi);
  DD_TRY(dd_errno);

  return 0;

error:
  return dd_errno;
};

static dd_error_t
dd_display_wvs75V2b_send_cmd(struct dd_DisplayWvs75V2b *display, uint8_t cmd) {
  dd_errno = dd_gpio_set_pin(0, display->dc, &display->gpio);
  DD_TRY(dd_errno);

  dd_errno = dd_spi_send_byte(cmd, &display->spi);
  DD_TRY(dd_errno);

  return 0;

error:
  return dd_errno;
}

static dd_error_t
dd_display_wvs75V2b_send_data(struct dd_DisplayWvs75V2b *display, uint8_t *data,
                              uint32_t len) {
  dd_errno = dd_gpio_set_pin(1, display->dc, &display->gpio);
  DD_TRY(dd_errno);

  dd_errno = dd_spi_send_bytes(data, len, &display->spi);
  DD_TRY(dd_errno);

  return 0;

error:
  return dd_errno;
}

dd_error_t dd_display_wvs75V2b_power_on(struct dd_DisplayWvs75V2b *display) {
  if (dd_gpio_read_pin(display->pwr, &display->gpio) != 1) {
    dd_errno = dd_gpio_set_pin(1, display->pwr, &display->gpio);
    DD_TRY(dd_errno);
    dd_sleep_ms(100);
  }

  dd_errno =
      dd_display_wvs75V2b_send_cmd(display, 0x01); // Write to power settings
  DD_TRY(dd_errno);

  dd_errno = dd_display_wvs75V2b_send_data(display,
                                           (uint8_t[]){
                                               0x07, // LDO disabled, VDHR
                                               0x07, // VGH=20V,VGL=-20V
                                               0x3F, // VDH=15V
                                               0x3F, // VDL=-15V
                                           },
                                           4);
  DD_TRY(dd_errno);

  dd_errno = dd_display_wvs75V2b_send_cmd(display,
                                          0x06); // Write to booster soft start
  DD_TRY(dd_errno);

  // I'm not sure what this part does but it is in mainline driver
  dd_errno = dd_display_wvs75V2b_send_data(display,
                                           (uint8_t[]){
                                               0x17,
                                               0x17,
                                               0x28,
                                               0x17,
                                           },
                                           4);
  DD_TRY(dd_errno);

  dd_errno = dd_display_wvs75V2b_send_cmd(display,
                                          0x04); // Power on the screen
  DD_TRY(dd_errno);
  dd_sleep_ms(100);
  dd_display_wvs75V2b_wait(display);

  dd_errno = dd_display_wvs75V2b_send_cmd(display,
                                          0x00); // Write to panel settings
  DD_TRY(dd_errno);

  dd_errno = dd_display_wvs75V2b_send_data(
      display,
      (uint8_t[]){
          0x0F, // Gate scan direction up, Source Shift Direction Rigth, Booster
                // on, Do not perform soft reset
      },
      1);
  DD_TRY(dd_errno);

  dd_errno = dd_display_wvs75V2b_send_cmd(display,
                                          0x61); // Write to resolution settings
  DD_TRY(dd_errno);
  dd_errno = dd_display_wvs75V2b_send_data(display,
                                           (uint8_t[]){
                                               0x03,
                                               0x20,
                                               0x01,
                                               0xE0,
                                           },
                                           4);
  DD_TRY(dd_errno);

  dd_errno = dd_display_wvs75V2b_send_cmd(display,
                                          0x15); // Write to resolution settings
  DD_TRY(dd_errno);
  dd_errno = dd_display_wvs75V2b_send_data(display,
                                           (uint8_t[]){
                                               0x00,
                                           },
                                           1);
  DD_TRY(dd_errno);

  dd_errno =
      dd_display_wvs75V2b_send_cmd(display,
                                   0x50); // VCOM AND D ATA INTERVAL SETTING
  DD_TRY(dd_errno);
  dd_errno = dd_display_wvs75V2b_send_data(display,
                                           (uint8_t[]){
                                               0x11,
                                               0x07,
                                           },
                                           2);
  DD_TRY(dd_errno);

  dd_errno = dd_display_wvs75V2b_send_cmd(display,
                                          0x60); // TCON SETTING
  DD_TRY(dd_errno);
  dd_errno = dd_display_wvs75V2b_send_data(display,
                                           (uint8_t[]){
                                               0x22,
                                           },
                                           1);
  DD_TRY(dd_errno);

  return 0;

error:
  return dd_errno;
}

dd_error_t dd_display_wvs75V2b_clear(struct dd_DisplayWvs75V2b *display) {
  const int width = 800;
  const int heigth = 480;
  uint8_t
      buf[(width / 8) * heigth]; // width/8 cause we send in bytes not in bits

  dd_errno = dd_display_wvs75V2b_send_cmd(display,
                                          0x10); // DATA START TRANSMISSION
  DD_TRY(dd_errno);
  memset(buf, 0xFF, sizeof(buf));
  for (int i = 0; i < sizeof(buf); i++) {
    dd_errno = dd_display_wvs75V2b_send_data(display,
                                             (uint8_t[]){
                                                 buf[i],
                                             },
                                             1);
    DD_TRY(dd_errno);
  }

  /* dd_errno = dd_display_wvs75V2b_send_data(display, buf, sizeof(buf)); */
  /* DD_TRY(dd_errno); */

  dd_errno = dd_display_wvs75V2b_send_cmd(
      display,
      0x13); // DATA START TRANSMISSION 2
             // How version 1 differ from version 2?
  DD_TRY(dd_errno);
  memset(buf, 0x00, sizeof(buf));
  for (int i = 0; i < sizeof(buf); i++) {
    dd_errno = dd_display_wvs75V2b_send_data(display,
                                             (uint8_t[]){
                                                 buf[i],
                                             },
                                             1);
    DD_TRY(dd_errno);
  }

  /* dd_errno = dd_display_wvs75V2b_send_data(display, buf, sizeof(buf)); */

  dd_errno = dd_display_wvs75V2b_send_cmd(display,
                                          0x12); // DISPLAY REFRESH
  DD_TRY(dd_errno);
  dd_sleep_ms(100);
  dd_display_wvs75V2b_wait(display);

  return 0;

error:
  return dd_errno;
}

dd_error_t dd_display_wvs75V2b_power_off(struct dd_DisplayWvs75V2b *display) {
  dd_errno = dd_display_wvs75V2b_send_cmd(display,
                                          0x02); // POWER OFF
  DD_TRY(dd_errno);
  dd_display_wvs75V2b_wait(display);

  dd_errno = dd_display_wvs75V2b_send_cmd(display,
                                          0x07); // DEEP SLEEP
  DD_TRY(dd_errno);
  dd_errno = dd_display_wvs75V2b_send_data(display,
                                           (uint8_t[]){
                                               0xA5,
                                           },
                                           1);
  DD_TRY(dd_errno);

  dd_sleep_ms(2000);

  return 0;

error:
  return dd_errno;
}
