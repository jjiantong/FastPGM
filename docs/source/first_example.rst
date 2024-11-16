First Example
=============

You can start FastPGM by running the built executable without any arguments, using the default configuration.

.. code-block:: bash

   # under the directory of FastPGM/build
   ./BayesianNetwork

By default settings, FastPGM will run the structure learning job via the PC-Stable method with one thread (the
sequential version) on 5000 samples generating from the ALARM Bayesian network. After successful running, you will
see key information during the structure learning procedure and the learned structure will be saved in
``FastPGM/dataset/alarm``. Command line options can be used to specify the job, method and corresponding parameters,
please see `parameters <https://fastpgm.readthedocs.io/en/latest/param_toc.html>`__.