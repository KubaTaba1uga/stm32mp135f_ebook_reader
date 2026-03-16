LVGL
===============

Patching
--------

We use meson to download appropriate lvgl version and patch it approprietly, to make lvgl usable by ebook reader.
Main customizations that we did are related to ``lv_conf.h`` file and fixing compilation warnings.

The procedure of creating the patch for lvgl is somwhat not straiforward so next we will walk threw it.
First you need to 
