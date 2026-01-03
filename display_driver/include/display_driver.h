/**
 * \file display_driver.h
 * \brief Public API for display drivers used by the eBook reader.
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
 * \note
 * Typical usage sequence: init() -> set_up_*() -> ops_*() -> destroy().
 * Always call destroy() to release resources.
 *
 * \warning
 * The driver is not responsible for leaving the device in proper state so
 * ensure that when your app receive signal or exits you destroy the driver so
 * it can restore pins to proper states.
 *
 */
#ifndef DISPLAY_DRIVER_H
#define DISPLAY_DRIVER_H
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/******************************************************************
 *                            Errors
 ******************************************************************
 */
/**
 * \brief Opaque error object returned by driver functions.
 *
 * Error objects are returned on failure and can be queried for details.
 */
struct dd_Error;

/** \brief Error type. NULL means "no error". */
typedef struct dd_Error *dd_error_t;

/**
 * \brief Get error code from an error object.
 * \param err Error instance.
 * \return Integer error code.
 */
int dd_error_get_code(dd_error_t err);

/**
 * \brief Get human-readable error message.
 * \param err Error instance.
 * \return NUL-terminated message string.
 */
const char *dd_error_get_msg(dd_error_t err);

/**
 * \brief Dump error information into a user-provided buffer.
 * \param err Error instance.
 * \param buf_size Size of \p buf in bytes.
 * \param buf Output buffer.
 * \return 0 on success, not 0 on error.
 */
int dd_error_dumps(dd_error_t err, size_t buf_size, char *buf);

/******************************************************************
 *                            Image
 ******************************************************************
 */
/**
 * \brief An image representation. 
 *
 * Image can be used to draw picture of any resolution.
 */
struct dd_Image;

/**
 * \brief Image type. 
 */
typedef struct dd_Image *dd_image_t;

/**
 * \brief An point on the image representation. 
 *
 * Image point represent anything that can be desribed by offset on X and Y axis.
 */
struct dd_ImagePoint {
  uint32_t x;
  uint32_t y;
};

/**
 * \brief Initialize image object.
 * \param img Pointer to image wich will be filled with by the function.
 * \param data Data encoded to display driver format.
 * \param resolution Resolution of the image encoded into data.
 * \return Error on failure, NULL on success.
 */
dd_error_t dd_image_init(dd_image_t *img, unsigned char *data, uint32_t data_len,
                  struct dd_ImagePoint resolution);

/**
 * \brief Destroy image object.
 * \param img Pointer to image wich will be destroyed by the function.
 */
void dd_image_destroy(dd_image_t *img);

/******************************************************************
 *              Waveshare 7.5 inch V2b Display Driver
 ******************************************************************
 */
/**
 * \brief Opaque driver instance for Waveshare 7.5" V2b tri-color display.
 */
struct dd_Wvs75V2b;

/** \brief Driver type. */
typedef struct dd_Wvs75V2b *dd_wvs75v2b_t;

/**
 * \brief Create and initialize the display driver instance.
 * \param dd Output pointer to receive the driver.
 * \return Error on failure, NULL on success.
 */
dd_error_t dd_wvs75v2b_init(dd_wvs75v2b_t *dd);

/**
 * \brief Destroy the driver instance and release resources.
 *        This function resets display to default state.
 * \param dd Pointer to the driver. Set to NULL on return.
 */
void dd_wvs75v2b_destroy(dd_wvs75v2b_t *dd);

/**
 * \brief Configure GPIO used as D/C (data/command) pin.
 * \param dd Driver instance.
 * \param gpio_chip_path Path to GPIO chip device (e.g. "/dev/gpiochip0").
 * \param pin_no GPIO line number.
 * \return Error on failure, NULL on success.
 */
dd_error_t dd_wvs75v2b_set_up_gpio_dc(dd_wvs75v2b_t dd,
                                     const char *gpio_chip_path, int pin_no);

/**
 * \brief Configure GPIO used as RESET pin.
 * \param dd Driver instance.
 * \param gpio_chip_path Path to GPIO chip device.
 * \param pin_no GPIO line number.
 * \return Error on failure, NULL on success.
 */
dd_error_t dd_wvs75v2b_set_up_gpio_rst(dd_wvs75v2b_t dd,
                                      const char *gpio_chip_path, int pin_no);

/**
 * \brief Configure GPIO used as BUSY pin.
 * \param dd Driver instance.
 * \param gpio_chip_path Path to GPIO chip device.
 * \param pin_no GPIO line number.
 * \return Error on failure, NULL on success.
 */
dd_error_t dd_wvs75v2b_set_up_gpio_bsy(dd_wvs75v2b_t dd,
                                      const char *gpio_chip_path, int pin_no);

/**
 * \brief Configure GPIO used as display power control pin.
 * \param dd Driver instance.
 * \param gpio_chip_path Path to GPIO chip device.
 * \param pin_no GPIO line number.
 * \return Error on failure, NULL on success.
 */
dd_error_t dd_wvs75v2b_set_up_gpio_pwr(dd_wvs75v2b_t dd,
                                      const char *gpio_chip_path, int pin_no);

/**
 * \brief Configure SPI master interface used to communicate with the display.
 * \param dd Driver instance.
 * \param spidev_path Path to spidev device (e.g. "/dev/spidev0.0").
 * \return Error on failure, NULL on success.
 */
dd_error_t dd_wvs75v2b_set_up_spi_master(dd_wvs75v2b_t dd,
                                        const char *spidev_path);

/**
 * \brief Reset the display using the configured reset line.
 * \param dd Driver instance.
 * \return Error on failure, NULL on success.
 */
dd_error_t dd_wvs75v2b_ops_reset(dd_wvs75v2b_t dd);

/**
 * \brief Power on the display.
 * \param dd Driver instance.
 * \return Error on failure, NULL on success.
 */
dd_error_t dd_wvs75v2b_ops_power_on(dd_wvs75v2b_t dd);

/**
 * \brief Power off the display.
 * \param dd Driver instance.
 * \return Error on failure, NULL on success.
 */
dd_error_t dd_wvs75v2b_ops_power_off(dd_wvs75v2b_t dd);

/**
 * \brief Clear the display to white or black.
 * \param dd Driver instance.
 * \param white If true clear to white, otherwise clear to black.
 * \return Error on failure, NULL on success.
 */
dd_error_t dd_wvs75v2b_ops_clear(dd_wvs75v2b_t dd, bool white);

/**
 * \brief Display picture on whole screen.
 * \param dd Driver instance.
 * \param image Image to displayed.
 * \return Error on failure, NULL on success.
 */
dd_error_t dd_wvs75v2b_ops_display_full(dd_wvs75v2b_t dd, dd_image_t image);


#endif // DISPLAY_DRIVER_H
