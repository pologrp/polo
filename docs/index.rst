=========================================
POLO: a POLicy-based Optimization library
=========================================

``polo`` is a header-only C++ library intended to help researchers and
practitioners solve optimization problems of the form

.. math::
  :label: composite

  \begin{align}
    \begin{aligned}
      & \operatorname*{minimize}_{x \in \mathbb{R}^{d}}
      & & \operatorname{\phi}(x) := \operatorname{F}(x) + \operatorname{h}(x)
    \end{aligned}
  \end{align}

on different computing platforms. Here, :math:`\operatorname{F}(\cdot)` is a
smooth function of the :math:`d`-dimensional decision vector, :math:`x`, and it
consists of :math:`N` *component* functions:

.. math::

  \begin{align*}
    \operatorname{F}(x) := \sum_{n=1}^{N} \operatorname{f_{n}}(x) \,.
  \end{align*}

:math:`\operatorname{h}(\cdot)`, on the other hand, is a possibly nonsmooth
function of the decision vector, :math:`x`.

In the documentation, we follow a tutorial approach when introducing new
concepts.

First, in :ref:`getting-started`, we show how to install the library and
what a typical workflow involving ``polo`` looks like.

Later, in three different chapters, we cover how different algorithms are
*executed*. In :ref:`serial`, we focus on building *proximal gradient
algorithms* that run *sequentially* to solve Problem :eq:`composite`. Then, in
:ref:`shared-memory`, we switch to parallel versions of the proximal gradient
algorithms, where all the component functions and the decision vector reside in
a *shared* memory space. These algorithms benefit from multiple cores to speed
up gradient calculations of the smooth loss. Finally, in
:ref:`distributed-memory`, we focus on solving Problem :eq:`composite` where
the component functions and the decision vector are *distributed* among
different nodes.

We leave the more advanced topics related to the internals of the library in
:ref:`advanced`.

.. toctree::
  :hidden:
  :maxdepth: 3

  getting-started
  serial
  shared-memory
  distributed-memory
  advanced
