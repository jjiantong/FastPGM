# class Clique

## Clique(set<Node\*> set_node_ptrs)
Generate a clique from a set of nodes pointers. It will call the function `InitializeClique`.

## void InitializeClique(set<Node\*> set_node_ptrs)
Initialize the parameters in the caller clique with a set of nodes pointers.

## Factor Collect()
A process of junction tree algorithm. Caller collects the information from
the downstream cliques by recursively call the function `Collect` on downstream cliques,
then update the parameters of itself using the collected information.
After collecting from all downstream cliques and updating, it will return a "message" for its upperstream clique.

## void Distribute()
A process of junction tree algorithm. Caller is the selected "root" of the junction tree.
It distribute the information that it knows to all downstream cliques recursively.

## void Distribute(Factor f)
A process of junction tree algorithm. The caller receives a "message" (the argument `f`)
from the upstream clique, and uses this message to update itself. Then constructs a new message
and makes recursive calls of `Distribute(Factor)` on all downstream cliques to pass the new message.

## Factor SumOutExternalVars(Factor f)
This function will be called by the function `MultiplyWithFactorSumOverExternalVars`
to sum out the variables in the parameters that are not related to the clique itself.

## void MultiplyWithFactorSumOverExternalVars(Factor f)
This function will be called by the function `UpdateUseMessage`.
Clique updates itself with the message by means of multiply the message with itself.
Then sum out the variables that are not related to itself.

## void UpdateUseMessage(Factor f)
Caller receives a "message" containing information from upstream or downstream cliques.
Then update the parameters of itself with this message.

## Factor ConstructMessage()
Caller constructs a "message" with the information it knows.
This message will be used by upstream or downstream cliques
in the process of "collect" and "distribute" in junction tree algorithm.

## void PrintPotentials()
Print the parameters of this clique.
