# bayesian-network

Developed by Linjian Li, using Clion 2018.3.1

## Important notes
I have meet some difficulties about K2 and BDeu scoring function.
I implement them according to the original equations in the papers.
But the intermediate result is *too large* (e.g. factorial of 2000) so that the variable cannot store it and becomes `inf`.
I need to find other equations.


## Feature
  * Structural learning with complete data
    * Chow-Liu tree (Tree-Augmented Naive Bayes (TAN))
  * Exact inference
    * Variable elimination
      * Optimal elimination order for **tree shape** network
    * Junction tree algorithm (Some call it clique tree algorithm. But a clique tree is not necessary a junction tree.)
      * Belief propagation (a.k.a. message passing, sum-product)
  * Approximate inference
      * Probabilistic logic sampling
        * To verify the correctness of my implementation of probabilistic logic sampling, I did the following things
          1. Learn a Chow-Liu tree from LIBSVM a3a dataset
          2. Draw 10000 samples from the Chow-Liu tree by probabilistic logic sampling
          3. Convert these samples to a file of the LIBSVM data file format
          4. Learn a new Chow-Liu tree from the file from step 4
          5. Compare the CPD of each node in the new and old Chow-Liu trees
          6. Observe that the CPD of two tree are VERY similar
      * Rejection sampling
  * Construct custom network from files
    * XMLBIF (which is also supported by Weka)
    


### supported problems
classification with discrete feature values

### to do:
 * classification with features of continuous values
   * Gaussian Bayesian network ***(need to implement junction tree algorithm first)***
     * Linear Gaussian Bayesian network
       * ALL variables are continuous
       * ALL CPDs are linear Gaussians
     * Conditional Gaussian Bayesian network
       * Discrete and continuous variables
 * explore regression
 * support graph structures beyond trees
   * Custom network
   * Structural learning algorithms
     * Score-based (the asterisk means the scoring function has been implemented but not the search function)
       * K2*
       * BDe, BDeu*
       * AIC*
       * MDL*/BIC*
 * Soft evidence (help support incomplete data)
 * Incomplete data
 * Inference
   * Cowell algorithm
   * Stochastic simulation (sampling)
   * Importance sampling
   * MCMC sampling (approximate inference)
   * Gibbs sampling
   * Markov blanket sampling
   * Rejection sampling
   * Likelihood weighting
   * Exact propagation (Gaussian Bayesian networks)
 * Latent variables
 * Common network file format (rather than format created by Linjian Li)
   * Graph Modeling Language (GML)
   * GraphML (XML-based)
   * BIF (like JSON, not the same as XMLBIF)
   * arff (Weka)

The Cowell algorithm is used to do inference on continuous-value node. It is based on the earlier Lauritzen and Jensen junction tree algorithm. It is numerically stable bacause it avoids repeated matrix inversions. (McGeachie, "CGBayesNets", 2014)

## How to use
Create a directory called ```bin``` under the root directory. In Linux, you can use the following command.

```bash
mkdir bin
```

Then, go into ```bin``` directory and issue ```cmake ..``` to generate the make file.


## Performance

### LIBSVM dataset "a1a"
As a benchmark, 
 * SVM implemented by me using Python achieves accuracy of **0.8286**
 * Logistic regression achieves accuracy of **0.8332**
 * sklearn.naive_bayes.BernoulliNB achieves accuracy of **0.7935**
 * Weka
   * logistic regression: **0.832181**
   * SMO: **83.8254**
   * BayesNet: **0.774809** (in fact, weka simply learns a naive Bayes according to the output of network structure)
   * NaiveBayes: **0.803269**
   * RandomForest: **0.828886**
 
My bayesian network using **Chow-Liu Tree** and **variable elimination inference**
Consumed time and accuracy for the whose process on "a1a" (load data -> train -> test)
 * Using optimal elimination order for **tree shape** network, it takes **3 minutes** and and achieve accuracy of **0.823782**

### LIBSVM dataset "a2a"
 * Chow-Liu Tree, variable elimination, 3 minutes, accuracy of **0.826281**

### LIBSVM dataset "a3a"
 * Junction Tree, belief propagation, did not record time spent, accuracy of **0.823393**

### LIBSVM dataset "w1a"
 * Chow-Liu Tree, variable elimination, 26 minutes, accuracy of **0.972436**


## Dependency
 * Google Test (embedded in the project)
 * TinyXML-2 (embedded in the project)


## blah blah
