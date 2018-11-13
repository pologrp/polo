=========================================
POLO: a POLicy-based Optimization library
=========================================

POLicy-Based Optimization Framework (POLO) is a C++ header-only library. It is
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

.. toctree::
  :hidden:
  :maxdepth: 3

  installation
  algorithms
  policies
  utilities
  api
  examples
