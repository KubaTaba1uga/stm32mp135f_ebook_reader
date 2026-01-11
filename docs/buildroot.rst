Buildroot
=========

.. note::
   `Buildroot manual <https://buildroot.org/downloads/manual/manual.html>`_

Our BSP is build using **Buildroot**, so to create any durable changes to the device you propably end up in integrating them into the **buildrooot** configuration. We are using special buildroot configuration developed by Bootlin `buildroot-external-st <https://github.com/bootlin/buildroot-external-st.git>`_, it is meant to serve as starting point to bootstrap development of the devices just like ours. Before going further let's understand one of buildroots concepts **in-tree** vs. **out-of-tree** builds because this is required to understand how buildroot-external-st is used in building our BSP.

In an **in-tree build**, you enter the Buildroot source directory and use configuration files, packages, and board definitions stored directly inside one of directories nested in buildroot root directory.

In an **out-of-tree build**, configuration files and custom packages are stored outside the main Buildroot tree, in a separate directory. This project uses the out-of-tree approach.

In our project Buildroot is located in ``third_party/buildroot`` directory and the external Buildroot tree is located in ``br2_external_tree`` directory.
When you execute ``inv build-bsp``, the build system enters ``third_party/buildroot`` and configures Buildroot using a configuration from
``br2_external_tree/configs`` (for details see ``tasks.py`` and the ``build_bsp`` function). If you inspect ``br2_external_tree/configs`` directory you will see various confiurations developed by bootlin and few ours configs wich you can differentiate by their prefix ``ebook_reader``.

Modifying the current config should be started by copying it and editing the copy. To select edited buildroot configuration you can use our good old command ``build-bsp``:

.. code-block:: console

   $ inv build-bsp <config-name>

Currently supported configurations are:

- ``ebook_reader_prod_defconfig``:
  Production build intended for the device. It assumes you boot from an SD card. This is the recommended option when getting started.

- ``ebook_reader_dev_defconfig``:
  Development build intended for rapid iteration. It assumes you use USB-C OTG to deploy changes quickly to a running device.
  Use this configuration while actively developing new features.

.. toctree::
   :maxdepth: 1
   :caption: Configurations details:

   buildroot/ebook_reader_prod_defconfig
   buildroot/ebook_reader_dev_defconfig

App dependencies
----------------

We are building userspace apps dependencies via meson, the idea here is to centralize all app build into the app directory itself.
For example sake let's consider ``display_driver`` library which implement drivers required to handle our screens, ``display_driver``
uses libgpiod which it compiles into it's library file. We use static compilation to ease updates and modification of libraries.
Because ``display_driver`` manages build of it's dependencies it can insert sanitizers to the libgpiod with need to use of buildroot.

The rule her is that each app or lib that we implement for need of ebook_reader should be self contained and build all it's dependencies.



