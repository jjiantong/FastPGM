# bayesian-network

Developed by Linjian Li, using Clion 2018.3.1

## Feature

  * Structural learning with complete data
    * Chow-Liu tree
  * Exact inference
    * Variable elimination

### supported problems
classification with discrete feature values

### to do: 
 * obtain an optimal sequence for variable elimination
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
Boost C++ Libraries. It is used to parse the string from dataset. Users need to download the librayies and modify the path to the libraries in `CMakeLists.txt`. 

## Performance

### LIBSVM dataset "a1a"
As a benchmark, 
 * SVM implemented by me using Python achieves accuracy of **0.8286**
 * Logistic regression achieves accuracy of **0.8332**
 * sklearn.naive_bayes.BernoulliNB achieves accuracy of **0.7935**
 
My bayesian network achieved accuracy of **0.8276** using **Chow-Liu Tree** and **variable elimination inference**.

## blah blah
Consumed time and accuracy for the whose process on "a1a" (load data -> train -> test)
* Using elimination order specified for "a1a", it can be done within **30 seconds**, and achieve accuracy of **0.8332**
* Using general elimination order, it takes **11 minutes** and and achieve accuracy of **0.762792**
