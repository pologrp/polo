Loading Data
============

In :ref:`anatomy`, we have seen how to manually define problem data (cf.
matrix :math:`A` and vector :math:`b` in :numref:`code-anatomy`). This approach
is appropriate for defining small data sets; however, it gets impractical for
larger data sets that many machine-learning problems involve. To facilitate
convenient experimentation on machine-learning problems, ``polo`` includes
functionality [#f1]_ for reading data from common formats such as `LIBSVM
<https://www.csie.ntu.edu.tw/~cjlin/libsvm/>`_ [2011-Chang]_.

Now, we briefly demonstrate how ``polo`` can be used to solve a logistic
regression problem on data in the LIBSVM format. To this end, we revisit the
example in :numref:`code-anatomy`. Instead of working on the :math:`3\times3`
data matrix, we :download:`download <https://www.csie.ntu.edu.tw/~cjlin/libsvmtools/datasets/binary/australian_scale>`
``australian_scale`` file from `australian <https://www.csie.ntu.edu.tw/~cjlin/libsvmtools/datasets/binary.html#australian>`_
data set to ``data`` folder under ``$HOME/examples``. The data set contains 690
samples, each of which has 14 features and belongs to either of the two
classes: -1 or +1. Next, we change the data-related parts of
:numref:`code-anatomy` to obtain :numref:`code-svmdata` (modifications are
highlighted).

.. literalinclude:: /examples/getting-started/svmdata.cpp
  :language: cpp
  :caption: getting-started/svmdata
  :name: code-svmdata
  :linenos:
  :emphasize-lines: 12-13,18-27,31,34

As can be observed, we first call ``svm`` reader on the data file, providing
the number of samples and features the file contains, and assign its output to
the automatic variable ``data``. Then, we try to approximate the Lipschitz
smoothness of the logistic loss defined on data. This is because we are still
using the vanilla gradient descent algorithm with a constant step size, and it
is known that the iterates of this algorithm converge to the optimum (for
convex functions) if the step size satisfies :math:`\gamma < 2/L`, where
:math:`L` is the smoothness parameter [2004-Nesterov]_. Because the logistic
loss is defined as

.. math::

  \operatorname{f}(x) := \sum_{n=1}^{N} \operatorname{f_{n}}(x) = \sum_{n=1}^{N}
    \log \left( 1 + \exp \left( -b_{n} \left \langle a_{n}, x \right \rangle
    \right) \right) \,,

and each :math:`\operatorname{f_{n}}(x)` is :math:`L_{n}`-smooth with
:math:`L_{n} = {\Vert a_{n} \Vert}_{2}^{2} / 4` [2014-Xiao]_, a computationally
cheap (albeit conservative) estimate for :math:`L` is

.. math::

  L = 0.25 N L_{\text{max}} \,, \qquad L_{\text{max}} = \max_{n} {\Vert a_{n}
    \Vert}_{2}^{2} \,.

Finally, we set the step size of the algorithm, initialize it with a
zero-vector of appropriate dimension, and run the algorithm. To compile
:numref:`code-svmdata`, we add the following lines to our previous
``CMakeLists.txt``:

.. code-block:: cmake

  add_executable(svmdata svmdata.cpp)
  target_link_libraries(svmdata polo::polo)

Building the executable using CMake and running the resulting program give::

  Optimum: 229.222
  Optimizer: [0.0110083,0.162899,0.0832372,0.627515,0.968077,0.328978,0.257715,1.69923,0.556535,0.157199,-0.143509,0.328954,-0.358702,0.179352,].

.. [2004-Nesterov] Yurii Nesterov. *Introductory Lectures on Convex
  Optimization*. Springer US, 2004. DOI: `10.1007/978-1-4419-8853-9
  <https://doi.org/10.1007/978-1-4419-8853-9>`_.
.. [2011-Chang] Chih-Chung Chang and Chih-Jen Lin. "LIBSVM." *ACM Transactions
  on Intelligent Systems and Technology* 2.3 (Apr. 2011), pp. 1-27. DOI:
  `10.1145/1961189.1961199 <https://doi.org/10.1145/1961189.1961199>`_.
.. [2014-Xiao] Lin Xiao and Tong Zhang. "A Proximal Stochastic Gradient Method
  with Progressive Variance Reduction." *SIAM Journal on Optimization* 24.4
  (Jan. 2014), pp. 2057–2075. DOI: `10.1137/140961791
  <https://doi.org/10.1137/140961791>`_.

.. rubric:: Footnotes

.. [#f1] We will cover this functionality in more detail in
  :ref:`utilities-data`.
