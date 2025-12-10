Getting Started
===============

Installation
------------

If you do not have `invoke` installed use:

.. code-block:: console

   $ apt-get install python3-invoke

Once `invoke` is installed download the repository and install dependencies:
   
.. code-block:: console

   $ git clone https://github.com/you/stm32mp135f_ebook_reader.git
   $ cd stm32mp135f_ebook_reader
   $ inv install

  
Usage
-----

Currently we support one build called `ebook_reader_dev_defconfig`, which is meant for quick development cycle.

First we need to download a sources required for our config:

.. code-block:: console

   $ inv configure
   $ inv download

Now all required packages are downloaded on the disk in `build/third_party` directory.

Once packages are downloaded we are ready for 
