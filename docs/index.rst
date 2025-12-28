STM32MP135F eBook Reader Documentation
======================================

.. TODO: Add a picture of the device here (recommended: a front view + open enclosure)

What is this?
-------------

This is an open-source eBook reader based on the **STM32MP135F**.

I created this project to improve my Linux-based device development skills and to better understand the ARM architecture.

Why should I care?
------------------

If you are curious how an eBook reader works internally, this documentation provides a practical overview of how such a device **can** be designed and implemented.

I also run a company that provides end-to-end Linux device development and builds products like this for customers.
If you are interested in working with us, this documentation can help you understand our approach and decide whether we are a good fit for your project.

How do I start?
---------------

To follow along, you need the device.

If you do not have one but want to acquire it, contact me at: **TODO: add email address**.

The device consists of:

- an **STM32MP135F** single board computer,
- an **e-paper display**,
- a **battery**,
- a **sd card**,
- and a **3D-printed enclosure**.

Each part is described in the sections below, but start with **Getting Started** to set up your workstation and begin working with the device.

.. warning::
   
   This documentation was tested only on Debian Trixie

.. toctree::
   :maxdepth: 1
   :caption: Contents:

   getting_started
   buildroot   
   mpu
   display
   battery
   enclosure
   dummy_app

   
