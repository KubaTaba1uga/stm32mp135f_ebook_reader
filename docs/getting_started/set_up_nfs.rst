Set up NFS server
=================

This guide describes how to configure the host network interface and NFS server
used by the STM32MP135F development image when booting over USB (CDC Ethernet).


Configure USB Network Interface
-------------------------------

Set a static IP for the USB gadget interface by editing
``/etc/network/interfaces``:

.. code-block:: none

   # STM32MP135F-DK2 Board USB gadget
   allow-hotplug enxf8dc7a000001
   iface enxf8dc7a000001 inet static
       address 192.168.7.1/24
       gateway 192.168.7.1
       hwaddress ether f8:dc:7a:00:00:01


Configure the NFS Server
------------------------

Install the NFS server package:

.. code-block:: console

   $ sudo apt-get install nfs-kernel-server


Bind the NFS server to the USB network interface by editing
``/etc/default/nfs-kernel-server``:

.. code-block:: none

   RPCNFSDPRIORITY=0
   NEED_SVCGSSD=""
   RPCMOUNTDOPTS="--port 20048 --bind 192.168.7.1"


Export the NFS directory to the STM32MP135F board (client IP ``192.168.7.2``)
by editing ``/etc/exports``:

.. code-block:: none

   /srv/nfs/ 192.168.7.2(rw,sync,no_root_squash,no_subtree_check)

