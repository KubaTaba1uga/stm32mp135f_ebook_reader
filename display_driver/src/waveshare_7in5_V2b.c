#include <stdint.h>
#include <stdio.h>

#include "display_driver.h"
#include "gpio/gpio.h"
#include "image.h"
#include "spi/spi.h"
#include "utils/err.h"
#include "utils/mem.h"
#include "utils/time.h"
#include "waveshare_7in5_V2b.h"

#define DD_WVS75V2B_WIDTH 480
#define DD_WVS75V2B_HEIGTH 800

dd_error_t dd_wvs75v2b_init(dd_wvs75v2b_t *dd) {
  if (!dd) {
    dd_errno = dd_errnos(EINVAL, "`dd` cannot be NULL");
    goto error;
  }

  *dd = dd_malloc(sizeof(struct dd_Wvs75V2b));

  dd_errno = dd_gpio_init(&(*dd)->gpio);
  if (dd_errno) {
    dd_ewrap();
    goto error_dd_cleanup;
  }

  return 0;

error_dd_cleanup:
  dd_free(*dd);
  *dd = NULL;
error:
  return dd_errno;
};

void dd_wvs75v2b_destroy(dd_wvs75v2b_t *dd) {
  if (!dd || !*dd) {
    return;
  }

  dd_gpio_set_pin(0, (*dd)->pwr, &(*dd)->gpio);
  dd_gpio_set_pin(0, (*dd)->rst, &(*dd)->gpio);
  dd_gpio_set_pin(0, (*dd)->dc, &(*dd)->gpio);
  dd_gpio_destroy(&(*dd)->gpio);
  dd_free(*dd);
  *dd = NULL;
}

dd_error_t dd_wvs75v2b_set_up_gpio_dc(dd_wvs75v2b_t dd,
                                      const char *gpio_chip_path, int pin_no) {
  if (!dd || !gpio_chip_path || pin_no < 0) {
    dd_errno = dd_errnos(EINVAL, "`dd` and `gpio_chip_path` cannot be NULL, "
                                 "`pin_no` has to be positive");
    goto error;
  }

  if (dd->dc) {
    dd_errno = dd_errnos(EINVAL, "Only one dc pin allowed");
    goto error;
  }

  dd_errno = dd_gpio_add_pin(gpio_chip_path, pin_no, &dd->dc, &dd->gpio);
  DD_TRY(dd_errno);

  dd_errno = dd_gpio_set_pin_output(dd->dc,
                                    true // Active-high
  );
  DD_TRY_CATCH(dd_errno, error_pin_cleanup);

  return 0;

error_pin_cleanup:
  dd_gpio_pin_destroy(dd->dc, &dd->gpio);
error:
  return dd_errno;
}

dd_error_t dd_wvs75v2b_set_up_gpio_rst(dd_wvs75v2b_t dd,
                                       const char *gpio_chip_path, int pin_no) {

  if (!dd || !gpio_chip_path || pin_no < 0) {
    dd_errno = dd_errnos(EINVAL, "`dd` and `gpio_chip_path` cannot be NULL, "
                                 "`pin_no` has to be positive");
    goto error;
  }

  if (dd->rst) {
    dd_errno = dd_errnos(EINVAL, "Only one rst pin allowed");
    goto error;
  }

  dd_errno = dd_gpio_add_pin(gpio_chip_path, pin_no, &dd->rst, &dd->gpio);
  DD_TRY(dd_errno);

  dd_errno = dd_gpio_set_pin_output(dd->rst,
                                    false // Active-low
  );
  DD_TRY_CATCH(dd_errno, error_pin_cleanup);

  return 0;

error_pin_cleanup:
  dd_gpio_pin_destroy(dd->rst, &dd->gpio);
error:
  return dd_errno;
};

dd_error_t dd_wvs75v2b_set_up_gpio_bsy(dd_wvs75v2b_t dd,
                                       const char *gpio_chip_path, int pin_no) {

  if (!dd || !gpio_chip_path || pin_no < 0) {
    dd_errno = dd_errnos(EINVAL, "`dd` and `gpio_chip_path` cannot be NULL, "
                                 "`pin_no` has to be positive");
    goto error;
  }

  if (dd->bsy) {
    dd_errno = dd_errnos(EINVAL, "Only one bsy pin allowed");
    goto error;
  }

  dd_errno = dd_gpio_add_pin(gpio_chip_path, pin_no, &dd->bsy, &dd->gpio);
  DD_TRY(dd_errno);

  dd_errno = dd_gpio_set_pin_input(dd->bsy);
  DD_TRY_CATCH(dd_errno, error_pin_cleanup);

  return 0;

error_pin_cleanup:
  dd_gpio_pin_destroy(dd->bsy, &dd->gpio);
error:
  return dd_errno;
};

dd_error_t dd_wvs75v2b_set_up_gpio_pwr(dd_wvs75v2b_t dd,
                                       const char *gpio_chip_path, int pin_no) {

  if (!dd || !gpio_chip_path || pin_no < 0) {
    dd_errno = dd_errnos(EINVAL, "`dd` and `gpio_chip_path` cannot be NULL, "
                                 "`pin_no` has to be positive");
    goto error;
  }

  if (dd->pwr) {
    dd_errno = dd_errnos(EINVAL, "Only one pwr pin allowed");
    goto error;
  }

  dd_errno = dd_gpio_add_pin(gpio_chip_path, pin_no, &dd->pwr, &dd->gpio);
  DD_TRY(dd_errno);

  dd_errno = dd_gpio_set_pin_output(dd->pwr,
                                    true // Active-high
  );
  DD_TRY_CATCH(dd_errno, error_pin_cleanup);

  return 0;

error_pin_cleanup:
  dd_gpio_pin_destroy(dd->pwr, &dd->gpio);

error:
  return dd_errno;
}

dd_error_t dd_wvs75v2b_set_up_spi_master(dd_wvs75v2b_t dd,
                                         const char *spidev_path) {
  if (!dd || !spidev_path) {
    dd_errno = dd_errnos(EINVAL, "`dd` and `spidev_path` cannot be NULL");
    goto error;
  }

  if (dd->spi.path) {
    dd_errno = dd_errnos(EINVAL, "Only one spi allowed");
    goto error;
  }

  dd_errno = dd_spi_init(spidev_path, &dd->spi);
  DD_TRY(dd_errno);

  return 0;

error:
  return dd_errno;
}

dd_error_t dd_wvs75v2b_ops_reset(dd_wvs75v2b_t dd) {
  puts(__func__);
  if (!dd || !dd->pwr || !dd->rst) {
    dd_errno =
        dd_errnos(EINVAL, "`dd`, `dd->pwr` and `dd->rst` cannot be NULL");
    goto error;
  }

  if (dd_gpio_read_pin(dd->pwr, &dd->gpio) != 1) {
    dd_errno = dd_gpio_set_pin(1, dd->pwr, &dd->gpio);
    DD_TRY(dd_errno);
    dd_sleep_ms(200);
  }

  dd_errno = dd_gpio_set_pin(0, dd->rst, &dd->gpio);
  DD_TRY(dd_errno);
  dd_sleep_ms(200);

  dd_errno = dd_gpio_set_pin(1, dd->rst, &dd->gpio);
  DD_TRY(dd_errno);
  dd_sleep_ms(10);

  dd_errno = dd_gpio_set_pin(0, dd->rst, &dd->gpio);
  DD_TRY(dd_errno);
  dd_sleep_ms(200);
  dd_wvs75V2b_wait(dd); // Give chip time to reset itself

  return 0;

error:
  return dd_errno;
};

dd_error_t dd_wvs75v2b_ops_power_on(dd_wvs75v2b_t dd) {
  puts(__func__);
  if (!dd || !dd->dc || !dd->rst || !dd->bsy || !dd->pwr || !dd->spi.path) {
    dd_errno = dd_errnos(EINVAL, "`dd`, `dd->dc`, `dd->rst`, `dd->bsy`, "
                                 "`dd->pwr` and `dd->spi.path` cannot be NULL");
    goto error;
  }

  if (dd_gpio_read_pin(dd->pwr, &dd->gpio) != 1) {
    dd_errno = dd_gpio_set_pin(1, dd->pwr, &dd->gpio);
    DD_TRY(dd_errno);
    dd_sleep_ms(200);
  }


  
  dd_errno = dd_wvs75V2b_send_cmd(dd, dd_Wvs75V2bCmd_POWER_SETTING);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_errno = dd_wvs75V2b_send_data(dd,
                                   (uint8_t[]){
                                       0x07, // LDO disabled, VDHR
                                       0x07, // VGH=20V,VGL=-20V
                                       0x3F, // VDH=15V
                                       0x3F, // VDL=-15V
                                   },
                                   4);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  /* dd_wvs75V2b_wait(dd); */

  dd_errno = dd_wvs75V2b_send_cmd(dd, dd_Wvs75V2bCmd_BOOSTER_SOFT_START);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  // I'm not sure what this part does but it is in mainline driver
  dd_errno = dd_wvs75V2b_send_data(dd,
                                   (uint8_t[]){
                                       0x17,
                                       0x17,
                                       0x28,
                                       0x17,
                                   },
                                   4);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  /* dd_wvs75V2b_wait(dd); */

  dd_errno = dd_wvs75V2b_send_cmd(dd, dd_Wvs75V2bCmd_POWER_ON);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_sleep_ms(100);
  /* dd_wvs75V2b_wait(dd); */

  dd_errno = dd_wvs75V2b_send_cmd(dd, dd_Wvs75V2bCmd_PANEL_SETTING);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_errno = dd_wvs75V2b_send_data(
      dd,
      (uint8_t[]){
          0x0F, // Gate scan direction UP, Source Shift Direction Rigth, Booster
                // on, Do not perform soft reset, Red/White/black mode, White/black works very slow
      },
      1);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  /* dd_wvs75V2b_wait(dd); */

  dd_errno = dd_wvs75V2b_send_cmd(dd, dd_Wvs75V2bCmd_RESOLUTION_SETTING);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_errno = dd_wvs75V2b_send_data(dd,
                                   (uint8_t[]){
                                       0x03,
                                       0x20,
                                       0x01,
                                       0xE0,
                                   },
                                   4);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);

  dd_errno = dd_wvs75V2b_send_cmd(dd, dd_Wvs75V2bCmd_DUAL_SPI_MODE);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_errno = dd_wvs75V2b_send_data(dd, (uint8_t[]){0x00}, 1);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  /* dd_wvs75V2b_wait(dd); */

  dd_errno =
      dd_wvs75V2b_send_cmd(dd, dd_Wvs75V2bCmd_VCOM_AND_DATA_INTERVAL_SETTING);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_errno = dd_wvs75V2b_send_data(dd,
                                   (uint8_t[]){
                                       0x11,
                                       0x07,
                                   },
                                   2);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  /* dd_wvs75V2b_wait(dd); */

  dd_errno = dd_wvs75V2b_send_cmd(dd, dd_Wvs75V2bCmd_TCON_SETTING);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_errno = dd_wvs75V2b_send_data(dd, (uint8_t[]){0x22}, 1);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_wvs75V2b_wait(dd);

  return 0;

error_dd_cleanup:
  dd_wvs75v2b_ops_reset(dd);
error:
  return dd_errno;
}

dd_error_t dd_wvs75v2b_ops_clear(dd_wvs75v2b_t dd, bool white) {
  puts(__func__);
  if (!dd || !dd->dc || !dd->rst || !dd->bsy || !dd->pwr || !dd->spi.path) {
    dd_errno = dd_errnos(EINVAL, "`dd`, `dd->dc`, `dd->rst`, `dd->bsy`, "
                                 "`dd->pwr` and `dd->spi.path` cannot be NULL");
    goto error;
  }

  dd_errno = dd_wvs75V2b_send_cmd(dd, dd_Wvs75V2bCmd_START_TRANSMISSION1);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  for (int i = 0; i < DD_WVS75V2B_HEIGTH * (DD_WVS75V2B_WIDTH / 8); i++) {
    dd_errno = dd_wvs75V2b_send_data(dd,
                                     (uint8_t[]){
                                         white ? 0xFF : 0x00,
                                     },
                                     1);
    DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  }
  dd_wvs75V2b_wait(dd);

  dd_errno = dd_wvs75V2b_send_cmd(dd, dd_Wvs75V2bCmd_START_TRANSMISSION2);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  for (int i = 0; i < DD_WVS75V2B_HEIGTH * (DD_WVS75V2B_WIDTH / 8); i++) {
    dd_errno = dd_wvs75V2b_send_data(dd, (uint8_t[]){0x00}, 1);
    DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  }
  dd_wvs75V2b_wait(dd);

  dd_errno = dd_wvs75V2b_send_cmd(dd, dd_Wvs75V2bCmd_DISPLAY_REFRESH);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_sleep_ms(100);
  dd_wvs75V2b_wait(dd);

  return 0;

error_dd_cleanup:
  dd_wvs75v2b_ops_reset(dd);
error:
  return dd_errno;
}

dd_error_t dd_wvs75v2b_ops_power_off(dd_wvs75v2b_t dd) {
  puts(__func__);
  if (!dd || !dd->dc || !dd->rst || !dd->bsy || !dd->pwr || !dd->spi.path) {
    dd_errno = dd_errnos(EINVAL, "`dd`, `dd->dc`, `dd->rst`, `dd->bsy`, "
                                 "`dd->pwr` and `dd->spi.path` cannot be NULL");
    goto error;
  }
  
  dd_errno = dd_wvs75V2b_send_cmd(dd, dd_Wvs75V2bCmd_POWER_OFF);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_wvs75V2b_wait(dd);

  dd_errno = dd_wvs75V2b_send_cmd(dd, dd_Wvs75V2bCmd_DEEP_SLEEP);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_errno = dd_wvs75V2b_send_data(dd, (uint8_t[]){0xA5}, 1);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_sleep_ms(300); // In deep sleep busy does not work so we need to estimate
                    // time required display perform deep sleep ops

  return 0;

error_dd_cleanup:
  dd_wvs75v2b_ops_reset(dd);  
error:
  return dd_errno;
}

dd_error_t dd_wvs75v2b_ops_display_full(dd_wvs75v2b_t dd, dd_image_t image) {
  puts(__func__);
  if (!dd || !dd->dc || !dd->rst || !dd->bsy || !dd->pwr || !dd->spi.path ||
      !image) {
    dd_errno = dd_errnos(
        EINVAL, "`dd`, `dd->dc`, `dd->rst`, `dd->bsy`, "
                "`dd->pwr`, `dd->spi.path` and `image` cannot be NULL");
    goto error;
  }

  struct dd_ImagePoint *img_res = dd_image_get_resolution(image);
  unsigned char *img_data = dd_image_get_data(image);
  uint32_t img_data_len = dd_image_get_data_len(image);
  
  if (img_res->x != DD_WVS75V2B_WIDTH || img_res->y != DD_WVS75V2B_HEIGTH) {
    dd_errno = dd_errnos(EINVAL, "To display picture on full screen it's resolution has to "
                                 "match display's resolution");
    goto error;
  }

  // The display does full refresh in about 18 seconds so sending
  // byte by byte does not affect this time much. Real bottleneck is in screen.
  dd_errno = dd_wvs75V2b_send_cmd(dd, dd_Wvs75V2bCmd_START_TRANSMISSION1);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  for (int i = 0; i < img_data_len; i++) {
    dd_errno = dd_wvs75V2b_send_data(dd, (uint8_t[]){img_data[i]}, 1);
    DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  }
  dd_wvs75V2b_wait(dd);

  dd_errno = dd_wvs75V2b_send_cmd(dd, dd_Wvs75V2bCmd_START_TRANSMISSION2);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  for (int i = 0; i < img_data_len; i++) {
    dd_errno = dd_wvs75V2b_send_data(dd, (uint8_t[]){0x00}, 1);
    DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  }
  dd_wvs75V2b_wait(dd);
  
  dd_errno = dd_wvs75V2b_send_cmd(dd, dd_Wvs75V2bCmd_DISPLAY_REFRESH);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  dd_wvs75V2b_wait(dd);
  
  return 0;

error_dd_cleanup:
  dd_wvs75v2b_ops_reset(dd);
error:
  return dd_errno;
};
