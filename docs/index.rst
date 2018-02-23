PBOpt: Policy-Based Optimization Toolbox
========================================

Policy-Based Optimization Toolbox (PBOpt) is a C++ header-only library. It is
intended to help researchers and practitioners solve optimization problems of
the form

.. math::

    \begin{align}
      \begin{aligned}
        & \operatorname*{minimize}_{x \in \mathbb{R}^{d}}
        & & \sum_{n=1}^{N} \operatorname{f_{n}}(x) + \operatorname{h}(x) \,.
      \end{aligned}
    \end{align}

Above, each :math:`\operatorname{f_{n}}(x)` is a differentiable function of
the decision variable :math:`x` whereas :math:`\operatorname{h}(x)` can be
non-differentiable. Typically, in machine-learning applications,
:math:`\operatorname{F}(x) := \sum_{n=1}^{N} \operatorname{f_{n}}(x)` generally
encodes the data loss while :math:`\operatorname{h}(x)` is used to enforce some
structure on the solution.

The library provides some useful *policies* and algorithm abstractions that
use these policies in a modular way. By leveraging on template metaprogramming
capabilities of C++, the library can be used to prototype, test and deploy
different algorithms in a flexible, yet, efficient way.

How to Install
--------------

Because PBOpt is header-only, it is enough to copy the contents of the
``include`` directory to one of the search paths of your compiler. If you have
cmake_ installed on your system, you can issue the following commands on your
terminal:

.. _cmake: https://cmake.org/

.. code-block:: shell

    git clone https://github.com/aytekinar/pbopt
    cd pbopt
    mkdir build
    cd build
    cmake -D CMAKE_INSTALL_PREFIX=$HOME/local ../
    cmake --build . --target install

This will install the library into ``$HOME/local/include``, where ``$HOME``
usually points to ``/home/<username>`` on Linux systems. Once the installation
is complete, you can either configure your compiler to search for the header
files under ``$HOME/local/include`` or use the switch ``-I $HOME/local/include``
whenever you compile your project that uses PBOpt.

What Next
---------

Having installed the library successfully, you continue reading to understand
how things work :ref:`under the hood <explanation start>`, or you can jump to
the :ref:`examples <example start>` section to have a walkthrough.

.. toctree::
   :glob:
   :hidden:
   :maxdepth: 2
   :Caption: Algorithm Abstraction

   algorithm/singlestage

.. toctree::
   :glob:
   :hidden:
   :maxdepth: 2
   :Caption: Gradient Policy

   gradient/momentum
   gradient/incremental

.. toctree::
   :glob:
   :hidden:
   :maxdepth: 2
   :Caption: Smoothing Policy

   smoothing

.. toctree::
   :glob:
   :hidden:
   :maxdepth: 2
   :Caption: Step-Size Policy

   step

.. toctree::
   :glob:
   :hidden:
   :maxdepth: 2
   :Caption: Projection Policy

   projection

.. toctree::
   :glob:
   :hidden:
   :maxdepth: 2
   :Caption: Execution Policy

   execution/serial
   execution/multithread

.. toctree::
   :glob:
   :hidden:
   :maxdepth: 2
   :Caption: Utilities

   utility/terminator
   utility/sampler
   utility/logger
   utility/matrix
   utility/problem

.. toctree::
   :glob:
   :hidden:
   :maxdepth: 2
   :Caption: Examples

   examples/*

.. toctree::
   :glob:
   :hidden:
   :maxdepth: 2
   :Caption: Further Topics

   further/*
