Proximal Gradient Methods
=========================

One approach to solving instances of Problem :eq:`composite` is to use proximal
gradient methods. The basic form of the proximal gradient iteration is

.. math::
  :label: proxgrad

  x_{k+1} = \arg\,\min_{x \in \mathbb{R}^{d}} \left\lbrace \operatorname{f}(x_{k})
    + \left\langle \nabla\operatorname{f}(x_{k}), x - x_{k} \right\rangle +
    \operatorname{h}(x) + \frac{1}{2\gamma_{k}} {\left\lVert x - x_{k}
    \right\rVert}_{2}^{2} \right\rbrace \,,

where :math:`\gamma_{k}` is the step size. Thus, the next iterate,
:math:`x_{k+1}`, is selected to be the minimizer of the sum of the first-order
approximation of the smooth loss function around the current iterate,
:math:`x_{k}`, the nonsmooth loss function, and a quadratic penalty on the
deviation from the current iterate [2017-Beck]_. After some algebraic
manipulations, one can rewrite Equation :eq:`proxgrad` in terms of the proximal
operator [2017-Beck]_

.. math::

  x_{k+1} & = \arg\,\min_{x \in \mathbb{R}^{d}} \left\lbrace \gamma_{k}
  \operatorname{h}(x) + \frac{1}{2} {\left\lVert x - \left(x_{k} - \gamma_{k}
    \nabla\operatorname{f}(x_{k})\right) \right\rVert}_{2}^{2} \right\rbrace \\
  & := \operatorname{prox}_{\gamma_{k}\operatorname{h}} \left(x_{k} - \gamma_{k}
    \nabla\operatorname{f}(x_{k})\right) \,.

As a result, the method can be interpreted as a two-step procedure: first, a
query point is computed by modifying the current iterate in the direction of
the negative gradient, and then the prox operator is applied to this query
point.

Even though the proximal gradient method described in Equation :eq:`proxgrad`
looks involved, in the sense that it requires solving an optimization problem
at each iteration, the prox-mapping can actually be evaluated very efficiently
for several important functions :math:`\operatorname{h}(\cdot)` such as, for
instance, projections onto affine sets, half-spaces, boxes, and
:math:`\ell_{1}`- and :math:`\ell_{2}`-norm balls. Together with its strong
theoretical convergence guarantees, this makes the proximal gradient method a
favorable option in many applications. However, the gradient calculation step
in the vanilla proximal gradient method can be prohibitively expensive when the
number of component functions (:math:`N`) or the dimension of the decision
vector (:math:`d`) is large enough. To improve the scalability of the proximal
gradient method, researchers have long tried to come up with ways of
parallelizing the proximal gradient computations and more clever query points
than the simple gradient step in Equation :eq:`proxgrad`. As a result, the
proximal gradient family encompasses a large variety of algorithms. We have
listed some of the more influential variants in :numref:`tbl-proxgrad` [#f1]_.

.. table:: Some members of the proximal gradient methods. ``s``, ``cr``, ``ir``
  and ``ps`` stand for serial, consistent read/write, inconsistent read/write
  and Parameter Server [2013-Li]_, respectively.
  :widths: auto
  :align: center
  :name: tbl-proxgrad

  +-----------------------------+----------------+----------------+------------------------------------+--------------------+-----------------------+
  | Algorithm                   | ``boosting``   | ``smoothing``  | ``step``                           | ``prox``           | ``execution``         |
  +=============================+================+================+====================================+====================+=======================+
  | (S)GD                       | :math:`\times` | :math:`\times` | :math:`\gamma`, :math:`\gamma_{k}` | :math:`\times`     | ``s``, ``cr``, ``ps`` |
  +-----------------------------+----------------+----------------+------------------------------------+--------------------+-----------------------+
  | IAG [2007-Blatt]_           | ``aggregated`` | :math:`\times` | :math:`\gamma`                     | :math:`\times`     | ``s``, ``cr``, ``ps`` |
  +-----------------------------+----------------+----------------+------------------------------------+--------------------+-----------------------+
  | PIAG [2016-Aytekin]_        | ``aggregated`` | :math:`\times` | :math:`\gamma`                     | :math:`\checkmark` | ``s``, ``cr``, ``ps`` |
  +-----------------------------+----------------+----------------+------------------------------------+--------------------+-----------------------+
  | SAGA [2014-Defazio]_        | ``saga``       | :math:`\times` | :math:`\gamma`                     | :math:`\checkmark` | ``s``                 |
  +-----------------------------+----------------+----------------+------------------------------------+--------------------+-----------------------+
  | Heavyball [1964-Polyak]_    | ``classical``  | :math:`\times` | :math:`\gamma`                     | :math:`\times`     | ``s``                 |
  +-----------------------------+----------------+----------------+------------------------------------+--------------------+-----------------------+
  | Nesterov [1983-Nesterov]_   | ``nesterov``   | :math:`\times` | :math:`\gamma`                     | :math:`\times`     | ``s``                 |
  +-----------------------------+----------------+----------------+------------------------------------+--------------------+-----------------------+
  | AdaGrad [2011-Duchi]_       | :math:`\times` | ``adagrad``    | :math:`\gamma`                     | :math:`\times`     | ``s``                 |
  +-----------------------------+----------------+----------------+------------------------------------+--------------------+-----------------------+
  | AdaDelta [2012-Zeiler]_     | :math:`\times` | ``adadelta``   | :math:`\gamma`                     | :math:`\times`     | ``s``                 |
  +-----------------------------+----------------+----------------+------------------------------------+--------------------+-----------------------+
  | Adam [2014-Kingma]_         | ``classical``  | ``rmsprop``    | :math:`\gamma`                     | :math:`\times`     | ``s``                 |
  +-----------------------------+----------------+----------------+------------------------------------+--------------------+-----------------------+
  | Nadam [2016-Dozat]_         | ``nesterov``   | ``rmsprop``    | :math:`\gamma`                     | :math:`\times`     | ``s``                 |
  +-----------------------------+----------------+----------------+------------------------------------+--------------------+-----------------------+
  | AdaDelay [2015-Sra]_        | :math:`\times` | :math:`\times` | :math:`\gamma_{k}`                 | :math:`\checkmark` | ``s``, ``cr``, ``ps`` |
  +-----------------------------+----------------+----------------+------------------------------------+--------------------+-----------------------+
  | HOGWILD! [2011-Recht]_      | :math:`\times` | :math:`\times` | :math:`\gamma`                     | :math:`\times`     | ``ir``                |
  +-----------------------------+----------------+----------------+------------------------------------+--------------------+-----------------------+
  | ASAGA [2016-Leblond]_       | ``saga``       | :math:`\times` | :math:`\gamma`                     | :math:`\times`     | ``ir``                |
  +-----------------------------+----------------+----------------+------------------------------------+--------------------+-----------------------+
  | ProxASAGA [2017-Pedregosa]_ | ``saga``       | :math:`\times` | :math:`\gamma`                     | :math:`\checkmark` | ``ir``                |
  +-----------------------------+----------------+----------------+------------------------------------+--------------------+-----------------------+

In the rest of the chapter, we will use different preconfigured algorithms from
:numref:`tbl-proxgrad` on two different problems: :ref:`serial-logistic` and
:ref:`serial-leastsquares`.

.. [2017-Beck] Amir Beck. *First-Order Methods in Optimization (MOS-SIAM Series
  on Optimization)*. Society for Industrial and Applied Mathematics (SIAM),
  2017. ISBN: `978-1-611974-98-0 <https://isbnsearch.org/isbn/9781611974980>`_.
.. [2013-Li] Mu Li et al. "Parameter Server for Distributed Machine Learning."
  *Big Learning Workshop, Advances in Neural Information Processing Systems 26
  (NIPS).* 2013. URL:
  http://web.archive.org/web/20160304101521/http://www.biglearn.org/2013/files/papers/biglearning2013_submission_2.pdf
.. [2007-Blatt] Doron Blatt, Alfred O. Hero, and Hillel Gauchman. "A Convergent
  Incremental Gradient Method with a Constant Step Size." *SIAM Journal on
  Optimization* 18.1 (Jan. 2007), pp. 29-51. DOI: `10.1137/040615961
  <https://doi.org/10.1137/040615961>`_.
.. [2016-Aytekin] Arda Aytekin, Hamid R. Feyzmahdavian, and Mikael Johansson.
  "Analysis and Implementation of an Asynchronous Optimization Algorithm for
  the Parameter Server." (Oct. 2016). arXiv: `1610.05507
  <https://arxiv.org/abs/1610.05507>`_.
.. [2014-Defazio] Aaron Defazio, Francis Bach, and Simon Lacoste-Julien.
  "SAGA: A Fast Incremental Gradient Method with Support for Non-Strongly
  Convex Composite Objectives." *Advances in Neural Information Processing
  Systems 27 (NIPS).* Curran Associates, Inc., 2014, pp. 1646-1654. URL:
  http://papers.nips.cc/paper/5258-saga-a-fast-incremental-gradient-method-with-support-for-non-strongly-convex-composite-objectives.pdf
.. [2015-Sra] Suvrit Sra et al. "AdaDelay: Delay Adaptive Distributed
  Stochastic Convex Optimization." (Aug. 2015). arXiv: `1508.05003
  <https://arxiv.org/abs/1508.05003>`_.
.. [2011-Recht] Benjamin Recht et al. "HOGWILD!: A Lock-Free Approach to
  Parallelizing Stochastic Gradient Descent." *Advances in Neural Information
  Processing Systems 24 (NIPS).* Curran Associates, Inc., 2011, pp. 693-701.
  URL: http://papers.nips.cc/paper/4390-hogwild-a-lock-free-approach-to-parallelizing-stochastic-gradient-descent.pdf
.. [2016-Leblond] Rémi Leblond, Fabian Pedregosa, and Simon Lacoste-Julien.
  "Asaga: Asynchronous Parallel Saga." (June 2016). arXiv: `1606.04809
  <https://arxiv.org/abs/1606.04809>`_.
.. [2017-Pedregosa] Fabian Pedregosa, Rémi Leblond, and Simon Lacoste-Julien.
  "Breaking the Nonsmooth Barrier: A Scalable Parallel Method for Composite
  Optimization." *Advances in Neural Information Processing Systems 30 (NIPS).*
  Curran Associates, Inc., 2017, pp. 56-65. URL:
  http://papers.nips.cc/paper/6611-breaking-the-nonsmooth-barrier-a-scalable-parallel-method-for-composite-optimization.pdf

.. rubric:: Footnotes

.. [#f1] Meanings of ``boosting``, ``smoothing``, ``step``, ``prox``, and
  ``execution`` will be clear in :ref:`proxgradient`.
