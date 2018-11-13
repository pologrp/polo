Proximal Gradient Algorithms
============================

In POLO, *proximal gradient algorithms* represent the family of algorithms that
attempt to solve the optimization problem

.. math::

  \begin{align}
    \begin{aligned}
      & \operatorname*{minimize}_{x \in \mathbb{R}^{d}}
      & & \sum_{n=1}^{N} \operatorname{f_{n}}(x) + \operatorname{h}(x)
    \end{aligned}
  \end{align}

by applying the *proximal gradient method*

.. math::

  \begin{align}
    x_{k+1} = \operatorname{arg}\,\operatorname*{min}_{x}
      \left\lbrace \operatorname{F}\left(x_{k}\right) +
      \left\langle g_{k}, x-x_{k}\right\rangle
      + \frac{1}{2\gamma_{k}}{\Vert x-x_{k} \Vert}_{2}^{2} +
      \operatorname{h}(x) \right\rbrace
  \end{align}

at each iteration :math:`k`. Here, :math:`g_{k}` represents some surrogate of
the gradient of the smooth loss function :math:`\operatorname{F}(x)` at the
decision variable :math:`x_{k}`, and :math:`\gamma_{k}` is the step size.

Algorithms in this family are composed of the following components, called the
*policies* [#]_ in POLO:

``boosting``
  Boosting policy of a given algorithm determines how to use the first-order
  information of a given smooth ``loss`` function.

  Some popular choices of this policy include classical momentum, Nesterov
  momentum and incremental gradient variants.

``smoothing``
  Smoothing policy of a given algorithm scales the coordinates of the gradient
  surrogate, :math:`g_{k}`, by taking into account the variance of the
  surrogates as the algorithm iterations proceed. Roughly speaking, different
  choices in this policy are used as different surrogates of the second-order
  information, *i.e.*, the inverse Hessian, of a given smooth ``loss`` function.

  Some popular choices are AdaGrad, RMSPRop, AdaDelta and AdaMax.

``step``
  The step size can also be chosen independently of the gradient surrogate used.
  The scalar step size, :math:`\gamma_{k}`, is a trade-off between minimizing
  the linear approximation of the smooth loss function and staying in the
  proximity of the current decision variable, when calculating the new decision
  variable.

  Choices in this policy include open-loop options such as the constant and
  the decreasing step size as well as the adaptive variants such as AdaDelay
  and the hypergradient descent method.

``prox``
  Prox policy of a given algorithm deals with solving the proximal optimization
  problem above, given the current decision variable, the (smoothed) gradient
  surrogate and the step size.

  Depending on the non-smooth function, :math:`\operatorname{h}(x)`, the options
  in this policy vary from set projections such as projecting onto orthants,
  affine sets and half-spaces to proximal calculations such as the
  :math:`\ell_{1}`-norm.

``execution``
  Execution policy determines how the aforementioned policies should be executed
  when solving a given problem.

  Some choices are the serial executor, consistent and inconsistent read/write
  multi-threaded executors for shared-memory architectures, and the Parameter
  Server [#]_ executor for distributed-memory architectures.

.. rubric:: Footnotes
.. [#] The term *policy* refers to the `programming paradigm`_, first popularized
       by Andrei Alexandrescu.
.. [#] M. Li *et al.*, "Parameter Server for Distributed Machine Learning," in
       *Big Learning Workshop, Advances in Neural Information Processing Systems
       26 (NIPS)*, 2013.

.. _`programming paradigm`: https://en.wikipedia.org/wiki/Policy-based_design
