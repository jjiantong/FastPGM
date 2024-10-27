Installation
============

Clone the FastPGM repository and build. Note the ``--recursive`` option which is needed for all the submodules.

.. code-block:: bash

   git clone --recursive https://github.com/jjiantong/FastPGM.git
   cd FastPGM
   # under the directory of FastPGM
   mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && make

First Example
=============

Run FastPGM through executable.

.. code-block:: bash

   # under the directory of FastPGM/build
   ./BayesianNetwork

By default settings, FastPGM will run the Bayesian network (BN) structure learning job via the PC-Stable method with
one thread (the sequential version) on 5000 samples generating from the ALARM BN. After successful running, you will
see key information during the structure learning procedure and the learned structure will be saved in
``FastPGM/dataset/alarm``. Command line options can be used to specify the job, method and corresponding parameters,
please see `parameters <https://fastpgm.readthedocs.io/en/latest/param_toc.html>`__.

Supported Jobs and Methods
==========================

The following jobs and methods are available in FastPGM.

* **Structure learning**: to learn the structure (a graph) of a BN

  * PC-Stable

* **Learning**: to learn the structure (a graph) and parameters (conditional probability tables) of a BN

  * PC-Stable + maximum likelihood estimation

* **Exact inference**: to infer the exact posterior distribution of unknown variables, given observations of some variables

  * Brute force
  * Junction tree
  * Variable elimination

* **Approximate inference**: to infer the approximate posterior distribution of unknown variables, given observations of some variables

  * Loopy belief propagation
  * Probabilistic logic sampling
  * likelihood weighting
  * self-importance sampling (and variances)
  * AIS-BN
  * EPIS-BN

* Classification: to categorize based on features (variables in BNs), accomplished through the building blocks of structure learning, parameter learning and inference.
* Other functionalities related to BNs: such as sample generation, dataset and network format convertor, etc.

Please see `knowledge base <https://fastpgm.readthedocs.io/en/latest/basic_toc.html>`__ for the related basis
and terminologies.

Citing FastPGM
==============

If you find FastPGM useful, please consider citing our papers.

Fast-BNS in IPDPS 2022 `Fast Parallel Bayesian Network Structure Learning <https://ieeexplore.ieee.org/abstract/document/9820657>`__:

.. code-block:: bibtex

   @inproceedings{jiang2022fast,
     title={Fast Parallel Bayesian Network Structure Learning},
     author={Jiang, Jiantong and Wen, Zeyi and Mian, Ajmal},
     booktitle={2022 IEEE International Parallel and Distributed Processing Symposium (IPDPS)},
     pages={617--627},
     year={2022}
   }

Fast-BNS-v2 in TPDS (2024) `Parallel and Distributed Bayesian Network Structure Learning <https://ieeexplore.ieee.org/abstract/document/10292875>`__:

.. code-block:: bibtex

   @article{yang2023parallel,
     title={Parallel and Distributed Bayesian Network Structure Learning},
     author={Yang, Jian and Jiang, Jiantong and Wen, Zeyi and Mian, Ajmal},
     journal={IEEE Transactions on Parallel and Distributed Systems},
     year={2024},
     publisher={IEEE}
   }

Fast-BNI in PPoPP 2023 `Fast Parallel Exact Inference on Bayesian Networks <https://dl.acm.org/doi/abs/10.1145/3572848.3577476>`__:

.. code-block:: bibtex

   @inproceedings{jiang2023fast,
     title={Fast Parallel Exact Inference on Bayesian Networks},
     author={Jiang, Jiantong and Wen, Zeyi and Mansoor, Atif and Mian, Ajmal},
     booktitle={Proceedings of the 28th ACM SIGPLAN Annual Symposium on Principles and Practice of Parallel Programming},
     pages={425--426},
     year={2023}
   }

Dependency
==========

 * `GoogleTest <https://github.com/google/googletest>`__
 * `TinyXML2 <https://github.com/leethomason/tinyxml2>`__
 * `ARFF <https://github.com/LinjianLi/ARFF>`__
 * `StasLib <https://github.com/jjiantong/stats>`__
 * `GCE-Math <https://github.com/kthohr/gcem>`__
 * `pybind11 <https://github.com/pybind/pybind11>`__


