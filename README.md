# bayesian-network

Developed by Linjian Li, using Clion 2018.3.1

## Feature

  * Structural learning with complete data
    * Chow-Liu tree
  * Exact inference
    * Variable elimination

## Performance

### LIBSVM dataset "a1a"
As a benchmark, 
 * SVM implemented by me using Python achieves accuracy of **0.8286**
 * Logistic regression achieves accuracy of **0.8332**
 * sklearn.naive_bayes.BernoulliNB achieves accuracy of **0.7935**
 
My bayesian network achieved accuracy of **0.8276** using **Chow-Liu Tree** and **variable elimination inference**.

## blah blah

The directory "BayesianNetwork" is the project directory. <br/>
Need to run "a1a.config.generate.py" first to generate the configuration about "a1a".
