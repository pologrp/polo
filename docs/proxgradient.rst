.. _proxgradient:

Proximal Gradient Methods
=========================

A careful review of the serial algorithms in the proximal gradient family
reveals that, given an initial search direction, which we call a *gradient
surrogate*, they differ from each other in their choices of four distinctive
algorithm primitives:

#. how they combine multiple gradient surrogates to form a better search
   direction, a step we refer to as :ref:`boosting <proxgrad-boosting>`,
#. how this search direction is filtered or scaled, which we call
   :ref:`smoothing <proxgrad-smoothing>`,
#. which :ref:`step <proxgrad-step>` size policy they use, and,
#. the type of projection they do in the :ref:`prox <proxgrad-prox>` step.

For instance, stochastic gradient descent (SGD) algorithms use partial gradient
information coming from component functions or decision vector *coordinates* as
the gradient surrogate at each iteration, whereas their aggregated versions
accumulate previous partial gradient information to boost the descent
direction. Similarly, different momentum-based methods such as the Heavyball
[1964-Polyak]_ and Nesterov's [1983-Nesterov]_ momentum accumulate the full
gradient information over iterations. Algorithms such as AdaGrad [2011-Duchi]_
and AdaDelta [2012-Zeiler]_, on the other hand, use the second-moment
information from the gradient surrogates and the decision vector updates to
adaptively scale, i.e., smooth, the gradient surrogates. Popular algorithms
such as Adam [2014-Kingma]_ and Nadam [2016-Dozat]_, available in most
machine-learning libraries, incorporate both boosting and smoothing to get
better update directions. Algorithms in the serial setting can also use
different step size policies and projections independently of the choices
above, and benefit from different :ref:`samplers <utilities-sampler>` to form
partial gradients as well as :ref:`encoders <utilities-encoder>` to compress
the gradient information. This results in the following pseudocode
representation of these algorithms:

.. math::

  & \textbf{Data: } \text{Differentiable functions, } \lbrace
    \operatorname{f_{n}}(\cdot) \rbrace; \text{regularizer, }
    \operatorname{h}(\cdot).

  & \textbf{Input: } \text{Initial decision vector, } x_{0}.

  & \textbf{Output: } \text{Final decision vector, } x_{k}.

  & \textbf{Initialize: } k \gets 0.

  & \textbf{while } \operatorname{not\_done}(k, g, x_{k},
    \operatorname{\phi}(x_{k})) \textbf{ do}

  & \quad g \gets \operatorname{gradient}(x_{k}; \operatorname{f_{n}}(\cdot));
    \texttt{ /* full or incremental */}

  & \quad g \gets \operatorname{sample}(g); \texttt{ /* block coordinate; optional */}

  & \quad e \gets \operatorname{encode}(g); \texttt{ /* optional */}

  & \quad g \gets \operatorname{decode}(e); \texttt{ /* optional */}

  & \quad g \gets \operatorname{boosting}(k, g); \texttt{ /* optional */}

  & \quad g \gets \operatorname{smoothing}(k, g, x_{k}); \texttt{ /* optional */}

  & \quad \gamma_{k} \gets \operatorname{step}(k, g, x_{k},
    \operatorname{\phi}(x_{k}));

  & \quad x_{k+1} \gets \operatorname{prox}_{\gamma_{k}\operatorname{h}}(x_{k}
    - \gamma_{k}g);

  & \quad k \gets k + 1;

  & \textbf{end}

  & \textbf{return } x_{k};

Most of the serial algorithms in this family either have direct counterparts in
the parallel setting or can be extended to have the parallel :ref:`execution
<proxgrad-execution>` support. However, adding parallel execution support
brings yet another layer of complexity to algorithm prototyping. First, there
are a variety of parallel computing environments to consider, from
shared-memory and distributed-memory environments with multiple CPUs to
distributed-memory heterogeneous computing environments that involve both CPUs
and GPUs. Second, some of these environments, such as the shared-memory, offer
different choices in how to manage race conditions. For example, some
algorithms choose to use mutexes to *consistently* read from and write to the
shared decision vector from different processes, whereas others prefer atomic
operations to allow for *inconsistent* reads and writes. Finally, the choice in
a specific computing environment might constrain choices in other algorithm
primitives. For instance, if the algorithm is to run on a distributed-memory
environment such as the Parameter Server [2013-Li]_, where only parts of the
decision vector and data are stored in individual nodes, then only updates and
projections that embrace data locality can be used.

In the rest of the chapter, we cover the five major policies that are used as
building blocks for the proximal gradient methods.

.. [1964-Polyak] Boris T. Polyak. "Some methods of speeding up the convergence
  of iteration methods." *USSR Computational Mathematics and Mathematical
  Physics* 4.5 (Jan. 1964), pp. 1-17. DOI: `10.1016/0041-5553(64)90137-5
  <https://doi.org/10.1016/0041-5553(64)90137-5>`_.
.. [1983-Nesterov] Yurii E. Nesterov. "A method for solving the convex
  programming problem with convergence rate :math:`\operatorname{\mathcal{O}}(
  1/k^{2})`." *Soviet Mathematics Doklady* 27.2 (1983), pp. 372-376.
.. [2011-Duchi] John C. Duchi, Elad Hazan, and Yoram Singer. "Adaptive
  Subgradient Methods for Online Learning and Stochastic Optimization."
  *Journal of Machine Learning Research (JMLR)* 12 (2011). pp. 2121-2159.
  ISSN: 1533-7928.
  URL: http://www.jmlr.org/papers/volume12/duchi11a/duchi11a.pdf
.. [2012-Zeiler] Matthew D. Zeiler. "ADADELTA: An Adaptive Learning Rate
  Method." (Dec. 2012). arXiv: `1212.5701 <https://arxiv.org/abs/1212.5701>`_.
.. [2014-Kingma] Diederik P. Kingma and Jimmy Ba. "Adam: A Method for
  Stochastic Optimization." (Dec. 2014). arXiv: `1412.6980
  <https://arxiv.org/abs/1412.6980>`_.
.. [2016-Dozat] Timothy Dozat. "Incorporating Nesterov Momentum into Adam."
  *ICLR Workshop*. 2016.

.. toctree::
  :hidden:

  proxgradient/boosting
  proxgradient/smoothing
  proxgradient/step
  proxgradient/prox
  proxgradient/execution
