Getting started
===============

Before we can build all elements of the device (operating system, bootloader ...) we need to install all required dependencies:

.. code-block:: console

   $ sudo apt install python3-invoke
   $ inv install

Build the SD card image
-----------------------

The operating system, bootloader, and application are stored on the SD card. Before making any changes, verify that you can rebuild the SD card image and boot the device successfully.

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
