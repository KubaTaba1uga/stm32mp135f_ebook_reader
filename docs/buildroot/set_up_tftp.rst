:orphan:

Set up TFTP server
==================

Install ``dnsmasq``:

.. code-block:: console

   $ sudo apt install dnsmasq

Create configuration file in ``/etc/dnsmasq.d/tftp.conf``:

.. code-block:: none
		
   # Disable dns
   port=0

   # Enable dnsmasq's built-in TFTP server
   enable-tftp

   # Set the root directory for files available via FTP.
   tftp-root=/srv/tftp

   # Do not abort if the tftp-root is unavailable
   tftp-no-fail

   # Bind to interfaces that appear later
   # Optional: restrict to your USB NIC only
   interface=enxf8dc7a000001

.. warning::

   Name of the interface assigned in ``interface=enxf8dc7a000001`` may be different on your machine.
   
Enable and start tftp service:

.. code-block:: console

   $ sudo systemctl enable dnsmasq
   $ sudo systemctl restart dnsmasq
		

Create a Test File
------------------

Add a small file to verify the setup:

.. code-block:: console

   $ echo Hello friend | sudo -u dnsmasq tee /srv/tftp/textfile.txt

Test by reading it:

.. code-block:: console

   $ cat /srv/tftp/textfile.txt
   Hello friend


Transfer File via TFTP
----------------------

Boot your board and enter u-boot cli, once we are in uboot we can transfer a test file via TFTP and load it into RAM.

.. warning::

   Avoid reserved memory addresses like ``0x81000000``, better use tested one such as
   ``0xC0000000``.

.. code-block:: console

   STM32MP> tftp 0xC0000000 textfile.txt

Expected output:

.. code-block:: none

   Using usb@49000000 device
   TFTP from server 192.168.7.1; our IP address is 192.168.7.2
   Filename 'textfile.txt'.
   Load address: 0xc0000000
   Loading: #
            2.9 KiB/s
   done
   Bytes transferred = 14 (e hex)

Dump memory to confirm the file was received correctly:

.. code-block:: console

   STM32MP> md 0xC0000000
   c0000000: 6c6c6548 7266206f 646e6569 e59f0a0a  Hello friend....


