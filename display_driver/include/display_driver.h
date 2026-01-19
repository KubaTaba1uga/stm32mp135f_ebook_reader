/**
 * @file display_driver.h
 * @brief Public API for display drivers used by the eBook reader.
 *
 * This header provides an abstraction layer for controlling supported displays.
 * Each display driver follows a similar structure:
 * - **init / destroy**: allocate and release driver instance resources
 * - **set_up_**: configure required peripherals (GPIO, SPI, etc.)
 * - **ops_**: perform display operations (reset, power, clear, refresh, ...)
 *
 * Currently supported displays:
 * - Waveshare 7.5" V2b tri-color e-paper (black / white / red)
 *
 * The common prefix for all public symbols is `dd` (display driver).
 *
 * @note
 * Typical usage sequence: init() -> set_up_*() -> ops_*() -> destroy().
 * Always call destroy() to release resources.
 *
 * @warning
 * The driver is not responsible for leaving the device in proper state so
 * ensure that when your app receive signal or exits you destroy the driver so
 * it can restore pins to proper states.
 *
 */
#ifndef DISPLAY_DRIVER_H
#define DISPLAY_DRIVER_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/******************************************************************
 *                            Errors
 ******************************************************************
 */
/**
 * @brief Opaque error object returned by driver functions.
 *
 * Error objects are returned on failure and can be queried for details.
 */
struct dd_Error;

/** @brief Error type. NULL means "no error". */
typedef struct dd_Error *dd_error_t;

/**
 * @brief Get error code from an error object.
 * @param err Error instance.
 * @return Integer error code.
 */
int dd_error_get_code(dd_error_t err);

/**
 * @brief Get human-readable error message.
 * @param err Error instance.
 * @return NUL-terminated message string.
 */
const char *dd_error_get_msg(dd_error_t err);

/**
 * @brief Dump error information into a user-provided buffer.
 * @param err Error instance.
 * @param buf_size Size of buf in bytes.
 * @param buf Output buffer.
 * @return 0 on success, not 0 on error.
 */
int dd_error_dumps(dd_error_t err, size_t buf_size, char *buf);

/******************************************************************
 *              Waveshare 7.5 inch V2b Display Driver
 ******************************************************************
 */
/**
 * @brief Config for display driver instance.
          You have to supply this config to dd_display_driver_init `config`
          attribute if you want to use this display. config can be initialized
          like this: &(struct dd_Wvs75V2bConfig){.dc = {.gpio_chip_path="", }}
          // TO-DO finish up example

          On Wvs75V2bConfig power on sequence look sth like this:
             1. set up all gpio pins.
             2. set up spi.
             3. send reset sequence to display controller.

          If you want to display 480x800 images instead of 800x480 set rotate to
          true.
 */
struct dd_Wvs75V2bConfig {
  struct {
    const char *gpio_chip_path;
    int pin_no;
  } dc;
  struct {
    const char *gpio_chip_path;
    int pin_no;
  } rst;
  struct {
    const char *gpio_chip_path;
    int pin_no;
  } bsy;
  struct {
    const char *gpio_chip_path;
    int pin_no;
  } pwr;
  struct {
    const char *spidev_path;
  } spi;

  bool rotate; // This display is 800x480, if you need to display 480x800 image
               // set this to true.
};

/******************************************************************
 *              Waveshare 7.5 inch V2 Display Driver
 ******************************************************************
 */
struct dd_Wvs75V2Config {
  struct {
    const char *gpio_chip_path;
    int pin_no;
  } dc;
  struct {
    const char *gpio_chip_path;
    int pin_no;
  } rst;
  struct {
    const char *gpio_chip_path;
    int pin_no;
  } bsy;
  struct {
    const char *gpio_chip_path;
    int pin_no;
  } pwr;
  struct {
    const char *spidev_path;
  } spi;

  bool rotate; // same as in v2b
  
};


/******************************************************************
 *                     Generic Display Driver
 ******************************************************************
 */
enum dd_DisplayDriverEnum {
  dd_DisplayDriverEnum_Wvs7in5V2b,
  dd_DisplayDriverEnum_Wvs7in5V2,  
};
typedef struct dd_DisplayDriver *dd_display_driver_t;

dd_error_t dd_display_driver_init(dd_display_driver_t *out,
                                  enum dd_DisplayDriverEnum model,
                                  void *config);
/**
 * @brief Clear the display to white or black.
 * @param dd Driver instance.
 * @param white If true clear to white, otherwise clear to black.
 * @return Error on failure, NULL on success.
 */
dd_error_t dd_display_driver_clear(dd_display_driver_t dd, bool white);

/**
 * @brief Display picture on whole screen.
 * @param dd Driver instance.
 * @param image Image to displayed.
 * @return Error on failure, NULL on success.
 */
dd_error_t dd_display_driver_write(dd_display_driver_t dd, unsigned char *buf,
                                   uint32_t buf_len);

void dd_display_driver_destroy(dd_display_driver_t *out);

#endif // DISPLAY_DRIVER_H
