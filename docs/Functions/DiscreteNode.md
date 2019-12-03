# class DiscreteNode

## void InitializeCPT()

## void AddInstanceOfVarVal(DiscreteConfig instance_of_var_val)

## void AddCount(int query_val, DiscreteConfig &parents_config, int count)

## double GetProbability(int query_val, DiscreteConfig &parents_config)

Return the conditional probability of query value given parents configuration.

For example, if you want to get the value of P(A|B), you can call the function
`GetProbability(A, B)`.

If the query value or the parents configuration contains the value that has not
been seen in the training stage, the function will return the smallest probability
value divided by the domain size and the number of parents configurations of the
node.
