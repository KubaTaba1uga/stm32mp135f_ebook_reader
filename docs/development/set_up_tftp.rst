Set up TFTP server
==================

This guide explains how to install and configure a **TFTP server** on the host,
prepare files for transfer, and fetch them from the STM32MP135F board via **U-Boot**.


Install the TFTP Server
-----------------------

Install ``tftpd-hpa`` (it is often already available on Ubuntu/Debian):

.. code-block:: console

   $ sudo apt install tftpd-hpa

To locate its configuration and service files:

.. code-block:: console

   $ sudo find /etc | grep tftp

The main configuration file is in ``/etc/default/tftpd-hpa`` directory:

.. code-block:: none

   TFTP_USERNAME="tftp"
   TFTP_DIRECTORY="/srv/tftp/"
   TFTP_ADDRESS=":69"
   TFTP_OPTIONS="--secure"

   
Create a Test File
------------------

Add a small file to verify the setup:

.. code-block:: console

   $ echo -e "Hello friend\n" | sudo tee /srv/tftp/textfile.txt

Ensure it is readable by TFTP clients:

.. code-block:: console

   $ sudo chmod 555 /srv/tftp/textfile.txt

Test by reading it:

.. code-block:: console

   $ sudo cat /srv/tftp/textfile.txt
   Hello friend


Transfer File via TFTP
----------------------

Boot board with u-boot next run TFTP to load the test file into RAM.
Avoid reserved memory addresses like ``0x81000000`` - use a safe one such as
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


