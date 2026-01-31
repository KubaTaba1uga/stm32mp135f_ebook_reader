Getting started
===============

To start working with the repository, use git to download it onto your machine:

.. code-block:: console

   $ git clone https://github.com/KubaTaba1uga/stm32mp135f_ebook_reader
   $ cd stm32mp135f_ebook_reader

The directory you are in is divided into few parts, we have the applications
in the apps directory, there are programs and libraries that we developed to allow you read books on the device.

There is also br2_external_tree directory which is a buildroot external tree directory, we use buildroot to build sdcard and libraries which are used by our apps. So you will find there stuff like linux config, bootloader config, apps build configs, librariies confgs etc. 

To build sd card containing the app use invoke command ``build-bsp``:

.. code-block:: console

   $ sudo apt install -y python3-invoke
   $ inv build-bsp
   $ sudo dd bs=4M conv=fsync if=build/sdcard.img of=/dev/sda

Development build
-----------------

Flashing SD card every time we change something in the app would make development frustrating, so we have also a development build, which aim to speed up development efforts by using TFTP+NFS to boot linux and the app itself. You can build it once again using ``build-bsp`` command:

.. code-block:: console

   $ inv build-bsp -c ebook_reader_dev_defconfig

before you flash SD card and boot the device, remember to configure your machine appropriatly, you need TFTP and NFS server configured as well as configured network card to work with DWC USB gadget on the device:

- :doc:`Set up NIC <dev/set_up_nic>`
- :doc:`Set up TFTP <dev/set_up_tftp>`
- :doc:`Set up NFS <dev/set_up_nfs>`
- :doc:`Prepare SD card <dev/set_up_sdcard>`

Once all host configuration is done, you can connect USB C cable to your ebook reader and boot it up, if all went good you should see how uboot is downloading linux image from your machine in the logs:

.. code-block:: console

   using dwc2-udc, OUT ep2out-bulk IN ep1in-bulk STATUS ep3in-int
   MAC f8:dc:7a:00:00:02
   HOST MAC f8:dc:7a:00:00:01
   crq->brequest:0x0
   high speed config #1: 2 mA, Ethernet Gadget, using CDC Ethernet
   USB network up!
   Using usb@49000000 device
   TFTP from server 192.168.7.1; our IP address is 192.168.7.2
   Filename 'zImage'.
   Load address: 0xc0200000
   Loading: ##################################################  7.9 MiB
	    11 MiB/s
   done
   Bytes transferred = 8299976 (7ea5c8 hex)
   high speed config #1: 2 mA, Ethernet Gadget, using CDC Ethernet
   USB network up!
   Using usb@49000000 device
   TFTP from server 192.168.7.1; our IP address is 192.168.7.2
   Filename 'stm32mp135f-dk-ebook_reader.dtb'.
   Load address: 0xc2400000
   Loading: ##################################################  53.5 KiB
	    7.5 MiB/s
   done
   Bytes transferred = 54736 (d5d0 hex)
   Kernel image @ 0xc0200000 [ 0x000000 - 0x7ea5c8 ]
   ## Flattened Device Tree blob at c2400000
      Booting using the fdt blob at 0xc2400000
   Working FDT set to c2400000
      Loading Device Tree to cffef000, end cffff5cf ... OK
   Working FDT set to cffef000

   Starting kernel ...

