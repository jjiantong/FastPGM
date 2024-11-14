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
