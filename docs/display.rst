Display
=======

The display we choose is https://www.waveshare.com/product/displays/e-paper/7.8inch-e-paper.htm. We tried chipper displays but main problem was resolution, on resolution like 800x480, displaying pdf properly proved to be impossible, scaling to so low resolution make pdfs unreadable.

.. list-table:: pinout
   :header-rows: 1

   * - Connector pin
     - GPIO bank
     - GPIO pin
     - Alternate func (T/F)
     - Alt func settings
     - GPIO Settings
     - Role
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
     - DIN (SPI data to hat)
   * - 21
     - A
     - 8
     - T
     - SPI5_MISO
     - Output, Active high
     - DOUT (SPI data from hat)       
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

.. note::
   Connector pin stands for the pin's number on 40 pin hardware header.       

.. note::
   When the pin `DC` is pulled HIGH, the data will be interpreted as data. When the pin is pulled Low, the data
   will be interpreted as command.

.. note::
   When the pin `BUSY` is pulled HIGH, the device is idle. When the pin is pulled Low, the device is busy
   processing a command and shouldn't be talked to.
   
