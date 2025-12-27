:orphan:

Configure USB Network Interface
===============================

Currently we support networking only via ``ifup/ifdown`` interface.

ifup/ifdown
-----------

Set a static IP for the USB gadget interface by adding interface to
``/etc/network/interfaces``:

.. code-block:: none

   # STM32MP135F-DK2 Board USB gadget
   allow-hotplug enxf8dc7a000001
   iface enxf8dc7a000001 inet static
       address 192.168.7.1/24
       gateway 192.168.7.1
       hwaddress ether f8:dc:7a:00:00:01

The value `enxf8dc7a000001` is an interface name, on debian it is derived from nic initial configuration:

* en -> Ethernet
* x -> external (non-PCI, usually USB)
* f8dc7a000001 -> derived from the device MAC address

It can differ on your machine. Check out ``dmesg`` to find nic's name on your machine.

Once interface is configured restart networking on your machine:

.. code-block:: bash

   sudo systemctl restart networking
