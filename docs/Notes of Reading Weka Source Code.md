# Notes of Reading Weka Source Code

## Weka is super fast at inference

I found that when Weka calculates the class membership probabilities for the
given test instance, it **only works if the given instance is complete**.
Weka simply multiply the conditional probability for each variable given the
value of its parents and then return the probability distribution of the class
variable. We can call it brute force method. (In fact, Weka uses addition in
log-space instead of multiplication in normal-space and then transform from
log-space to normal-space at last.)

I did some experiments using brute force method on `a1a` dataset, the running
time is 65x faster than variable elimination algorithm.

## Use N-ary Counts to Represent Instantiation of a Group of Variable

May be good for parallelize.

## Smoothing on Parameter Learning

Greatly improves the performance. I have done experiments on multiple datasets to verify that.

## Store Counts of Data Occurrence Instead of Storing Probability Directly

Store the counts for each value. When the model needs the probability,
it can just divide the count by the sum of counts.

An advantage is that the model can update the parameter when there are more
data coming in. It can just add the corresponding counts then the model is
updated. If the model stores the probability directly, it will be hard to
update the parameter.

## Calculate the Probability in Log Space

Calculate the probability in log space and change to normal space at last.

Two advantages:

1. Probability multiplying becomes log-probability adding.
2. When many numbers between 0 and 1 multiply together, the result may be too
small causing underflow. Change them to log space and calculating the sum will
preserve better accuracy.

## local K2 algorithm

The algorithm check the variables preceding the target variable one by one. If
adding a variable to the parent set of the target variable makes the score better,
then the variable will be added, or else will not.

The algorithm does not check **every** possible combination of the preceding variables.
