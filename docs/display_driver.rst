Display driver
==============

We had issues sourcing and ordering multiple e-paper screens of the same model, so the project
needs to support multiple displays and variants without changing application code.

The vendor **Waveshare** reference driver was difficult to integrate into our
project. For this reason we implemented our own driver layer with a clean, consistent API.
This gives us better control over initialization and power sequencing, and allows us to tailor
timings and configuration specifically for the eBook reader use case.

The next section shows a typical usage example, followed by the full API reference.

Typical usage
-------------

.. code-block:: c
		
   #include "display_driver.h"
   #include <stdio.h>

   static void die(dd_error_t err) {
     char buf[256];
     dd_error_dumps(err, sizeof(buf), buf);
     fprintf(stderr, "display driver error: %s\n", buf);
   }

   int main(void) {
     dd_wvs75v2b_t dd = NULL;
     dd_error_t err = NULL;

     err = dd_wvs75v2b_init(&dd);
     if (err) {
       die(err);
       return 1;
     }

     err = dd_wvs75v2b_set_up_gpio_dc(dd, "/dev/gpiochip0", 25);
     if (err) {
       die(err);
       goto out;
     }
     
     err = dd_wvs75v2b_set_up_gpio_rst(dd, "/dev/gpiochip0", 24);
     if (err) {
       die(err);
       goto out;
     }

     err = dd_wvs75v2b_set_up_gpio_pwr(dd, "/dev/gpiochip0", 23);
     if (err) {
       die(err);
       goto out;
     }

     err = dd_wvs75v2b_set_up_gpio_bsy(dd, "/dev/gpiochip0", 22);
     if (err) {
       die(err);
       goto out;
     }

     err = dd_wvs75v2b_set_up_spi_master(dd, "/dev/spidev0.0");
     if (err) {
       die(err);
       goto out;
     }

     err = dd_wvs75v2b_ops_power_on(dd);
     if (err) {
       die(err);
       goto out;
     }

     err = dd_wvs75v2b_ops_reset(dd);
     if (err) {
       die(err);
       goto out;
     }

     err = dd_wvs75v2b_ops_clear(dd, true);
     if (err) {
       die(err);
       goto out;
     }

     err = dd_wvs75v2b_ops_power_off(dd);
     if (err) {
       die(err);
       goto out;
     }

   out:
     dd_wvs75v2b_destroy(&dd);
     return err ? 1 : 0;
   }


API
--------

.. doxygenfile:: display_driver.h
   :project: STM32MP135F eBook Reader
   :path: ../display_driver/include


Waveshare 7in5 V2b
------------------

The display used in this project is the Waveshare 7.5" V2b tri-color e-paper module
(black / white / red). It can be ordered directly from the official Waveshare store:
`Waveshare 7.5" e-Paper HAT (B) <https://www.waveshare.com/pico-epaper-7.5-b.htm>`_.

We selected the tri-color variant because the black/white version was not available at the time of development, and there were few realistic alternatives in the DIY market. Most commonly available e-paper modules trade features for simplicity: refresh times are long, refresh rates are low, and features such as touch input or backlight are typically not available.

Despite these limitations, the module provides a practical and well-documented baseline
for prototyping and for developing a reliable driver for our eBook reader.

.. list-table:: e-Paper HAT pinout
   :header-rows: 1

   * - Connector pin
     - GPIO bank
     - GPIO pin
     - Alternate func (T/F)
     - Alt func settings
     - GPIO Settings
     - Role
   * - 1
     - —
     - —
     - F
     - —
     - —
     - 3.3V
   * - 6
     - —
     - —
     - F
     - —
     - —
     - Ground
   * - 19
     - H
     - 3
     - T
     - SPI5_MOSI
     - Output, Active high
     - DIN (SPI data to device)
   * - 23
     - H
     - 7
     - T
     - SPI5_SCK
     - Output, Active high
     - CLK (SPI clock)
   * - 24
     - H
     - 11
     - T
     - SPI5_NSS
     - Output, Active low
     - CS (SPI chip select; active low)
   * - 22
     - I
     - 0
     - F
     - —
     - Output, Active high
     - DC (data/command select)
   * - 11
     - C
     - 2
     - F
     - —
     - Output, Active low
     - RST (reset)
   * - 18
     - G
     - 3
     - F
     - —
     - Input, Active Low
     - BUSY (device ready/busy)
   * - 12
     - A
     - 4
     - F
     - —
     - Output, Active high       
     - PWR (power enable)

.. note::
   Connector pin stands for the pin's number on 40 pin hardware header.       

.. note::
   When the pin `DC` is pulled HIGH, the data will be interpreted as data. When the pin is pulled Low, the data
   will be interpreted as command.

.. note::
   When the pin `BUSY` is pulled HIGH, the device is idle. When the pin is pulled Low, the device is busy
   processing a command and shouldn't be talked to.
