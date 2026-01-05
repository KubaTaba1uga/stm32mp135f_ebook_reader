#include <math.h>
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
  **dd = (struct dd_Wvs75V2b){0};

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

  if ((*dd)->pwr) {
    dd_gpio_set_pin(0, (*dd)->pwr, &(*dd)->gpio);
  }
  if ((*dd)->rst) {
    dd_gpio_set_pin(0, (*dd)->rst, &(*dd)->gpio);
  }
  if ((*dd)->dc) {
    dd_gpio_set_pin(0, (*dd)->dc, &(*dd)->gpio);
  }

  dd_spi_destroy(&(*dd)->spi);
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
                // on, Do not perform soft reset, Red/White/black mode,
                // White/black works very slow
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
/* int get_bit(int i, unsigned char *buf, uint32_t buf_len) { */
/*   if (i < 0 || (uint32_t)i >= buf_len * 8) return -1; */
/*   int byte_number = i >> 3; */
/*   int bit_number  = 7 - (i % 7); */
/*   return (buf[byte_number] & (1u << bit_number)) != 0; */
/* } */


/* int get_bit(int i, unsigned char *buf, uint32_t buf_len) { */
/*   int byte_number = i / 8; */

/*   if (byte_number < 0 || byte_number >= buf_len) { */
/*     return -1; */
/*   } */

/*   // If MSB */
/*   int bit_number = 7 - (i % 8); */
/*   // If LSB */
/*   /\* int bit_number = ((i) % 8) - 1; *\/ */
/*   if (bit_number < 0) { */
/*     bit_number = 0; */
/*   } */

/*   return (buf[byte_number] & (1 << bit_number)) > 0; */
/* } */

/* void set_bit(int i, int val, unsigned char *buf, uint32_t buf_len) { */
/*   int byte_number = i / 8; */

/*   if (byte_number < 0 || byte_number >= buf_len) { */
/*     return; */
/*   } */

/*   // If MSB */
/*   int bit_number = 7 - (i & 7); */
/*   /\* int bit_number = 7 - (i % 8); *\/ */
/*   // If LSB */
/*   /\* int bit_number = ((i) % 8) - 1; *\/ */
/*   if (bit_number < 0) { */
/*     bit_number = 0; */
/*   } */

/*   if (val == 0) { */
/*     buf[byte_number] &= ~(1 << bit_number); */
/*   } else { */
/*     buf[byte_number] |= (1 << bit_number); */
/*   } */
/* } */

int get_bit(int i, unsigned char *buf, uint32_t buf_len) {
  if (i < 0 || (uint32_t)i >= buf_len * 8) return -1;
  int byte = i / 8;
  int bit  = 7 - (i % 8);
  return (buf[byte] >> bit) & 1;
}

void set_bit(int i, int val, unsigned char *buf, uint32_t buf_len) {
  if (i < 0 || (uint32_t)i >= buf_len * 8) return;
  int byte = i / 8;
  int bit  = 7 - (i % 8);
  if (val) buf[byte] |=  (1u << bit);
  else     buf[byte] &= ~(1u << bit);
}

int get_pixel(int x, int y, int width, unsigned char *buf, uint32_t buf_len) {
  if (x < 0 || y < 0) {
    return -1;
  }

  int bit = width * y + x;

  return get_bit(bit, buf, buf_len);
}
void set_pixel(int x, int y, int width, int val, unsigned char *buf,
               uint32_t buf_len) {
  if (x < 0 || y < 0) {
    return;
  }

  int bit = width * y + x;

  set_bit(bit, val, buf, buf_len);
}

/*  unsigned char * rotate(int width, int heigth,unsigned char *buf, uint32_t buf_len) { */
/*   const int src_w = 800, src_h = 480; */
/*   const int dst_w = 480; */

/*   unsigned char *out = dd_malloc(buf_len); */
/*   memset(out, 0, buf_len); */

/*   for (int y = 0; y < src_h; y++) { */
/*     for (int x = 0; x < src_w; x++) { */
/*       int v = get_pixel(x, y, src_w, buf, buf_len); */

/*       int dx = src_h - 1 - y; // 479..0 */
/*       int dy = x;             // 0..799 */

/*       set_pixel(dx, dy, dst_w, v, out, buf_len); */
/*     } */
/*   } */

/*   /\* memcpy(buf, out, buf_len); *\/ */
/*   /\* free(out); *\/ */
/*   return out; */
/* } */

unsigned char *rotate(int width, int heigth, unsigned char *buf,
                      uint32_t buf_len) {
  unsigned char *rotated = dd_malloc(buf_len);

  /* unsigned char *new_img = rotated; */
  memset(rotated, 0, buf_len);

  int first_pixel = width * (heigth - 1);
  int last_pixel = first_pixel + width;
  (void)first_pixel;
  (void)last_pixel;  
  printf("buf_len=%d\n", buf_len);
  int v;
  int dst_i = 0  ;
for (int x = width - 1; x >= 0; --x) {
  for (int y = 0; y < heigth; ++y) {
    v = get_pixel(x, y, width, buf, buf_len);
    set_bit(dst_i++, v, rotated, buf_len);
  }
}

  /* for (int src_i = first_pixel, dst_i = 0; src_i < last_pixel; src_i++) { */
  /*   for (int src_k = 0; src_k < heigth; src_k++, dst_i++) { */
  /*     int bit_number = src_i - (width * src_k); */
  /*     int bit_value = get_bit(bit_number, buf, buf_len); */
  /*     set_bit(dst_i, bit_value, rotated, buf_len); */
  /*   } */
  /* } */

  /* memcpy(buf, rotated, buf_len); */

  /* dd_free(rotated); */

  return rotated;
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

  /* struct dd_ImagePoint *img_res = dd_image_get_resolution(image); */
  unsigned char *img_data = dd_image_get_data(image);
  uint32_t img_data_len = dd_image_get_data_len(image);

  /* if (img_res->x != DD_WVS75V2B_WIDTH - 1 || */
  /*     img_res->y != DD_WVS75V2B_HEIGTH - 1) { */
  /*   dd_errno = */
  /*       dd_errnof(EINVAL, */
  /*                 "To display picture on full screen it's resolution has to " */
  /*                 "match display's resolution: x=%d, y=%d", */
  /*                 img_res->x, img_res->y); */
  /*   goto error; */
  /* } */

  // The display does full refresh in about 18 seconds so sending
  // byte by byte does not affect this time much. Real bottleneck is in screen.
  dd_errno = dd_wvs75V2b_send_cmd(dd, dd_Wvs75V2bCmd_START_TRANSMISSION1);
  DD_TRY_CATCH(dd_errno, error_dd_cleanup);

  unsigned char *rotated_img = rotate(800, 480, img_data, img_data_len);
  /* unsigned char *rotated_img =img_data; */
  for (int i = 0; i < img_data_len; i++) {    
    dd_errno = dd_wvs75V2b_send_data(dd, (uint8_t[]){rotated_img[i]}, 1);
    DD_TRY_CATCH(dd_errno, error_dd_cleanup);
  }
  dd_wvs75V2b_wait(dd);
  dd_free(rotated_img);

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
