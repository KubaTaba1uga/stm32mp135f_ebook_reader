Development
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

  

ebook_reader_dev_defconfig
--------------------------

Currently we support one dev build called ``ebook_reader_dev_defconfig``, which is meant for quick development cycle on official `STM32 dev board <https://www.st.com/en/evaluation-tools/stm32mp135f-dk.html/>`_.

Before development become rapid you need to build full BSP, this step will take some time but ideally has to be performed only once. This step is the longest in the whole development cycle so go grab a coffee or watch some TV cause this is gonna take a while:

.. code-block:: console

   $ inv build-bsp

   
Before you are ready to boot up the image, you need to configure your development machine accordingly:

.. toctree::
   :maxdepth: 1

   development/set_up_nic	      
   development/set_up_tftp
   development/set_up_nfs
   development/set_up_sdcard   

The development image is meant to be booted over USB acting as a CDC gadget. Boot flow for the image looks like this:

* TF-A loads U-Boot from the SD card.
* U-Boot loads Linux from the TFTP server using the USB CDC gadget.
* Linux loads its root filesystem from the NFS server using the USB CDC gadget.

Once the development machine is ready we can populate build artifacts to proper directories:

.. code-block:: console
		
   $ inv deploy-to-sdcard		
   $ inv deploy-to-tftp		
   $ inv deploy-to-nfs

At this point you are ready to boot a development board and start linux cli.


Buildroot interface
-------------------

We have bunch of commands in invoke wich are meant to serve as handy interface to buildroot:

* inv build-bsp
* inv build-linux
* inv build-uboot
* inv build-optee
* inv build-tfa

They are okay if you are forced to interact with buildroot, but for everyday work you may be interested in the alternative described further.


Fast interface
--------------

Problem with buildroot commands is they are very very slow, too slow for development. For example on my machine each command to only start take like 30-60 seconds so we need something without the overhead.

We have a bunch of invoke commands wich mean to replicate what buildroot is doing but faster. Each of these commands start with `fbuild-` (abbrevation from fast build) and is trying to do what buildroot is doing just faster for development needs.

Currently supported fast commands are:

* inv fbuild-linux-dt: Goal of these command is to read `BR2_LINUX_KERNEL_INTREE_DTS_NAME` and `BR2_LINUX_KERNEL_CUSTOM_DTS_DIR` from `ebook_reader_dev_defconfig` and recompile appropriate linux DT files and copy them to `build/buildroot/images`.

