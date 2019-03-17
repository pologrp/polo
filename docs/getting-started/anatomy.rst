.. _anatomy:

Anatomy of a POLO Program
=========================

Programs that use ``polo`` are written in a single C++ file, as shown in
:numref:`code-anatomy`.

.. note::

  All the code samples presented in this documentation are provided under
  ``docs/examples`` directory in the source tree. Listing captions give the
  relative path to the sample file (without the ``.cpp`` extension). For
  example, ``getting-started/anatomy`` refers to ``anatomy.cpp`` file under
  ``docs/examples/getting-started``.

.. literalinclude:: /examples/getting-started/anatomy.cpp
  :language: cpp
  :caption: getting-started/anatomy
  :name: code-anatomy
  :linenos:

As can be observed, after including the system libraries and ``polo`` in our
C++ file, we follow the following five steps:

#. Define the problem data,
#. Define the smooth loss on the data,
#. Select and configure the desired solver,
#. Initialize the solver and run it to minimize the total loss, and,
#. Print the result.

In :numref:`code-anatomy`, we first define the problem data (``data``) using
the following (dense) matrix (``dmatrix``) and vector

.. math::

  A = \begin{bmatrix}
    1 & 0 & 0 \\
    0 & 1 & 0 \\
    0 & 0 & 1
  \end{bmatrix} \,, \qquad b = \begin{bmatrix}
    -1 \\ 1 \\ 1
  \end{bmatrix} \,,

both of which have values of type ``double`` and indices of type ``int``. Then,
we pick a predefined loss of type ``logistic`` on ``data``, which describes the
smooth loss

.. math::

  \operatorname{f}(x) = \sum_{n=1}^{N} \log \left( 1 + \exp \left( -b_{n} \left
    \langle a_{n}, x \right \rangle \right) \right) \,,

where :math:`a_{n}` and :math:`b_{n}` are the :math:`n^{\text{th}}` row of
:math:`A` and :math:`b`, respectively. Currently, ``polo`` supports several
common smooth loss functions (see :ref:`utilities-loss`). In addition, we can
define custom loss functions in just a few lines of code. We will discuss this
in more detail in :ref:`custom-loss`. After defining the smooth loss, we select
a vanilla gradient descent (``gd``) algorithm, and set its constant step size.
In fact, ``gd`` is just an alias for a specific algorithm among a number of
well-known optimization algorithms that ``polo`` supports from the proximal
gradient family. ``polo`` not only allows users to configure, modify, and
extend these algorithms in different ways but also supports the creation of
completely new algorithms. We will describe this functionality in detail later
in :ref:`proxgradient`. After configuring the solver, we ``initialize`` it with
a decision vector, :math:`x_{0} = {[1, 1, 1]}^{\top}`, and ``solve`` the
optimization problem defined on ``loss``. Finally, we get the final decision
vector (``getx``) that the gradient descent algorithm produces, together with
the associated loss value (``getf``), and print the result.
