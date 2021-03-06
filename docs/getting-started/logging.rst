.. _logging:

Logging and Post-Processing
===========================

So far, we have seen how to define or load data, pick a loss, select and
configure an algorithm, and run it to minimize a smooth loss,
:math:`\operatorname{f}(x)`. We have used ``getx`` and ``getf`` member
functions of ``gd`` to retreive the last decision vector (i.e., *iterate*)
generated by the algorithm and the smooth loss value at that iterate.

Generally, we are not only interested in the last iterate and the loss value
generated by the algorithm but also the sequence of states (e.g., iterates,
(partial) gradients, loss values, iteration counts, wall-clock times) the
algorithm generates. To support *logging* these states while the algorithm is
running, ``polo`` provides different :ref:`utilities-logger`. Here, we briefly
show how to log iteration counts, wall-clock times and the function values
easily to a *comma-separated values* (csv) file.

Revisiting the example in :numref:`code-svmdata`, we need to pick a proper
state logger, input the logger to the algorithm, and finally save the
(in-memory) logged states to a csv file. We provide the resulting code in
:numref:`code-logger`, with the necessary changes highlighted.

.. literalinclude:: /examples/getting-started/logger.cpp
  :language: cpp
  :caption: getting-started/logger.cpp
  :name: code-logger
  :linenos:
  :emphasize-lines: 2,34-35,40,42-48

First, we include the standard C++ `<fstream>
<https://en.cppreference.com/w/cpp/header/fstream>`_ library to be able to open
a csv file. Then, we pick a ``value`` logger, which logs the iteration counts,
wall-clock times and the loss values generated by the algorithm, and we provide
the ``logger`` to the ``solve`` method of our algorithm as the second argument.
Last, for post-processing purposes, we open a csv file, named ``logger.csv``,
and write each ``log`` line by line. Note that the ``value`` logger, by
default, outputs the iteration count, wall-clock time (in milliseconds) and the
loss value in the given order, delimited by a comma.

We append the following lines to ``CMakeLists.txt``

.. literalinclude:: /examples/getting-started/CMakeLists.txt
  :language: cmake
  :lines: 15-16

and build the project. Running the executable should give the same output as
before::

  Optimum: 229.222
  Optimizer: [0.0110083,0.162899,0.0832372,0.627515,0.968077,0.328978,0.257715,1.69923,0.556535,0.157199,-0.143509,0.328954,-0.358702,0.179352,].

However, this time, our executable has created an artifact, named
``logger.csv``. We can check, for instance, the last 5 lines of the file:

.. code-block:: shell

  # assuming that we are already in $HOME/examples/build
  tail -n 5 logger.csv
  96,5.61734,229.408
  97,5.66521,229.37
  98,5.70951,229.332
  99,5.75266,229.295
  100,5.79627,229.258

Moreover, we can use a plotting script such as that given in
:numref:`code-logger-plot` to plot the loss values with respect to iteration
counts and wall-clock times.

.. literalinclude:: /examples/getting-started/logger.py
  :language: python
  :caption: getting-started/logger.py
  :name: code-logger-plot

The resulting figure should look similar to :numref:`fig-logger`. There, we
observe the loss values plotted against the iteration counts (left) and the
wall-clock times (right).

.. figure:: /examples/getting-started/logger.*
  :name: fig-logger
  :align: center

  Loss values generated by the algorithm in :numref:`code-logger`.

.. note::

  For this example, we have used `matplotlib <https://matplotlib.org/>`_ as the
  plotting library in Python. The library can be installed easily, if there
  exists `pip <https://pip.pypa.io/en/stable/>`_ on the system, by issuing
  ``pip install --user --upgrade matplotlib``.
