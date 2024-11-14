Installation & First Example
============================

Clone the FastPGM repository and build. Note the ``--recursive`` option which is needed for all the submodules.

.. code-block:: bash

   git clone --recursive https://github.com/jjiantong/FastPGM.git
   cd FastPGM
   # under the directory of FastPGM
   mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && make


Run FastPGM through executable.

.. code-block:: bash

   # under the directory of FastPGM/build
   ./BayesianNetwork

By default settings, FastPGM will run the Bayesian network (BN) structure learning job via the PC-Stable method with
one thread (the sequential version) on 5000 samples generating from the ALARM BN. After successful running, you will
see key information during the structure learning procedure and the learned structure will be saved in
``FastPGM/dataset/alarm``. Command line options can be used to specify the job, method and corresponding parameters,
please see `parameters <https://fastpgm.readthedocs.io/en/latest/param_toc.html>`__.

