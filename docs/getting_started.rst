Getting Started
===============

Installation
------------

If you do not have ``invoke`` installed use:

.. code-block:: console

   $ apt-get install python3-invoke

Once ``invoke`` is installed download the repository and install dependencies:
   
.. code-block:: console

   $ git clone https://github.com/you/stm32mp135f_ebook_reader.git
   $ cd stm32mp135f_ebook_reader
   $ inv install

  
Dev Image Usage
---------------

Currently we support one build called ``ebook_reader_dev_defconfig``, which is meant for quick development cycle, to build it do:

.. code-block:: console

   $ inv build-bsp

   
Before you are ready to boot up the image, you need to configure your development machine accordingly:

.. toctree::
   :maxdepth: 1

   getting_started/set_up_tftp
   getting_started/set_up_nfs
   getting_started/set_up_sdcard   

The development image is meant to be booted over USB acting as a CDC gadget.
The development boot flow looks like this:

* TF-A loads U-Boot from the SD card.
* U-Boot loads Linux from the TFTP server using the USB CDC gadget.
* Linux loads its root filesystem from the NFS server using the USB CDC gadget.


Once the development machine is ready we can populate build artificts to proper directories:

.. code-block:: console
		
   $ inv deploy-to-sdcard		
   $ inv deploy-to-tftp		
   $ inv deploy-to-nfs
