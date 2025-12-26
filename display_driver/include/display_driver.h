#ifndef DISPLAY_DRIVER_H
#define DISPLAY_DRIVER_H
#include <stddef.h>
#include <stdbool.h>

/*

Goal of this code is to provide easy to use driver for few waveshare display
that we may use for the ebook reader.

The prefix for all the code is `dd` abbrevation from display driver.

Currently supported screens:
- waveshare 7.5 inch V2 tri color (red, black, white)

*/

/******************************************************************
 *                            Errors
 ******************************************************************
 */
struct dd_Error;
typedef struct dd_Error *dd_error_t;

int dd_error_get_code(dd_error_t err);
const char *dd_error_get_msg(dd_error_t err);
int dd_error_dumps(dd_error_t err, size_t buf_size, char *buf);

/******************************************************************
 *              Waveshare 7.5 inch V2b Display Driver
 ******************************************************************
 */
struct dd_Wvs75V2b;
typedef struct dd_Wvs75V2b *dd_wvs75v2b_t;

dd_error_t dd_wvs75v2b_init(dd_wvs75v2b_t *dd);
void dd_wvs75v2b_destroy(dd_wvs75v2b_t *dd);
dd_error_t dd_wvs75v2b_set_up_gpio_dc(dd_wvs75v2b_t dd, const char *gpio_chip_path,
                                      int pin_no);
dd_error_t dd_wvs75v2b_set_up_gpio_rst(dd_wvs75v2b_t dd, const char *gpio_chip_path,
                                       int pin_no);
dd_error_t dd_wvs75v2b_set_up_gpio_bsy(dd_wvs75v2b_t dd, const char *gpio_chip_path,
                                       int pin_no);
dd_error_t dd_wvs75v2b_set_up_gpio_pwr(dd_wvs75v2b_t dd,
                                       const char *gpio_chip_path, int pin_no);
dd_error_t dd_wvs75v2b_set_up_spi_master(dd_wvs75v2b_t dd, const char *spidev_path);
dd_error_t dd_wvs75v2b_ops_reset(dd_wvs75v2b_t dd);
dd_error_t dd_wvs75v2b_ops_power_on(dd_wvs75v2b_t dd);
dd_error_t dd_wvs75v2b_ops_power_off(dd_wvs75v2b_t dd);
dd_error_t dd_wvs75v2b_ops_clear(dd_wvs75v2b_t dd, bool white);


#endif // DISPLAY_DRIVER_H
