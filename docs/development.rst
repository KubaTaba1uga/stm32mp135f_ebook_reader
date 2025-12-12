Development
===============

This site describes how to set up the BSP for development, for cases where you need to add new features, modify existing components, or iterate quickly during development.

Installation
------------

If you do not have ``invoke`` installed, use:

.. code-block:: console

   $ apt-get install python3-invoke

Once ``invoke`` is installed, download the repository and install dependencies:

.. code-block:: console

   $ git clone https://github.com/you/stm32mp135f_ebook_reader.git
   $ cd stm32mp135f_ebook_reader
   $ inv install


ebook_reader_dev_defconfig
--------------------------

Currently we support one dev build called ``ebook_reader_dev_defconfig``, which is meant for a quick development cycle on the official `STM32 dev board <https://www.st.com/en/evaluation-tools/stm32mp135f-dk.html/>`_.

Before development becomes rapid, you need to build the full BSP. This step will take some time, but ideally it has to be performed only once. This step is the longest in the whole development cycle, so go grab a coffee or watch some TV, because this is going to take a while:

.. code-block:: console

   $ inv build-bsp


Before you are ready to boot the image, you need to configure your development machine accordingly:

.. toctree::
   :maxdepth: 1

   development/set_up_nic
   development/set_up_tftp
   development/set_up_nfs
   development/set_up_sdcard

The development image is meant to be booted over USB acting as a CDC gadget. The boot flow for the image looks like this:

* TF-A loads U-Boot from the SD card.
* U-Boot loads Linux from the TFTP server using the USB CDC gadget.
* Linux loads its root filesystem from the NFS server using the USB CDC gadget.

Once the development machine is ready, we can populate build artifacts to the proper directories:

.. code-block:: console

   $ inv deploy-to-sdcard
   $ inv deploy-to-tftp
   $ inv deploy-to-nfs

At this point you are ready to boot a development board and start the Linux CLI.


Buildroot interface
-------------------

We have a bunch of commands in invoke which are meant to serve as a handy interface to Buildroot:

* ``inv build-bsp``: Configure Buildroot (by default ``ebook_reader_dev_defconfig``) and run a full BSP build.
* ``inv build-linux``: Configure Buildroot and build the ``linux`` package.
* ``inv build-uboot``: Configure Buildroot and build the ``uboot`` package.
* ``inv build-tfa``: Configure Buildroot and build the ``arm-trusted-firmware`` package.

They are okay if you are forced to interact with Buildroot, but for everyday work you may be interested in the alternative described further.

Fast interface
--------------

The problem with Buildroot commands is they are very, very slow, too slow for development. For example, on my machine each command takes around 30–60 seconds just to start, so we need something without the overhead.

We have a bunch of invoke commands which are meant to replicate what Buildroot is doing, but faster. Each of these commands starts with ``fbuild-`` (abbreviation for “fast build”) and tries to do what Buildroot does, just faster for development needs.

Currently supported fast commands are:

* ``inv fbuild-linux-dt``: The goal of this command is to read ``BR2_LINUX_KERNEL_INTREE_DTS_NAME`` and ``BR2_LINUX_KERNEL_CUSTOM_DTS_DIR`` from ``ebook_reader_dev_defconfig``, recompile the appropriate Linux DT files, and copy them to ``build/buildroot/images``.

