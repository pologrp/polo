=========================================
POLO: a POLicy-based Optimization library
=========================================

``polo`` is a header-only C++ library intended to help researchers and
practitioners solve optimization problems of the form

.. math::
  :label: composite

  \begin{aligned}
    & \operatorname*{minimize}_{x \in \mathbb{R}^{d}}
    & & \operatorname{\phi}(x) := \operatorname{f}(x) + \operatorname{h}(x)
  \end{aligned}

on different computing platforms. Here, the total loss consists of two parts.
The first part, :math:`\operatorname{f}(\cdot)`, is a *smooth* function of the
:math:`d`-dimensional decision vector, :math:`x`, and it consists of
:math:`N` *component* functions:

.. math::

  \operatorname{f}(x) = \sum_{n=1}^{N} \operatorname{f_{n}}(x) \,.

The second part, :math:`\operatorname{h}(\cdot)`, is a possibly *nonsmooth*
function of the decision vector, :math:`x`.

In this documentation, we aim at providing the necessary information for both
users and future contributors to get started with ``polo``. In
:ref:`getting-started`, we show how to install the library and what a typical
workflow involving ``polo`` looks like. Then, in three parts, we cover
different aspects of the library.

In the first part, we follow a tutorial-based approach to show users how to use
``polo`` for solving Problem :eq:`composite` on different computing platforms.
In :ref:`serial`, we focus on building *proximal gradient algorithms* that run
*sequentially* on a single CPU. Then, in :ref:`shared-memory`, we switch to
parallel versions of the proximal gradient algorithms, where all the component
functions and the decision vector reside in a *shared* memory space. These
algorithms benefit from multiple CPUs to speed up gradient computations of the
smooth loss. Finally, in :ref:`distributed-memory`, we focus on solving
Problem :eq:`composite` when the component functions and the decision vector
are *distributed* among different nodes.

The second part of the documentation covers more advanced topics that are
mostly related to the internals of the library. These topics would serve as a
starting point for users and contributors to extend the functionalities of the
library. In :ref:`proxgradient`, we cover the abstraction for the proximal
gradient algorithms, and provide detailed information on their *policies*.
Then, in :ref:`utilities`, we cover the functionalities provided by the
``utilities`` layer of the library. Finally, in :ref:`c-api`, we provide the C
API of the library, which implements many algorithms from the proximal gradient
family and can be used from high-level languages to solve Problem
:eq:`composite`.

In the last part, we provide high-level language integrations of ``polo``.
Currently, ``polo`` is wrapped and extended in the Julia language, and we will
cover the library in :ref:`polo-jl`.

.. toctree::
  :hidden:
  :maxdepth: 3

  getting-started

.. toctree::
  :caption: Tutorials
  :hidden:
  :maxdepth: 3

  serial
  shared-memory
  distributed-memory

.. toctree::
  :caption: Advanced Topics
  :hidden:
  :maxdepth: 3

  proxgradient
  utilities
  c-api

.. toctree::
  :caption: High-Level Language Integrations
  :hidden:
  :maxdepth: 3

  julia
