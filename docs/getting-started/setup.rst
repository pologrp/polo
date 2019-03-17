Setting Up
==========

In this section, we will cover how to download, and install ``polo`` from
source, or use the prebuilt multi-architecture Docker_ images. The commands
given in this section are valid ``bash`` commands, and they should work on
Unix-like systems as well as Windows [#f1]_.

.. _Docker: https://www.docker.com/

Obtaining the Library
---------------------

``polo`` is hosted_ at GitHub under the permissive MIT license. There are two
different ways to obtain the library.

The first, and the suggested, way is to use git_ to clone the repository, and
check out a specific version (i.e., a snapshot). For instance, the following
code will clone the latest snapshot (i.e., the ``master`` branch) of the
library

.. code-block:: shell

  git clone https://github.com/pologrp/polo $HOME/polo

to ``polo`` directory under ``$HOME``, which serves as the default repository
for a user's personal files on Unix-like systems. Later, we can change
directory and check out a different snapshot by issuing the following command

.. code-block:: shell

  cd $HOME/polo
  git checkout v1.0.0

Here, we are checking out the ``v1.0.0`` snapshot.

The second way is to :download:`download <https://github.com/pologrp/polo/archive/master.zip>`
the ``master`` branch as a zip file (named, e.g., ``polo-master.zip`` under
``$HOME/Downloads`` directory), and unzip its contents under ``$HOME`` by
issuing the following command

.. code-block:: shell

  unzip $HOME/Downloads/polo-master.zip -d $HOME
  cd $HOME/polo-master # changes directory to the unzipped library

Similarly, we can visit the `releases <https://github.com/pologrp/polo/releases>`_
page of the library, download the specific snapshots, and follow the same
procedure to unzip their contents.

.. _hosted: https://github.com/pologrp/polo
.. _git: https://git-scm.org/

.. note::

  Currently, there are no tagged versions/snapshots in the library. Hence,
  throughout the documentation, we assume that the latest snapshot (i.e., the
  ``master`` branch) is checked out under ``$HOME/polo`` directory.

.. todo::

  After tagging the first version, reword the above note.

.. _install:

Installing the Library
----------------------

``polo`` is a C++ template library that uses C++11 features. As a result, we
need to have a compiler that supports these features. Among the well-known
compilers, ``gcc v4.8.1``, ``clang v3.3``, and ``Visual Studio 2015``, together
with their newer versions, all support C++11.

In addition to the C++ feature dependencies, ``polo`` requires CMake_ (at least
``v3.9.0``) to manage its *optional* dependencies:

#. Reference or optimized implementations of BLAS_ and LAPACK_ for matrix
   algebra,
#. Thread support library (usually included in the compilers) for
   :ref:`shared-memory`, and,
#. cURL_, 0MQ_ and cereal_ for :ref:`distributed-memory`.

.. todo::

  Explicitly mention the versions of the required packages.

Having installed an appropriate compiler chain and CMake, we can now start with
the installation procedure. First, because CMake only allows for
*out-of-source* builds, we need to create a new directory (e.g., ``build``)
under ``$HOME/polo`` so that CMake can create its artifacts:

.. code-block:: shell

  cd $HOME/polo
  mkdir build
  cd build

From this point, there are two options to install ``polo``. The first is to
check the system for the installed libraries, configure ``polo`` to enable
those features that rely on the optional dependencies if they exist on
the system, and finally install the library and configuration files under
``$HOME/local``:

.. code-block:: shell

  cmake -D CMAKE_INSTALL_PREFIX=$HOME/local ../src
  cmake --build .
  cmake --build . --target install

Here, we have used `CMAKE_INSTALL_PREFIX
<https://cmake.org/cmake/help/v3.9/variable/CMAKE_INSTALL_PREFIX.html>`_ to
install the library and configuration files under ``$HOME/local``. This is
usually needed on systems where we do not have direct write access to the
system directories. As a result, we need to tell CMake to also search this
directory for installed libraries when we compile programs that use ``polo``
(we will come to this later in :ref:`compilation`).

.. note::

  If ``-D CMAKE_INSTALL_PREFIX=$HOME/local`` is dropped, and the user has the
  proper write access, the library and configuration files will be installed
  under the system directories. In the documentation, we assume that the user
  does *not* have administrative/system rights.

The second option to install ``polo`` is to use the *superbuild* feature to
install all the optional dependencies and turn on all the features covered in
this documentation:

.. code-block:: shell

  cmake -D CMAKE_BUILD_TYPE=Release \
        -D CMAKE_INSTALL_PREFIX=$HOME/local ../
  cmake --build .
  cmake --build . --target install

Note that, this time, we use ``Release`` mode for `CMAKE_BUILD_TYPE
<https://cmake.org/cmake/help/v3.9/variable/CMAKE_BUILD_TYPE.html>`_ to install
the optimized binaries of the optional dependencies.

.. note::

  The superbuild feature requires a Fortran compiler to build BLAS and LAPACK
  from source.

.. todo::

  Later, think of using `Conan <https://conan.io/>`_ to package ``polo`` and
  its dependencies as prebuilt binaries.

.. _CMake: https://cmake.org/
.. _BLAS: https://www.netlib.org/blas/
.. _LAPACK: https://www.netlib.org/lapack/
.. _cURL: https://curl.haxx.se/
.. _0MQ: http://zeromq.org/
.. _cereal: https://github.com/USCiLab/cereal

Using the Prebuilt Docker Images
--------------------------------

On some systems or architectures, installing ``polo`` with all the optional
dependencies can be involved. To alleviate the problems in these situations, we
also provide multi-architecture Docker images that contain all the optional
dependencies. Using the Docker images is as simple as issuing the following:

.. code-block:: shell

  docker pull pologrp/polo
  docker run --tty --interactive pologrp/polo /bin/bash

Here, we are first `pulling
<https://docs.docker.com/engine/reference/commandline/pull/>`_ the latest
prebuilt Docker image of ``polo`` for our system, and then `running
<https://docs.docker.com/engine/reference/run/>`_ it in an isolated container
interactively with ``bash``.

.. note::

  The rest of the documentation can be followed easily after either installing
  ``polo`` from source by using the superbuild feature or using the prebuilt
  Docker images.

.. rubric:: Footnotes

.. [#f1] Windows users *might* need to install `Cygwin
  <https://www.cygwin.com/>`_ or `Windows Subsystem for Linux
  <https://docs.microsoft.com/en-us/windows/wsl/about>`_.
