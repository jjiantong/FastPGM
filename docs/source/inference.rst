Inference
=========

Given a PGM, we often have some observed variables :math:`\mathcal{E} \subseteq \mathcal{V}` and the observed values
:math:`\textbf{e}` of :math:`\mathcal{E}` are called *evidence*. Inference in PGMs is the process of computing the
posterior distribution of a subset of variables :math:`\mathcal{Y} \subseteq \mathcal{V}` given observed evidence
:math:`\mathcal{E} = \textbf{e}`.

Take :numref:`Figure %s <fig_bn>` as an example, suppose we know that the patient is a smoker (:math:`\sigma=1`) and
has not been to Asia (:math:`\alpha=0`), and we want to query the probability of the patient suffering from dyspnea
(:math:`\delta`). Thus, we compute distribution over :math:`\delta` given the evidence of :math:`\{\sigma=1, \alpha=0\}`,
i.e., :math:`P(\delta | \sigma=1, \alpha=0)`. This task is central to the utility of PGMs, as it enables the model to
make predictions, update beliefs, and perform decision-making under uncertainty.

To infer the posterior distribution of :math:`Y_j \in \mathcal{Y}` given :math:`\mathcal{E} = \textbf{e}`, a common
practice is to first compute :math:`P(Y_j, \mathcal{E} = \textbf{e})` and :math:`P(\mathcal{E} = \textbf{e})`, then
combine them according to the Bayes' Theorem:

.. math::

    P(Y_j| \mathcal{E} = \textbf{e}) = \frac{P(Y_j, \mathcal{E} = \textbf{e})}{P(\mathcal{E} = \textbf{e})}.

Inference can be exact or approximate. In some simple scenarios, inference is straightforward and exact. However,
in more complex PGMs with large numbers of variables and dense connections, exact inference becomes computationally
intractable due to the exponential growth of the state space. For instance, in a network with :math:`n` binary variables,
the number of possible joint assignments is :math:`2^n`, making exact computation infeasible for large :math:`n`. In
complex PGMs, approximate inference methods can be necessary. The choice between exact and approximate inference methods
depends on the specific requirements and constraints of the application.

