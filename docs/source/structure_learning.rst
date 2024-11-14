Structure Learning
==================

Structure learning in PGMs is a fundamental task that involves determining the optimal graphical structure
:math:`\mathbb{G}` that is well matched the observed data. This structure :math:`\mathbb{G}`, whether directed as in
BNs or undirected as in MRFs, captures the dependencies among the variables and is crucial for both understanding the
underlying probabilistic relationships. Meanwhile, structure learning also facilitates subsequent tasks such as
inference, prediction, and decision-making. The learned structure serves as a foundation for building more complex
models, particularly in domains such as genetics, epidemiology, and natural language processing, where understanding
the dependencies between variables is critical.

The challenge in structure learning lies in the combinatorial nature of the problem. Given a set of :math:`n` variables,
the number of possible graph structures grows super-exponentially with :math:`n`, making exhaustive search infeasible
for all but the smallest sets of variables.

There are two common approaches for structure learning from data: score-based approaches and constraint-based approaches.


Score-based Methods
-------------------

The score-based approaches use a scoring function to measure the fitness of the structures (i.e., graphs) to the data
and find the highest score out of all the possible graphs. Therefore, these methods make the number of possible graphs
super-exponential to the number of dimensions (i.e., :math:`n`) of the learning problems.

To find the best or the top-k best structure(s) of the PGM, we need some metrics that enable us to compare the qualities
of different structures. The metrics are known as *scoring functions*, whose inputs are a possible structure and the
training dataset, and then measure the ability of the PGM to represent the distribution of the data. Some commonly used
scoring functions include Bayesian Dirichlet (BD), K2, BDe, BDeu, log-likelihood, Minimum Description Length (MDL),
Bayesian Information Criterion (BIC), Akaike Information Criterion (AIC), and so on.


Constraint-based Methods
------------------------

The constraint-based approaches perform a number of *conditional independence (CI) tests* to identify the independence
relations among the random variables and use these relations as constraints to construct PGMs. This category of methods
often runs in a polynomial time, and is commonly used in high-dimensional problems.

Here we introduce the important concept of CI tests (based on BNs). Consider some random variables :math:`V_i`,
:math:`V_j` and :math:`V_k`, a CI test assertion of the form :math:`I(V_i, V_j | \{V_k\})` means :math:`V_i` and
:math:`V_j` are independent given :math:`V_k`. Let :math:`\mathcal{D} = \{c_0, c_1, ..., c_{m-1}\}` denote a data set of
:math:`m` complete samples, a CI test :math:`I(V_i, V_j | \{V_k\})` determines whether the corresponding hypothesis
:math:`I(V_i, V_j | \{V_k\})` holds or not, based on statistics of :math:`\mathcal{D}`.

For discrete variables, the most common statistic for testing :math:`I(V_i, V_j | \{V_k\})` is the :math:`G^2` test
statistic, which is defined as

.. math::

    G^2 = 2 \sum_{x, y, z} N_{xyz} log \frac{N_{xyz}}{E_{xyz}},

where :math:`N_{xyz}` is the number of samples in :math:`\mathcal{D}` that satisfy :math:`V_i = x`, :math:`V_j = y` and
:math:`V_k = z`. The value of :math:`N_{xyz}` can be obtained from the contingency table that shows the frequencies for
all configurations of values. :math:`G^2` follows an asymptotic :math:`\chi^2` distribution with
:math:`(|V_i|-1)(|V_j|-1)`, where :math:`|\cdot|` denotes the number of possible values of the variable. The p value of
:math:`\chi^2` distribution can be calculated according to the :math:`G^2` statistic and the final decision is made by
comparing p value with the significance level :math:`\alpha`. If p value is greater than :math:`\alpha`, the independent
hypothesis :math:`I(V_i, V_j | \{V_k\})` is accepted; otherwise, the hypothesis is rejected. :math:`E_{xyz}` is the
expected frequency which is defined as

.. math::

    E_{xyz} = \frac{N_{x+z} N_{+yz}}{N_{++z}},

where :math:`N_{x+z} = \sum_{y} N_{xyz}$, $N_{+yz} = \sum_{x} N_{xyz}`, and :math:`N_{++z} = \sum_{xy} N_{xyz}`, which
represent the marginal frequencies.


PC-stable
^^^^^^^^^

A fundamental constraint-based algorithm is the PC (named after its authors Peter and Clark) algorithm, and PC-stable
solves the order-dependent issue in the original PC algorithm and produces less error. PC-stable is important since
most constraint-based methods are improved versions of the PC-stable algorithm or proceed along similar lines of the
PC-stable algorithm.




