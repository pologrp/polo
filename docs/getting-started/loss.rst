.. _custom-loss:

Defining Custom Loss Functions
==============================

In our examples, we have thus far used the ``logistic`` loss on ``data`` that
is either defined manually or read from a dataset. However, we can also define
our custom loss functions, and pass them as the first argument to the ``solve``
member function of the algorithms. To demonstrate this, we focus on the
following simple loss function:

.. math::

  \operatorname{f}(x) = \sum_{n=1}^{N} \frac{1}{n} {\left(x^{(n)} - n\right)}^{2}

for some :math:`N \geq 1`. As can be observed, the loss is a quadratic function
of the :math:`N`-dimensional decision vector, and the optimizer of Problem
:eq:`composite` with this loss is

.. math::

  x^{\star} = \begin{bmatrix}
    1 \\ 2 \\ \vdots \\ N
  \end{bmatrix} \quad \text{with} \quad
  \operatorname{f}\left(x^{\star}\right) = 0 \,.

Furthermore, it can easily be verified that the gradient and the Hessian of the
loss are

.. math::

  \nabla\operatorname{f}(x) = \begin{bmatrix}
    \frac{2}{1} \left(x^{(1)} - 1\right) \\
    \frac{2}{2} \left(x^{(2)} - 2\right) \\
    \vdots \\
    \frac{2}{N} \left(x^{(N)} - N\right) \\
  \end{bmatrix} \quad \text{and} \quad
  \nabla^{2}\operatorname{f}(x) = \begin{bmatrix}
    2 & & & \\
    & 1 & & \\
    & & \ddots & \\
    & & & \frac{2}{N}
  \end{bmatrix} \,.

In ``polo``, a smooth loss is *any* object that, when called with two input
arguments, ``x`` and ``g``, returns the loss value associated with ``x`` and
writes the gradient in ``g``. Because the loss objects are not allowed to
modify the decision vector and because of compatibility with other (high-level)
languages, the types of ``x`` and ``g`` are ``const value_t *`` and
``value_t *``, respectively, where ``value_t`` is the type (e.g., ``double``)
of the values that the decision vector and the gradient contain. Hence, one way
to define the loss mentioned above is to create a new ``struct`` as follows

.. literalinclude:: /examples/getting-started/loss.cpp
  :language: cpp
  :lines: 11-26

where we keep the data (``N``) of the loss ``private``, and define the
``operator()`` member function appropriately. Note that, because C++ has
`zero-based numbering <https://en.wikipedia.org/wiki/Zero-based_numbering>`_,
we use ``n-1`` when indexing ``x`` and ``g``.

To use a custom loss object of type ``simple_loss``, we first construct
``loss`` with, e.g., ``N = 10``. Thanks to the simple and closed-form
representation of this simple loss function, we can easily verify that the loss
is a :math:`\mu`-*strongly convex* function with :math:`\mu =
\lambda_{\text{min}} = 2/N` and :math:`L = \lambda_{\text{max}} = 2`, where
:math:`\lambda_{\text{min}}` and :math:`\lambda_{\text{max}}` are the minimum
and maximum eigenvalues of the Hessian, respectively. We know from
[2004-Nesterov]_ that the vanilla gradient descent algorithm with constant step
size :math:`\gamma = \frac{2}{\mu + L}` converges linearly to the optimum. We
set the step size of the algorithm accordingly, and define a ``value``
terminator with absolute and relative tolerances of :math:`10^{-8}` and
:math:`10^{-13}`, respectively. The resulting code is given in
:numref:`code-loss`.

.. literalinclude:: /examples/getting-started/loss.cpp
  :language: cpp
  :caption: getting-started/loss.cpp
  :name: code-loss
  :linenos:
  :emphasize-lines: 11-26,30-33,37,43,46,51

We append the following lines to ``CMakeLists.txt``

.. literalinclude:: /examples/getting-started/CMakeLists.txt
  :language: cmake
  :lines: 39-40

and build the project. Running the executable should give the output::

  Optimum: 0.000000
  Optimizer: [1.000036,2.000000,3.000000,4.000000,5.000000,6.000000,6.999998,7.999984,8.999910,9.999641,].

Here, we observe that the optimum value is attained (up to the fixed, 6-digit
precision) by the algorithm, even though the optimizer is not. This is because
the ``value`` terminator *only* checks the absolute and relative changes in the
loss value. If we want to have a termination condition based on the changes in
the decision vector instead, we can replace

.. code-block:: cpp

  terminator::value<double, int> terminator(1E-8, 1E-13);

with

.. code-block:: cpp

  terminator::decision<double, int> terminator(1E-8);

This ``decision`` terminator with the tolerance :math:`10^{-8}` will stop the
algorithm when the following condition holds:

.. math::

  {\left\lVert x_{k-1} - x_{k} \right\rVert}^{2}_{2} < \epsilon_{\text{abs}}
    = 10^{-8} \,.

Rebuilding the project and rerunning the executable should give the following::

  Optimum: 0.000000
  Optimizer: [1.000000,2.000000,3.000000,4.000000,5.000000,6.000000,7.000000,8.000000,9.000000,10.000000,].
