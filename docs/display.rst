Display
=======

The e-book reader uses an **e-ink** display. Unlike LCDs, e-ink does not require power to *maintain* an image: once a page is rendered, it stays visible (similar to paper). The main drawback is the **very low refresh rate** compared to LCD. Even partial refreshes can take several seconds, so we must be careful about what we update and when.

Our goal is to **minimize refresh operations** in response to a user button press. All state transitions in the e-book reader are triggered by button events, so the UI should be designed to avoid unnecessary screen changes.

Because the exact display model may vary between devices, we developed a display driver that supports all required displays. Build and deploy it with:

.. code-block:: console

		$ inv fbuild-display-driver
		$ inv deploy-nfs --no-rootfs

The display driver includes examples located in ``build/display_driver/``. These are a convenient way to test display functionality on the device without integrating changes into the main application.

If you need to work on the display driver itself, the source code is in the ``display_driver/src`` directory, while examples are in ``display_driver/examples/stm``.

Build system support also runing unit tests on the host. This is useful because host-based tests can enable sanitizers, which speeds up debugging (e.g., segfaults, memory leaks). To build the display driver unit tests:

.. code-block:: console

		$ inv fbuild-display-driver-tests

Now you can execute tests via invoke with:

.. code-block:: console

		$ inv test-display-driver
		
		
Waveshare 7.5inch V2b
---------------------

The first display we started with was the **Waveshare 7.5-inch tri-color V2** panel. At the moment we use only the **two-color mode**, which is sufficient for our needs.

This panel has drawbacks: refresh times are long, and it lacks grayscale support, so images are effectively limited to black and white. However, it is inexpensive, which matters for a hobby project. It is also often easier to source than the two-color equivalent, which helps when you need a replacement quickly.

The display comes with a prebuilt driver board, `e-paper driver hat Rev2.3`, which works well. Most example drivers we found only support **4-wire mode**. It would be useful to read data back from the display (e.g., temperature or firmware/version information), so it is worth considering adding **3-wire capability** to our driver.

Driver API
----------

.. doxygenfile:: display_driver.h
   :project: STM32MP135F eBook Reader
   :path: ../display_driver/include

