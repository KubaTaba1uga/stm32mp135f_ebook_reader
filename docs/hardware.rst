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



Thirst things to do now is configure pins approprietly to it's functions via DT.



Working with the display is mainly driven by choice of the alreay available HAL to quick things up.





The HAL can be used with various backends but we chose ``USE_DEV_LIB`` backend. 

