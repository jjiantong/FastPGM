# Notes of Reading Weka Source Code

## Use N-ary Counts to Represent Instantiation of a Group of Variable

May be good for parallelize.

## Smothing on Parameter Learning

Greatly improves the performance. I have done experiments on multiple datasets to varify that.

## Store Counts of Data Occurence Instead of Storing Probability Directly

Store the counts for each value. When the model needs the probability, it can just divide the count by the sum of counts.

An advantage is that the model can update the parameter when there are more data coming in. It can just add the corresponding counts then the model is updated. If the model stores the probability directly, it will be hard to update the parameter.

## Calculate the Probability in Log Space

Calculate the probability in log space and change to normal space at last.

Two advantages:

1. Probability multiplying becomes log-probability adding.
2. When many numbers between 0 and 1 multiply together, the result may be too small causing underflow. Change them to log space and calculating the sum will preserve better accuracy.
