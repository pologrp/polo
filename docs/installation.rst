Installation
============

This chapter documents how to install ``polo``.

Building From Source
--------------------

Because POLO is header-only, it is enough to copy the contents of the
``include`` directory to one of the search paths of your compiler. If you have
cmake_ installed on your system, you can issue the following commands on your
terminal:

.. _cmake: https://cmake.org/

.. code-block:: shell

  git clone https://github.com/aytekinar/polo
  cd polo
  mkdir build
  cd build
  cmake -D CMAKE_INSTALL_PREFIX=$HOME/local ../
  cmake --build . --target install

This will install the library into ``$HOME/local/include``, where ``$HOME``
usually points to ``/home/<username>`` on Linux systems. Once the installation
is complete, you can either configure your compiler to search for the header
files under ``$HOME/local/include`` or use the switch ``-I $HOME/local/include``
whenever you compile your project that uses POLO.

Using Docker Images
-------------------

Some text on Docker images.

What Next
---------

Having installed the library successfully, you continue reading to understand
how things work :ref:`under the hood <explanation start>`, or you can jump to
the :ref:`examples <example start>` section to have a walkthrough.
