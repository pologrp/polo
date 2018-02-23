.. _example start:

Gradient Descent
================

Let us assume that we have the following unconstrained quadratic programming
problem:

.. math::

    \begin{align}
      \begin{aligned}
        & \operatorname*{minimize}_{x \in \mathbb{R}^{3}}
        & & \frac{1}{2} x^{\top} \begin{bmatrix}
          1 & 0 & 0 \\
          0 & 2 & 0 \\
          0 & 0 & 3
        \end{bmatrix} x + {\begin{bmatrix}
          -0.1 \\ 0.2 \\ -0.3
        \end{bmatrix}}^{\top} x \,.
      \end{aligned}
    \end{align}

It is easy to verify that the optimizer for the problem is :math:`x^{\star} =
[0.1,-0.1,0.1]` with the corresponding optimum value :math:`f^{\star} = -0.03`.

One possible way to define the above ``loss`` function in C++ is to write a
function of the form:

.. literalinclude:: /../examples/gradient.cpp
   :language: cpp
   :lines: 4-18

In PBOpt, algorithm instances that use the gradient information require that
``loss`` objects are callable with two pointers to the underlying floating-point
type. The first parameter, ``x``, points to the first element of the current
decision vector and the second parameter, ``g``, points to the first element
of the corresponding gradient. The ``loss`` object is only allowed to modify the
gradient value, and hence, the signature has ``const`` qualifier for ``x``.
Finally, the loss objects are required to return the function value at the given
``x``. In the above example, the user is working with ``double`` precision
floating-point numbers.

Having defined our ``loss`` object, we can select an algorithm to solve the
minimization problem defined by the smooth ``loss``. Below is a piece of code
to achieve this, which is also provided as ``examples/gradient.cpp`` in the
repository.

.. literalinclude:: /../examples/gradient.cpp
   :language: cpp

In PBOpt, the ``algorithm::singlestage`` class is an abstraction of a family of
algorithms. With the default settings, it represents the gradient descent
algorithm with a constant step-size. For convex quadratic minimization problems,
it is known that the best constant step-size for the gradient descent iterations
is :math:`\gamma_{k} = 2/(\mu + L)`, which is equal to 0.5 in the above example.
Hence, after selecting our algorithm, we provide the constant step size with the
``step_params`` method call. We, then, initialize the algorithm with some
decision vector, and solve the problem. By default, ``solve`` method iterates
100 times until the algorithm terminates. Finally, we use the standard C++
printing functionality to obtain the result:

.. code:: bash

    Optimizer is [0.1,-0.1,0.1,].
    Optimum value is -0.03.
