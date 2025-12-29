ebook_reader_dev_defconfig
==========================

.. TO-DO: add graph describing tf-a, optee, u-boot on sdcard and linux image via TFTP and rootfs (busybox init) via NFS.

The ``ebook_reader_dev_defconfig`` configuration is designed to speed up development by reducing the need to repeatedly reflash the SD card. The device still boots normally from the SD card (because the bootloader is stored there), but after the bootloader stage it downloads the Linux kernel over **TFTP** and loads it into RAM. Once Linux is running, it mounts an **NFS** export as its root filesystem.
   
This may sound complex, but the advantage is significant: most of the files used by the reader come directly from your workstation. That means changes are picked up immediately, without manually copying files to the device, which greatly speeds up development workflow.

Before using development configuration, set up the following on your workstation:

- :doc:`Set up NIC <set_up_nic>`
- :doc:`Set up TFTP <set_up_tftp>`
- :doc:`Set up NFS <set_up_nfs>`
- :doc:`Prepare SD card <set_up_sdcard>`

Once your workstation is properly set up, build the BSP using the development configuration:

.. code-block:: console

   $ inv build-bsp ebook_reader_dev_defconfig

Artifacts that we just build need to end up in few locations - SD card, TFTP server and NFS server:

.. code-block:: console

   $ inv deploy-to-sdcard
   $ inv deploy-to-tftp
   $ inv deploy-to-nfs

   
Buildroot interface
-------------------

On the day to day basis you do not need to build all parts of the BSP, so we have a bunch of commands in invoke which are meant to serve as a handy interface to Buildroot:

* ``inv build-linux``: Configure Buildroot and build the ``linux`` package.
* ``inv build-uboot``: Configure Buildroot and build the ``uboot`` package.
* ``inv build-optee``: Configure Buildroot and build the ``optee`` package.
* ``inv build-tfa``: Configure Buildroot and build the ``arm-trusted-firmware`` package.

Each command support passing a target, for example to clean linux package you can do:

.. code-block:: console

   $ inv build-linux -t clean

Which under the hood executes ``make clean-linux``. More info about available buildroot targets and their usage can be find here:

- ``source``: Fetch source (download tarball / clone repo).
- ``depends``: Build/install dependencies required for the package.
- ``extract``: Put source into build dir (extract/copy).
- ``patch``: Apply patches.
- ``configure``: Run configure commands (if any).
- ``build``: Compile the package.
- ``install-staging``: Install into staging dir (if needed).
- ``install-target``: Install into target/rootfs dir (if needed).
- ``install``: Run install-staging + install-target (host packages: install into host dir).
- ``dirclean``: Remove the whole package build directory.
- ``reinstall``: Re-run install commands only.
- ``rebuild``: Re-run compilation commands only.
- ``reconfigure``: Re-run configure, then rebuild.

Buildroot commands are okay if you need reproducibility, but for everyday work you may be interested in faster alternative.

Fast interface
--------------

The problem with Buildroot commands is they are very, very slow, too slow for development. For example, on my machine each command takes around 30-60 seconds just to start, so we implemented something without the additional overhead.

The idea is to replicate what Buildroot is doing manually in python (tasks.py), which so far proves to be much faster. Each command starts with ``fbuild-`` (abbreviation for “fast build”) and tries to do exactly what Buildroot does.

Currently supported fast commands are:

* ``inv fbuild-linux-dt``: The goal of this command is to read ``BR2_LINUX_KERNEL_INTREE_DTS_NAME`` and ``BR2_LINUX_KERNEL_CUSTOM_DTS_DIR`` from ``ebook_reader_dev_defconfig``, recompile the appropriate Linux DT files, and copy them to ``build/buildroot/images``.
* ``inv fbuild-linux-kernel``: Rebuild the Linux kernel image (``zImage``) using the existing
  Buildroot toolchain and kernel source tree, then copy the result to
  ``build/buildroot/images``. This is useful when you modify kernel code and want a fast
  rebuild without going through the full Buildroot package pipeline.

  
