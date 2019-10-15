.. _compilation:

Compiling, Linking and Executing
================================

After writing C++ files, we need to compile the source code, and (optionally)
link against different libraries to obtain the final executable. Traditionally,
this compilation and linking process is merged into a single command line, in
which the users provide different compile-time definitions (to configure parts
of the dependencies), include directories (to search for library files), and
the libraries to link against. However, this gets tedious when projects get
bigger.

As we have discussed in :ref:`install`, ``polo`` relies on CMake to manage its
dependencies in a cross-platform compatible way. From the user's perspective,
this also has the added advantage of managing the compilation and linking
process much more easily.

To build code that requires ``polo``, we simply create a *CMake project*. A
CMake project is a directory that has a ``CMakeLists.txt`` file, which lists
the executable files and their dependencies in the directory. Most modern
integrated development environments (IDEs) natively support CMake projects and
help users create the ``CMakeLists.txt`` file interactively [#f1]_. However, we
briefly mention here how to create and use CMake projects, manually, in the
most basic form.

Let's assume that we have a directory, ``$HOME/examples``, which contains the
sample code in :numref:`code-anatomy` in a file, ``anatomy.cpp``. In the same
directory, we create a ``CMakeLists.txt`` file that has the following lines

.. code-block:: cmake

  cmake_minimum_required(VERSION 3.9.0)
  project(polo-examples)

  find_package(polo CONFIG REQUIRED)

  add_executable(anatomy anatomy.cpp)
  target_link_libraries(anatomy PRIVATE polo::polo)

Here, we first choose a minimum version of CMake that is required for our
project and name our project ``polo-examples``. Then, we tell CMake to find
the configuration files for ``polo``, which is required for our project. If
CMake cannot find the configuration files, the build process will stop.
Otherwise, CMake will source the configuration files and create a target
library called ``polo::polo``, which knows how to handle its dependencies.
Finally, for each executable we might have in the directory, we create a target
executable (``add_executable``) and link it against ``polo::polo``
(``target_link_libraries``). In the example, we have only one target
executable, which is named ``anatomy``, and it consists of only one source
file, ``anatomy.cpp``. Note that we do not need to define any compile-time
definitions, include directories, or any of the optional dependencies of
``polo`` as they are already handled transitively by CMake behind the scenes.

The next step is to create a new directory (e.g., ``build``) inside
``$HOME/examples`` for CMake to put its build artifacts. Eventually, we will
end up with the following directory structure

.. code-block:: shell

  cd $HOME/examples # change directory to $HOME/examples
  mkdir build # make a directory named build
  tree . # check the directory structure
  .
  ├── anatomy.cpp
  ├── build
  └── CMakeLists.txt

Finally, we change directory to ``build``, ask CMake to build the project for
us, and run the executable

.. code-block:: shell

  cd build
  cmake -D CMAKE_BUILD_TYPE=Release \
        -D CMAKE_PREFIX_PATH=$HOME/local ../
  cmake --build .
  ./anatomy # could be ./anatomy.exe on Windows systems

Here, we again use ``Release`` mode for `CMAKE_BUILD_TYPE
<https://cmake.org/cmake/help/v3.9/variable/CMAKE_BUILD_TYPE.html>`_, which
results in optimized binaries, and point `CMAKE_PREFIX_PATH
<https://cmake.org/cmake/help/v3.9/variable/CMAKE_PREFIX_PATH.html>`_ to
``$HOME/local``, where we have installed ``polo``, so that CMake can find and
source the configuration files.

.. note::

  If ``-D CMAKE_INSTALL_PREFIX=$HOME/local`` is not used in :ref:`install`, we
  can drop ``-D CMAKE_PREFIX_PATH=$HOME/local`` when building targets that
  require ``polo``. This is the case when ``polo`` is installed in system
  directories or when Docker images are used.

In the end, the executable should give the following output::

  Optimum: 0.0300018
  Optimizer: [-4.57459,4.61311,4.61311,].


.. rubric:: Footnotes

.. [#f1] See, for instance, `CMake Wiki
  <https://gitlab.kitware.com/cmake/community/wikis/doc/Editors>`_ for a
  comprehensive list of editors.
