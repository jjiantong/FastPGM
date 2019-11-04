# class Factor

## SetMembers(set<int> rv, set<DiscreteConfig> sc, map<DiscreteConfig, double> mp)
As the name suggests.

## CopyFactor(Factor F)
As the name suggests.

## Factor(DiscreteNode \*node)
Constructor

## ConstructFactor(DiscreteNode \*node)
The actural contructing function.

Time complexity: O( domain cardinality of the node times the number of configuration of its parents )

## MultiplyWithFactor(Factor second_factor)
Mltiply the caller with the callee and return a new factor. The caller and callee are un-changed.

Time complexity: O( number of rows of the caller times number of rows of the callee )

## SumOverVar(int index)
Sum out the variable of given index in the factor.

Time complexity: O( n_r * log(n_r) ), where n_r is the number of the rows

## Normalize()
Normalizing such that the sum of all entries equals to 1.

Time complexity: O( n_r ), where n_r is the number of the rows

## PrintPotentials()
As the name suggests.