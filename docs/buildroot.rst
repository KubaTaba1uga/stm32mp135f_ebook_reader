Buildroot
=========

.. note::
   `Buildroot manual <https://buildroot.org/downloads/manual/manual.html>`_

We build the BSP using **Buildroot**. As a starting point for our configuration, we use ST’s external Buildroot tree:
`buildroot-external-st <https://github.com/bootlin/buildroot-external-st.git>`_.

This setup relies on the Buildroot concept of **in-tree** vs. **out-of-tree** builds.

In an **in-tree build**, you enter the Buildroot source directory and use configuration files, packages, and board definitions stored directly inside that tree.

In an **out-of-tree build**, configuration files and custom packages are stored outside the main Buildroot tree, in a separate directory.
This project uses the out-of-tree approach because it keeps project-specific changes cleanly separated from upstream Buildroot.

In this repository, Buildroot itself is located in ``third_party/buildroot``, and the external Buildroot tree is located in ``br2_external_tree``.

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

