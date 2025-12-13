Hardware
========

Screen
--------

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
     - DUMMY       
     - CLK (SPI clock)
   * - 24
     - H
     - 11
     - T
     - SPI5_NSS
     - DUMMY       
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
     - Input, Active High
     - BUSY (device ready/busy)
   * - 12
     - A
     - 4
     - F
     - —
     - DUMMY       
     - PWR (power enable)

.. note::
   Connector pin stands for the pin's number on 40 pin hardware header.       

.. note::
   It is natural to assume that a GPIO is "active" when its output signal is 1
   ("high"), and inactive when it is 0 ("low"). However in practice the signal of a
   GPIO may be inverted before is reaches its destination, or a device could decide
   to have different conventions about what "active" means. Such decisions should
   be transparent to device drivers, therefore it is possible to define a GPIO as
   being either active-high ("1" means "active", the default) or active-low ("0"
   means "active") so that drivers only need to worry about the logical signal and
   not about what happens at the line level.

.. note::
   Sometimes shared signals need to use "open drain" (where only the low signal
level is actually driven), or "open source" (where only the high signal level is
driven) signaling.

.. note::
   When the pin (DC) is pulled HIGH, the data will be interpreted as data. When the pin is pulled Low, the data
will be interpreted as command.

We can confirm that spi5 is detected by linux checking whether there is spi master registered:

.. code-block:: console

   # ls -l /sys/class/spi_master/
   total 0
   lrwxrwxrwx    1 root     root             0 Jan  1 00:35 spi0 -> ../../devices/platform/soc/5c007000.bus/4c003000.spi/spi_master/spi0

On our example `4c003000` points to spi5 register which confirms it is enabled.

Screen Driver
-------------

The waveshare driver require choosing library to talk with gpio. We choose `libgpiod <https://libgpiod.readthedocs.io/en/stable/index.html>`_, mainly because this is standard way of communicating now with userspace gpio interface, so it should just work. Version: 1.6.5.

## TO-DO improve above argumentation

To confirm that libgpiod is working you can try turning led on board (`PA14`) on and off:

.. code-block:: console

   # gpiodetect
   gpiochip0 [GPIOA] (16 lines)
   gpiochip1 [GPIOB] (16 lines)
   gpiochip2 [GPIOC] (16 lines)
   gpiochip3 [GPIOD] (16 lines)
   gpiochip4 [GPIOE] (16 lines)
   gpiochip5 [GPIOF] (16 lines)
   gpiochip6 [GPIOG] (16 lines)
   gpiochip7 [GPIOH] (15 lines)
   gpiochip8 [GPIOI] (8 lines)
   #gpioinfo 0
   gpiochip0 - 16 lines:
	   line   0:        "PA0"       unused   input  active-high
	   line   1:        "PA1"       unused   input  active-high
	   line   2:        "PA2"       unused   input  active-high
	   line   3:        "PA3"       unused   input  active-high
	   line   4:        "PA4"       unused   input  active-high
	   line   5:        "PA5"       unused   input  active-high
	   line   6:        "PA6"       kernel   input  active-high [used]
	   line   7:        "PA7"       unused   input  active-high
	   line   8:        "PA8"       unused   input  active-high
	   line   9:        "PA9"       unused   input  active-high
	   line  10:       "PA10"       unused   input  active-high
	   line  11:       "PA11"       unused   input  active-high
	   line  12:       "PA12"       unused   input  active-high
	   line  13:       "PA13"       unused   input  active-high
	   line  14:       "PA14"       unused   input  active-high
	   line  15:       "PA15"       unused   input  active-high
   # gpioget 0 14
   1
   # gpioset 0 14=0
   # gpioset 0 14=1


SPI is used from userspace thanks to spidev library, more info about api `here <https://www.kernel.org/doc/html/latest/spi/spidev.html>`_.

