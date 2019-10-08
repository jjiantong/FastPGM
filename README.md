# bayesian-network

Developed by Linjian Li, using Clion

# Important notes


# Feature

* Structural learning with complete data
  * Chow-Liu tree (Tree-Augmented Naive Bayes (TAN))
* Gaussian Bayesian network
    * Conditional Gaussian Bayesian network
      * Discrete nodes must not have continuous parents
* Exact inference
  * Variable elimination
    * Optimal elimination order for **tree shape** network
  * Junction tree algorithm (Some call it clique tree algorithm. But a clique tree is not necessary a junction tree.)
    * Belief propagation (a.k.a. message passing, sum-product)
  * Elimination tree algorithm
    * for inference on Conditional Gaussian network
    * based on the work of Cowell (2005)
* Approximate inference
    * Probabilistic logic sampling
    * Rejection sampling
    * Markov chain Monte Carlo (MCMC) sampling
      * Gibbs sampling
    * Likelihood weighting sampling
* Construct custom network from files
  * XMLBIF (which is also supported by Weka)


## supported problems
classification with discrete/continuous feature values

# to do:
 * load continuous values from data file
 * classification with features of continuous values
 * explore regression
 * support graph structures beyond trees
   * Custom network
   * Structural learning algorithms
     * Score-based (the asterisk means the scoring function has been implemented but not the search function)
       * K2*
       * BDeu*
       * AIC*
       * MDL*/BIC*
 * Soft evidence (help support incomplete data)
 * Incomplete data
 * Common network file format
   * Graph Modeling Language (GML)
   * GraphML (XML-based)
   * BIF (like JSON, not the same as XMLBIF)
   * arff (Weka)


# How to use
Create a directory called ```bin``` under the root directory. In Linux, you can use the following command.

```bash
mkdir bin
```

Then, go into ```bin``` directory and issue ```cmake ..``` to generate the make file.


# Performance

## LIBSVM dataset "a1a"
As a benchmark, 
 * SVM implemented by me using Python achieves accuracy of **0.8286**
 * Logistic regression achieves accuracy of **0.8332**
 * sklearn.naive_bayes.BernoulliNB achieves accuracy of **0.7935**
 * Weka
   * logistic regression: **0.832181**
   * SMO: **0.838254**
   * BayesNet: **0.832084**
   * NaiveBayes: **0.803269**
   * RandomForest: **0.828886**
 
My bayesian network using **Chow-Liu Tree** and **variable elimination inference**
Consumed time and accuracy for the whose process on "a1a" (load data -> train -> test)
 * Using optimal elimination order for **tree shape** network, it achieves accuracy of **0.823782**

## LIBSVM dataset "a2a"
 * Chow-Liu Tree, variable elimination, accuracy of **0.826281**

## LIBSVM dataset "a3a"
 * Junction Tree, belief propagation, accuracy of **0.823393**
 * Chow-Liu Tree, likelihood weighting, accuracy of **0.806451**

## LIBSVM dataset "w1a"
 * Chow-Liu Tree, variable elimination, accuracy of **0.972436**

## Covertype Data Set from UCI

Number of instances: 581012
* first 11,340 records used for training data subset
* next 3,780 records used for validation data subset
* last 565,892 records used for testing data subset

Number of attributes: 54

Chow-Liu Tree, variable elimination, accuracy of **0.457765**, 232 seconds.

Chow-Liu Tree, likelihood weighting (50 samples), accuracy of **0.514571**, 3620 seconds.

Chow-Liu Tree, junction tree, 2.4 instances per second, ETA 235,788 seconds (65.5 hours).
It takes such a long time because the implementation of junction tree is not parallel-friendly.
If I only test the first 50000 instances in the test set, it achieves accuracy of **0.681632** with 21,915 seconds.

Chow-Liu Tree, probabilistic logic sampling and rejection sampling (100000 samples), accuracy of **0.143157**, 7067 seconds.

## Phishing Websites Data Set from UCI

Number of instances: 11054 (2456 in old dataset)

Number of attributes: 30

Chow-Liu Tree, variable elimination, accuracy of **0.924914**, 11 seconds.

Chow-Liu Tree, likelihood weighting (50 samples), accuracy of **0.923648**, 25 seconds.

Chow-Liu Tree, junction tree, accuracy of **0.924914**, 131 seconds.

Chow-Liu Tree, probabilistic logic sampling and rejection sampling (100000 samples), accuracy of **0.500045**, 35 seconds.



# Dependency
 * Google Test (embedded in the project)
 * TinyXML-2 (embedded in the project)


# blah blah
