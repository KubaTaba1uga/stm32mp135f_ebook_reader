#ifndef DISPLAY_DRIVER_H
#define DISPLAY_DRIVER_H

/*

Goal of this code is to provide easy to use driver for few waveshare display
that we may use for the ebook reader.

The prefix for all the code is `dd` abbrevation from display driver.

*/

/******************************************************************
 *                            Errors                              *
 ******************************************************************
 */
struct dd_Error;
typedef struct dd_Error *dd_error_t;
int dd_error_get_code(dd_error_t err);
const char *dd_error_get_msg(dd_error_t err);

/******************************************************************
 *                       Display Driver                           *
 ******************************************************************
 */
struct dd_DisplayDriver;
typedef struct dd_DisplayDriver *dd_display_driver_t;

dd_error_t dd_global_init(void);

enum dd_SupportedDisplayEnum {
  dd_SupportedDisplayEnum_Waveshare_7_5_inch_V2_b,
};
dd_error_t dd_display_driver_init(dd_display_driver_t *dd,
                                  enum dd_SupportedDisplayEnum display);

enum dd_GpioPinEnum {
  dd_GpioPinEnum_DC,
  dd_GpioPinEnum_RST,
  dd_GpioPinEnum_BSY,
  dd_GpioPinEnum_PWR,
};
dd_error_t dd_display_driver_add_gpio_pin(dd_display_driver_t dd,
                                          enum dd_GpioPinEnum pin_cls,
                                          const char *gpio_chip, int pin_no);

dd_error_t dd_display_driver_add_spi_master(dd_display_driver_t dd,
                                           const char *spidev_path);

dd_error_t dd_display_driver_reset(dd_display_driver_t dd);

void dd_display_driver_destroy(dd_display_driver_t *dd);

dd_error_t dd_display_driver_read_temp(dd_display_driver_t dd, int *temp);

#endif // DISPLAY_DRIVER_H
