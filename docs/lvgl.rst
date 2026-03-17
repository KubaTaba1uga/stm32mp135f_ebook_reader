LVGL
===============

Patching
--------

We use meson to download appropriate lvgl version and patch it approprietly, to make lvgl usable by ebook reader.
Main customizations that we did are related to ``lv_conf.h`` file and fixing compilation warnings.

The procedure of creating the patch for lvgl is somwhat not straiforward so next we will walk threw it.
First you need to go into lvgl directory and copy files that you want to change, for example sake i changed lv_conf.h:

.. code-block:: console

   $ cp src/lv_conf.h /tmp/

Then we need to change copy of a file and once it's done we are ready to create a patch:

.. code-block:: console

   $ git diff src/lv_conf.h /tmp/lv_conf.h > mypatch.patch
   $ sed -i 's/b\/tmp/b\/src/g' mypatch.patch
   $ cp mypatch.patch ../packagefiles/lvgl/

Last thing is to add this new patch to lvgl.wrap to property diff_files.

