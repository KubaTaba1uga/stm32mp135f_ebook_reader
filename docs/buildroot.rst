Buildroot
=========

.. note::
   `Buildroot manual <https://buildroot.org/downloads/manual/manual.html>`_

Our BSP is build using **Buildroot**, so to create any durable changes to the device you propably end up in integrating them into the **buildrooot** configuration. Let's understand how the BSP is build in details so you can modify it without a hassle.
   
Bootlin developed BSP starting point called `buildroot-external-st <https://github.com/bootlin/buildroot-external-st.git>`_. To bootstrap development of the device we are using it. Before going further lat's understand one of buildroots concepts **in-tree** vs. **out-of-tree** builds.

In an **in-tree build**, you enter the Buildroot source directory and use configuration files, packages, and board definitions stored directly inside one of directories nested in buildroot root directory.

In an **out-of-tree build**, configuration files and custom packages are stored outside the main Buildroot tree, in a separate directory.
This project uses the out-of-tree approach because it keeps project-specific changes cleanly separated from upstream Buildroot.

In our project Buildroot is located in ``third_party/buildroot`` directory and the external Buildroot tree is located in ``br2_external_tree`` directory.

When you execute ``inv build-bsp``, the build system enters ``third_party/buildroot`` and configures Buildroot using a configuration from
``br2_external_tree/configs`` (for details see ``tasks.py`` and the ``build_bsp`` function).

You can select a specific configuration by running:

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

