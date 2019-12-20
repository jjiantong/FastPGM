# class JunctionTree

## Moralize(int **direc_adjac_matrix, int &num_nodes)

For each node, link every pair of parents of it.

Time complexity: O(n^3), where n is the number of nodes.

## GenMapElimVarToClique()

Generate a map whose keys are elimination variables and the values are the cliques.

## MinNeighbourElimOrd(int **adjac_matrix, int &num_nodes)

Determine the elimination ordering by the rule of minimum number of neighbours.

## Triangulate(Network \*net, int \*\*adjac_matrix, int &num_nodes, vector\<int\> elim_ord, set<Clique*> &cliques)

A recursive function to triangulate the moralized un-directed graph with the given elimination ordering.

1. select a node at the front of elimination ordering
2. find all neighbours of the node
3. form a clique (complete graph) using the node and all its neighbours
4. delete the node from the elimination ordering and cut all edges connecting to it in the graph
5. call `Triangulate` on the remaining graph, nodes, and elimination ordering (recursion)

## ElimRedundantCliques()

Some cliques can be regarded as subsets of some other cliques. These cliques can be eliminated.

## FormListShapeJunctionTree(set<Clique*> &cliques)

Form a junction tree which has the shape of a linked-list. This shape makes implementation for junction tree on conditional Gaussian Bayesian network easier. The formed junction tree can be split into two part by one cut. One is the discrete part and the other is the continuous part.

## FormJunctionTree(set<Clique*> &cliques)

Form a junction tree by Prim's algorithm. The weights of edges is represented by the weights of the separators.

## NumberTheCliquesAndSeparators()

Give each clique and each separator an ID.

## AssignPotentials()

The potentials in the cliques of the junction tree are initially being 1 or empty. We then need to assign the probability distribution of the network to the junction tree.

## LoadDiscreteEvidence(const DiscreteConfig &E)

Given the obseration on discrete variables, set the uncompatible entries of the cliques in the junction tree to be zero.

## MessagePassingUpdateJT()

Call `Collect` and `Distribute` to update the junction tree such that the potentials can correctly represent part of the distribution of the original Bayesian network.

## PrintAllCliquesPotentials()

As the name suggests.

## PrintAllSeparatorsPotentials()

As the name suggests.

## BeliefPropagationCalcuDiscreteVarMarginal(int query_index)

Calculate and return the marginal distribution of a discrete variable.

## InferenceUsingBeliefPropagation(int &query_index)

Call `BeliefPropagationCalcuDiscreteVarMarginal` and find the argmax of the distribution.

## EvaluateVarElimAccuracy(int class_var, Dataset *dts)

For each testing instance in the test set, load the evidence into the junction tree, update it by belief propagation (a.k.a. message passing), calculate marginal distribution of the class variable, and make the prediction.

Return the accuracy on the test set.
