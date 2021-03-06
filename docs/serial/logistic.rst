.. _serial-logistic:

Logistic Regression
===================

In the :ref:`previous chapter <getting-started>`, we used a vanilla gradient
descent (``gd``) algorithm to solve a logistic loss minimization problem.
``gd`` is the most basic member of the proximal gradient family.

In this section, we first revisit the logistic loss minimization problem, use
different (and more advanced) members of the family, i.e., Heavyball
[1964-Polyak]_, Nesterov [1983-Nesterov]_, AdaGrad [2011-Duchi]_ and Adam
[2014-Kingma]_, and compare their performances. To this end, we need to change
the algorithm-related parts of :numref:`code-terminator` to obtain
:numref:`code-serial-logistic`.

.. literalinclude:: /examples/serial/logistic.cpp
  :language: cpp
  :caption: serial/logistic.cpp
  :name: code-serial-logistic
  :linenos:
  :emphasize-lines: 4,32-49,55,63

In this example, we use `preprocessor directives
<https://en.cppreference.com/w/cpp/preprocessor>`_ to conditionally compile
parts of the file. For instance, if the preprocessor directive ``MOMENTUM`` is
defined, ``alg`` will be an instance of ``momentum`` (i.e., Heavyball), which
updates, at each iteration :math:`k`, the decision variable :math:`x_{k}` as
follows:

.. math::

  \nu_{k} & = \mu \nu_{k-1} + \epsilon g_{k}

  x_{k+1} & = x_{k} - \nu_{k} \,,

where :math:`\mu` is the *momentum term* and usually set to 0.9 [2016-Ruder]_,
:math:`g_{k}` is the gradient at :math:`x_{k}` (i.e., :math:`g_{k} = \nabla
\operatorname{f}(x_{k})`) in the example, and :math:`\nu_{k}` is the variable
that holds the exponential moving average of the gradient at iteration
:math:`k`. We set :math:`\mu = 0.9` and :math:`\epsilon = 1/L` by using
``boosting_parameters`` [#f1]_ member function of ``alg``, and ``filename`` to
``logistic-momentum.csv``, which is used for saving the logged states of the
algorithm. We select and configure [#f1]_ other algorithms similarly, and guard
them with the preprocessor directives. To compile
:numref:`code-serial-logistic` for different algorithms, we add the following
lines to ``CMakeLists.txt``:

.. literalinclude:: /examples/serial/CMakeLists.txt
  :language: cmake
  :lines: 1-3,8-11,16-19,24-27
  :emphasize-lines: 2,6,10,14

CMake's `target_compile_definitions
<https://cmake.org/cmake/help/v3.9/command/target_compile_definitions.html>`_
command helps us define the appropriate preprocessor directives. Basically,
using this command, we create four different executables from the same source
file. Building the project, running the executables, and using the plotting
script given in :numref:`code-serial-logistic-plot` result in a figure similar
to :numref:`fig-serial-logistic`.

.. literalinclude:: /examples/serial/logistic.py
  :language: python
  :caption: serial/logistic.py
  :name: code-serial-logistic-plot

.. figure:: /examples/serial/logistic.*
  :name: fig-serial-logistic
  :align: center

  Loss values generated by the algorithms in :numref:`code-serial-logistic`.

In :numref:`fig-serial-logistic`, we observe that all four algorithms converge
roughly to the same loss value when a value terminator with an absolute value
tolerance of ``5E-2`` is used. Both ``momentum`` and ``nesterov`` require the
knowledge of the Lipschitz parameter (:math:`L`) of the logistic loss, and they
have similar convergence profiles. ``adagrad`` and ``adam``, on the other hand,
are configured *without* using the knowledge of :math:`L` (cf.
:numref:`code-serial-logistic`), and they can *adapt* their learning rates to
the loss function at hand. For instance, in ``adagrad``, there is an increase
in the loss value initially resulting from a too big initial step size, which
is later accounted for as the algorithm proceeds. It is also worth noting that,
in ``polo``, the default initial step size for ``adagrad`` is 1, and the
configuration in :numref:`code-serial-logistic` for ``adam`` is the one
suggested in [2014-Kingma]_. Both algorithms can be tuned to have better
performances than the ones obtained in this example.

Now, we add different regularizers to the problem, and repeat the same
procedure to solve the corresponding regularized problems.

:math:`\ell_{1}` Regularization
-------------------------------

Elastic Net Regularization
--------------------------

.. [2016-Ruder] Sebastian Ruder. "An overview of gradient descent optimization
  algorithms." (Sep. 2016). arXiv: `1609.04747
  <https://arxiv.org/abs/1609.04747>`_.

.. rubric:: Footnotes

.. [#f1] The configuration of the algorithms and the terminology will become
  more evident in :ref:`proxgradient`.
