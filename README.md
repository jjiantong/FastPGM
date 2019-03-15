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
  * Construct custom network from files
    * Format of custom network structure
      * The first line is a string "BEGIN_OF_STRUCTURE"
      * The last line of the structure is "END_OF_STRUCTURE"
      * The second line is an integer of the number of nodes
      * Each line from the third till the second to the last represents a node and its child(ren) separated by whitespace
      * The node index should start at 0 instead of 1

      for example:
      ```
      BEGIN_OF_STRUCTURE
      4
      0 -> 1 2
      1 -> 2 3
      2 -> 3
      3 ->
      END_OF_STRUCTURE
      ```
    * Format of custom network parameters
      * The first line is a string "BEGIN_OF_PARAMETERS"
      * The last line is "END_OF_PARAMETERS"
      * Suppose there are *n* nodes
      * Each of the first *n* lines after "BEGIN_OF_PARAMETERS" is the domain of each node
      * Each of the second *n* lines after "BEGIN_OF_PARAMETERS" is the value of each node's query, condition, and probability, separated by whitespace
        * The first integer is this node's index
        * If the node has no parent, there should be "--marg". If it has parent(s), there should be "--cond"
        * The query value and the condition are separated by "|", and the probability value comes after "@"
        * If the condition consists of multiple nodes, nodes are separated by ","
        * For each node in the condition, its index and value are separated by ":"

      for example:
      ```
      BEGIN_OF_PARAMETERS
      0 : 0 1
      1 : 0 1
      2 : -1 1
      3 : -1 1
      0 --marg 0@0.3 1@0.7
      1 --cond 0|0:0@0.3 1|0:0@0.7 0|0:1@0.4 1|0:1@0.6
      2 --cond -1|0:0,1:0@0.3 1|0:0,1:0@0.7 -1|0:1,1:0@0.2 1|0:1,1:0@0.8 -1|0:0,1:1@0.1 1|0:0,1:1@0.9 -1|0:1,1:1@0.5 1|0:1,1:1@0.5 
      3 --cond -1|1:0,2:-1@0.3 1|1:0,2:-1@0.7 -1|1:1,2:-1@0.2 1|1:1,2:-1@0.8 -1|1:0,2:1@0.1 1|1:0,2:1@0.9 -1|1:1,2:1@0.5 1|1:1,2:1@0.5
      END_OF_PARAMETERS
      ```

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
       * K2
       * BDe, BDeu
       * AIC*
       * MDL*/BIC*
 * Soft evidence (help support incomplete data)
 * Incomplete data
 * Inference
   * Cowell algorithm
   * MCMC sampling (approximate inference)
   * Gibbs sampling
   * Rejection sampling
   * Exact propagation (Gaussian Bayesian networks)
 * Latent variables
 * Common network file format (rather than format created by Linjian Li)
   * Graph Modeling Language (GML)
   * GraphML (XML-based)
   * BIF (supported by Weka)
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
 
My bayesian network using **Chow-Liu Tree** and **variable elimination inference**
Consumed time and accuracy for the whose process on "a1a" (load data -> train -> test)
 * Using optimal elimination order for **tree shape** network, it takes **3 minutes** and and achieve accuracy of **0.823782**

### LIBSVM dataset "a2a"
 * Chow-Liu Tree, variable elimination, 3 minutes, accuracy of **0.826281**

### LIBSVM dataset "a3a"
 * Junction Tree, belief propagation, did not record time spent, accuracy of **0.823393**

### LIBSVM dataset "w1a"
 * Chow-Liu Tree, variable elimination, 26 minutes, accuracy of **0.972436**

## blah blah
