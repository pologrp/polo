.. YAPOT documentation master file, created by
   sphinx-quickstart on Fri Feb  9 09:51:58 2018.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to YAPOT's documentation!
=================================

.. toctree::
   :glob:
   :hidden:
   :maxdepth: 2
   :Caption: Algorithm Abstraction

   algorithm/singlestage.rst

.. toctree::
   :glob:
   :hidden:
   :maxdepth: 2
   :Caption: Descent Policy

   descent/momentum.rst
   descent/nesterov.rst

.. toctree::
   :glob:
   :hidden:
   :maxdepth: 2
   :Caption: Smoothing Policy

   smoothing/none.rst
   smoothing/adagrad.rst
   smoothing/rmsprop.rst
   smoothing/adadelta.rst

.. toctree::
   :glob:
   :hidden:
   :maxdepth: 2
   :Caption: Step-Size Policy

   step/constant.rst
   step/decreasing.rst

.. toctree::
   :glob:
   :hidden:
   :maxdepth: 2
   :Caption: Projection Policy

   projection/none.rst

.. toctree::
   :glob:
   :hidden:
   :maxdepth: 2
   :Caption: Execution Policy

   execution/serial.rst

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
