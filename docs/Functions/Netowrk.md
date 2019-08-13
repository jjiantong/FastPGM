# class Network

## void PrintEachNodeParents();
Print the network structure. Each line corresponds to a node and its parents.

Example (pseudocode)
```
Network structure as

  ----------------------|
  |                     V
+---+      +---+      +---+
| A | ---> | B | ---> | C |
+---+      +---+      +---+

PrintEachNodeParents()
```
Output
```
A: 
B:  A
C:  A  B
```


## Node\* FindNodePtrByIndex(int index);
Input: node index

Output: node pointer

## Node\* FindNodePtrByName(string name);
Input: node name

Output: node pointer

## virtual void StructLearnCompData(Trainer \*trn) = 0;
A virtual function to do structure learning with complete data.

## void LearnParmsKnowStructCompData(const Trainer \*trn)
Learn parameters of the network, provided with known structure and complete data.

## void SetParentChild(Node \*par, Node \*chi);
Input: two node pointer

Output: add `chi` to `par` as a child, and add `par` to `chi` as a parent

## void SetParentChild(int, int);
Reloaded version.


## void RemoveParentChild(Node \*par, Node \*chi);
Input: two node pointer

Output: remove `chi` from `par` as a child, and remove `par` from `chi` as a parent

## void RemoveParentChild(int, int);
Reloaded version.

## vector<int> GenTopoOrd()
Generate the topological order of the network.

Input: none

Output: a vector<int>, the elements is the indexes of the nodes

## virtual pair<int\*, int> SimplifyDefaultElimOrd(Combination) = 0
The performance of variable elimination relies heavily on the elimination ordering. A good elimination ordering will greatly accelerate the performance.

## Combination ConstructEvidence(int \*nodes_indexes, int \*observations, int num_of_observations)
Construct evidence (observation).

Input: nodes_indexes: array of indexes of nodes <br/>
       observations: array of observed values of nodes <br/>
       num_of_observations: number of the observed nodes

Output: an instance of type `Combination`

## vector<Factor> ConstructFactors(int \*Z, int nz, Node \*Y)
Construct a set factors from an array of node pointers.

Input: Z: array of node indexes <br/>
       nz: number of elements in Z <br/>
       Y: a node pointer

Output: a vector<Factor>, each element is constructed from a node in Z or Y


## void LoadEvidence(vector<Factor> \*factors_list, Combination E, set<int> all_related_vars)
Load the evidence into the factors. The factors are constructed from the nodes and are not compatible with the evidence (observation).

## Factor SumProductVarElim(vector<Factor> factors_list, int \*Z, int nz)
Input: a list of `Factor`s <br/>
       an array of variable elimination ordering
       an integer indicating number of elements in variable elimination ordering

Output: a `Factor` after eliminating all variables in the elimination ordering


## Factor VarElimInferReturnPossib(int \*elim_ord, int num_elim_ord, Combination evidence, Node \*target)
Input: elim_ord: elimination ordering
       num_elim_ord: number of elements in variable elimination ordering
       evidence: observed evidence on variables
       target: the target query variable

Output: a `Factor` containing the probabilities of the target query variable


## Factor VarElimInferReturnPossib(Combination evidence, Node \*target)
A reloaded version of not specify the elimination ordering.


## int PredictUseVarElimInfer(int \*Z, int nz, Combination E, int Y_index)
Input: Z: array of node indexes, the order is the elimination order <br/>
       nz: size of Z <br/>
       E: evidence <br/>
       Y_index: the index of the query variable

Output: the index of predicted value of node Y

## int PredictUseVarElimInfer(Combination E, int Y_index)
A reloaded version of not specify the elimination ordering.

Input: E: evidence <br/>
       Y_index: the index of the query variable

Output: the index of predicted value of node Y



## double TestNetReturnAccuracy(Trainer \*tester)
Using exact inference.

Input: tester: an instance of `Trainer` containing the test dataset

Output: Accuracy on test set

## double TestNetByApproxInferReturnAccuracy(Trainer \*tester, int num_samp)
Using approximate inference.

Input: tester: an instance of `Trainer` containing the test dataset
       num_samp: number of samples generated to do approximate inference

Output: Accuracy on test set


## Combination ProbLogicSampleNetwork()
Probabilistic logic sampling is a method proposed by Max Henrion in 1988. Draw a sample from the network model, starting from the root node and following the topological order.

Input: none

Output: a sample drawn from the network

## set<int> GetMarkovBlanketIndexesOfNode(Node \*node_ptr)
Get the indexes of the Markov blanket of the specified node.

Input: node_ptr: the pointer to the target node

Output: a set of integers which are the Markov blanket of the target node

## int SampleNodeGivenMarkovBlanketReturnValIndex(Node \*node_ptr, Combination markov_blanket)
Draw a sample from a node given the evidence of its Markov blanket.

Input: node_ptr: the target node
       markov_blanket: the evidence on the target node's Markov blanket

Output: the index of a possible value of the node

## vector<Combination> DrawSamplesByProbLogiSamp(int num_samp)
Draw many samples from the network by means of probabilistic logic sampling.

Input: num_samp: the number of samples needed

Output: a vector containing the drawn samples

## vector<Combination> DrawSamplesByGibbsSamp(int num_samp, int num_burn_in)
Draw many samples from the network by means of Gibbs sampling.

Input: num_samp: the number of samples needed
       num_burn_in: the number of samples in burn-in period

Output: a vector containing the drawn samples

## int ApproxInferByProbLogiRejectSamp(Combination e, Node \*node, vector<Combination> &samples)
Approximate inference by probabilistic logic sampling sampling.

Input: e: observed evidence
       node: the pointer to the node
       samples: the samples drawn by probabilistic logic sampling

Output: Output: the index of predicted value of node

## int ApproxInferByProbLogiRejectSamp(Combination e, int node_index, vector<Combination> &samples)
A reloaded version.

