# class Network

## void PrintNetworkStruct();
Print the network structure. Each line corresponds to a node and its parents.

Example (pseudocode)
```
Network structure as

  ----------------------|
  |                     V
+---+      +---+      +---+
| A | ---> | B | ---> | C |
+---+      +---+      +---+

PrintNetworkStruct()
```
Output
```
A: 
B:  A
C:  A  B
```


## Node\* FindNodePtrByIndex(int index);
Input: node index <br/>
Output: node pointer

## Node\* FindNodePtrByName(string name);
Input: node name <br/>
Output: node pointer

## virtual void StructLearnCompData(Trainer \*trn) = 0;

## void LearnParmsKnowStructCompData(const Trainer \*trn)
Learn parameters of the network, provided with known structure and complete data.

## void SetParentChild(Node \*par, Node \*chi);
Input: two node pointer <br/>
Output: add `chi` to `par` as a child, and add `par` to `chi` as a parent

## void SetParentChild(int, int);
Reloaded version.


## void RemoveParentChild(Node \*par, Node \*chi);
Input: two node pointer <br/>
Output: remove `chi` from `par` as a child, and remove `par` from `chi` as a parent

## void RemoveParentChild(int, int);
Reloaded version.

## vector<int> GenTopoOrd()
Generate the topological order of the network.

Input: none <br/>
Output: a vector<int>, the elements is the indexes of the nodes

## virtual pair<int\*, int> SimplifyDefaultElimOrd(Combination) = 0

## Combination ConstructEvidence(int \*nodes_indexes, int \*observations, int num_of_observations)
Construct evidence (observation).

Input: nodes_indexes: array of indexes of nodes <br/>
       observations: array of observed values of nodes <br/>
       num_of_observations: number of the observed nodes <br/>
Output: an instance of type `Combination`

## vector<Factor> ConstructFactors(int \*Z, int nz, Node \*Y)
Construct a set factors from an array of node pointers.

Input: Z: array of node indexes
       nz: number of elements in Z
       Y: a node pointer
Output: a vector<Factor>, each element is constructed from a node in Z or Y


## void LoadEvidence(vector<Factor> \*factors_list, Combination E, set<int> all_related_vars)
Load the evidence into the factors. <br/>
The factors are constructed from the nodes and are not compatible with the evidence (observation).

## Factor SumProductVarElim(vector<Factor>, int \*, int);
## Factor VarElimInferReturnPossib(int \*elim_ord, int num_elim_ord, Combination evidence, Node \*target);
## Factor VarElimInferReturnPossib(Combination evidence, Node \*target);

## int PredictUseVarElimInfer(int \*Z, int nz, Combination E, int Y_index)
Input: Z: array of node indexes, the order is the elimination order <br/>
       nz: size of Z <br/>
       E: evidence <br/>
       Y_index: the index of the query variable <br/>
Output: the index predicted value of node Y

## int PredictUseVarElimInfer(Combination, int);

## double TestNetReturnAccuracy(Trainer \*);
## double TestNetByApproxInferReturnAccuracy(Trainer \*, int);


## // Probabilistic logic sampling is a method
## // proposed by Max Henrion at 1988.
## Combination ProbLogicSampleNetwork();

## set<int> GetMarkovBlanketIndexesOfNode(Node \*node_ptr);
## int SampleNodeGivenMarkovBlanketReturnValIndex(Node \*node_ptr, Combination markov_blanket);

## vector<Combination> DrawSamplesByProbLogiSamp(int num_samp);
## vector<Combination> DrawSamplesByGibbsSamp(int num_samp, int num_burn_in);

## int ApproxInferByProbLogiRejectSamp(Combination e, Node \*node, vector<Combination> &samples);
## int ApproxInferByProbLogiRejectSamp(Combination e, int node_index, vector<Combination> &samples);


