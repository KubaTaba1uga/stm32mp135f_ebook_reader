:orphan:

Set up SD Card
==============

Initialize and partition the SD card.

.. warning::

   Replace ``/dev/sda`` with the correct device for your SD card.
   Running these commands on the wrong disk may cause data loss.

.. code-block:: console

   $ sudo sgdisk -o /dev/sda
   $ sudo sgdisk --resize-table=128      \
                 -a 1 -n 1:34:545        \
                 -c 1:fsbl1              \
                 -n 2:546:1057           \
                 -c 2:fsbl2              \
                 -n 3:1058:9249          \
                 -c 3:fip                \
                 -n 4:9250:              \
                 -c 4:rootfs             \
                 -A 4:set:2              \
                 -p /dev/sda

This creates the following partitions:

.. list-table::
   :header-rows: 1

   * - Partition
     - Name
     - Description
   * - 1
     - fsbl1
     - First-stage bootloader
   * - 2
     - fsbl2
     - Backup bootloader
   * - 3
     - fip
     - Firmware Image Package
   * - 4
     - rootfs
     - Root filesystem

The SD card is now ready to be populated with development image.
