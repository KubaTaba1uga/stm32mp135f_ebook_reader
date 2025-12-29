Getting started
===============

If you look for guides like how to upload new ebook onto a reader, that is not here. This part of documentation aim at people who are meant to develop something onto the device, new app new functionality, choose yourself. In further chapters i will try to be describing how the ebook reader was build so you can happily modify it's every part. First step in our journey will be obtaining firmware's source code:

.. code-block:: console

   $ git clone http://github.com/KubaTaba1uga/stm32mp135f_ebook_reader.git
   $ cd stm32mp135f_ebook_reader

To build all elements of the device (operating system, bootloader, libraries ...) we need to install required dependencies like compiler, script language interpreter etc. :

.. code-block:: console

   $ sudo apt install python3-invoke
   $ inv install

   
Build the SD card image
-----------------------

The device's firmware is on the SD card. When we box a brand new device we flash firmware onto the SD card and inject it to the device. The repository you just downloaded contain all source code required to build the sd card from scratch allowing the developer to repeat whole building process on it's own machine. Before making any changes however, we need to verify that you can rebuild the SD card image and boot the device successfully because if the device stops working from any reason we want you to be able to go back to default settings and confirm it is the firmware's fault. To build the BSP (Board Support Package) image perform following command:

.. code-block:: console

   $ inv build-bsp

.. note::
   
   This step will take a while so please be patient.
   

Deploy the image to an SD card
------------------------------

Once the image is built, insert the SD card into your workstation and identify its device path (for example: ``/dev/sda``).

Then deploy the image:

.. code-block:: console

   $ inv deploy-sdcard /dev/sda

.. warning::

   Double-check the device path before running the deploy command.
   Using the wrong device (e.g. your system disk) will overwrite data.

Boot the device
---------------

Now insert the SD card back into the eBook reader and power it on. The device should boot normally using the freshly built firmware.

Once you confirmed that device is working properly, you can go further to part where we modify the ebook's firmware.
