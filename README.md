# bayesian-network

Developed by Linjian Li, using Clion

# Important notes


# Feature

* Structural learning with complete data
  * Chow-Liu tree (Tree-Augmented Naive Bayes (TAN))
  * K2 search algorithm (Like Weka)
  * Dynamic programming method by Ott et al.
* Gaussian Bayesian network
    * Conditional Gaussian Bayesian network
      * Discrete nodes must not have continuous parents
* Exact inference
  * Variable elimination
    * Optimal elimination order for **tree shape** network
    * Works on incomplete evidence
  * Junction tree algorithm (Some call it clique tree algorithm. But a clique tree is not necessary a junction tree.)
    * Belief propagation (a.k.a. message passing, sum-product)
    * Works on incomplete evidence
  * Elimination tree algorithm
    * For inference on Conditional Gaussian network
    * Based on the work of Cowell (2005)
    * Have not been tested yet
  * Simple brute force method
    * Like Weka
    * Fast
    * ONLY works on complete evidence
* Approximate inference
    * Probabilistic logic sampling
    * Rejection sampling
    * Markov chain Monte Carlo (MCMC) sampling
      * Gibbs sampling
    * Likelihood weighting sampling
* Construct custom network from files
  * XMLBIF (which is also supported by Weka)
* Network structure scoring
  * K2
  * BDeu
  * AIC
  * MDL/BIC


## supported problems
classification with discrete/continuous feature values

# to do:
 * test conditional gaussian Bayesian network
 * load continuous values from data file
 * classification with features of continuous values
 * explore regression
 * support graph structures beyond trees
   * Custom network
   * Structural learning algorithms
     * Hill Climbing
     * Tabu search
 * Soft evidence (help support incomplete data)
 * Incomplete data
 * Common network file format
   * Graph Modeling Language (GML)
   * GraphML (XML-based)
   * BIF (like JSON, not the same as XMLBIF)
   * arff (Weka)


# How to use

Clone the repo.
```
git clone https://github.com/jjiantong/Bayesian-network
cd Bayesian-network/lib
#under the directory of lib
git submodule init ARFF && git submodule update
git submodule init googletest && git submodule update
git submodule init tinyxml2 && git submodule update
git submodule init stats && git submodule update
git submodule init gcem && git submodule update
```

Build the project. 

Create a directory ```build``` under the root directory. Generate the make file and compile.
```
mkdir build
cd build
cmake ..
make
```

On Mac OS, the default compiler (clang) needs to be changed to g++.
```
mkdir build
cd build
cmake -DCMAKE_CXX_COMPILER=g++-9 ..
make
```

Run the test.
```
cd test
./test_exe
```


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
 
My bayesian network
* Chow-Liu Tree, Laplace smoothing alpha=2, brute force given complete instance (like Weka), accuracy of **0.827626**, **7** seconds.
* Chow-Liu Tree, Laplace smoothing alpha=2, variable elimination, accuracy of **0.823782**, 161 seconds.
* Chow-Liu Tree, alpha=2, likelihood weighting (50 samples), accuracy of **0.8248**, 636 seconds.
* Chow-Liu Tree, Laplace smoothing alpha=1, junction tree algorithm, accuracy of **0.823782**, 1335 seconds.
* Chow-Liu Tree, probabilistic logic sampling and rejection sampling (100000 samples), accuracy of **0.501147**, 5534 seconds.

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

Chow-Liu Tree, Laplace smoothing parameter alpha=1, brute force given complete instance (like Weka), accuracy of **0.5324**, 222 seconds.

Chow-Liu Tree, variable elimination, accuracy of **0.530348**, 10075 seconds.

Chow-Liu Tree, likelihood weighting (50 samples), accuracy of **0.514571**, 3620 seconds (first 50000 instances).

Chow-Liu Tree, junction tree, 2.4 instances per second, ETA 235,788 seconds (65.5 hours).
It takes such a long time because the implementation of junction tree is not parallel-friendly.
If I only test the first 50000 instances in the test set, it achieves accuracy of **0.681632** taking 21,915 seconds.

Chow-Liu Tree, probabilistic logic sampling and rejection sampling (100000 samples), accuracy of **0.143157**, 7067 seconds (first 50000 instances).

#### On Weka

Naive Bayes, accuracy of 0.521686, 34 seconds.

Bayes Net, accuracy of 0.524978, 19 seconds.

SMO, accuracy of 0.574944, 10 seconds.

## Phishing Websites Data Set from UCI

Number of instances: 11054 (2456 in old dataset)

Number of attributes: 30

Train-test data split = 1:9

Naive Bayes network, variable elimination, accuracy of **0.83546**, 12 seconds.

Naive Bayes network, Laplace smoothing parameter alpha=2, variable elimination, accuracy of **0.8368**, 10 seconds.

Naive Bayes network, Laplace smoothing alpha=1, likelihood weighting (50 samples), accuracy of **0.9009**, 38 seconds.

Naive Bayes network, Laplace smoothing parameter alpha=1, brute force given complete instance (like Weka), accuracy of **0.9023**, **0.9** seconds.

Chow-Liu Tree, Laplace smoothing alpha=1, brute force given complete instance (like Weka), accuracy of **0.9023**, **0.7** seconds.

Chow-Liu Tree, variable elimination, accuracy of **0.602674**, 11 seconds.

Chow-Liu Tree, Laplace smoothing parameter alpha=1, variable elimination, accuracy of **0.8304**, 13 seconds.

Chow-Liu Tree, likelihood weighting (50 samples), accuracy of **0.600764**, 25 seconds.

Chow-Liu Tree, Laplace smoothing parameter alpha=1, likelihood weighting (50 samples), accuracy of **0.9011**, 32 seconds.

Chow-Liu Tree, junction tree, accuracy of **0.602674**, 131 seconds.

Chow-Liu Tree, Laplace smoothing parameter alpha=1, junction tree, accuracy of **0.8304**, 113 seconds.

Chow-Liu Tree, probabilistic logic sampling and rejection sampling (100000 samples), accuracy of **0.508142**, 300 seconds.

#### On Weka

Naive Bayes, accuracy of 0.925621, 0.08 seconds.

Bayes Net, accuracy of 0.915971, 0.09 seconds.

SMO, split train:test=1:9, accuracy of 0.932858, 0.05 seconds.

#### On Scikit-Learn

MultinomialNB (alpha=1), accuracy of 0.7457, 0.004 seconds.

MultinomialNB (alpha=400), accuracy of 0.8268, 0.004 seconds.


# Dependency
 * [GoogleTest](https://github.com/google/googletest)
 * [TinyXML2](https://github.com/leethomason/tinyxml2)
 * [ARFF](https://github.com/LinjianLi/ARFF)


# blah blah
