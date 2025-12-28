Getting started
===============

Obtain source code required to orchestrate the build:

.. code-block:: console

   $ git clone http://github.com/KubaTaba1uga/stm32mp135f_ebook_reader.git
   $ cd stm32mp135f_ebook_reader

To build all elements of the device (operating system, bootloader, libraries ...) we need to install required dependencies:

.. code-block:: console

   $ sudo apt install python3-invoke
   $ inv install

Build the SD card image
-----------------------

The firmware is on the SD card. Before making any changes, verify that you can rebuild the SD card image and boot the device successfully.

To build the BSP (Board Support Package) image do:

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

Insert the SD card back into the eBook reader and power it on.
The device should boot normally using the freshly built firmware.

Once you confirmed that device is working properly, we can talk about how to modify the ebook's firmware.

