# bayesian-network

Developed by Linjian Li, using Clion 2018.3.1

## Important notes


## Feature
  * Structural learning with complete data
    * Chow-Liu tree
  * Exact inference
    * Variable elimination
      * Optimal elimination order for **tree shape** network
  * Construct custom network from files
    * Format of file's contents
      * The first line is an integer of the number of nodes
      * Each line from the second till the end represents a node and its child(ren) separated by white space
        * node_index -> child_1 child_2 ...

### supported problems
classification with discrete feature values

### to do:
 * classification with features of continues values
 * explore regression
 * support graph structures beyond  trees

## How to use
Create a directory called ```bin``` under the root directory. In Linux, you can use the following command.

```bash
mkdir bin
```

Then, go into ```bin``` directory and issue ```cmake ..``` to generate the make file.

## Dependencies
Boost C++ Libraries. It is used to parse the string from dataset. Users need to download the libraries and modify the path to the libraries in `CMakeLists.txt`.

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
 * 3 minutes, accuracy of **0.826281**

### LIBSVM dataset "w1a"
 * 26 minutes, accuracy of **0.972436**

## blah blah