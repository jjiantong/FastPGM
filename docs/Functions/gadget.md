# gadget.h

## set<DiscreteConfig> GenAllCombFromSets(set<DiscreteConfig> \*)
A recursive function generate all possible combinations from several sets. <br/>

Input: several sets (set of sets) <br/>
Output: all possible combinations of elements of each set, by picking one element in each set. <br/>

Example (pseudocode)
```
A = {<a,1>, <a,3>, <a,5>}
B = {<b,100>, <b,101>, <b,102>}
set_of_sets = {A, B}
result = GenAllConfgFromSets(set_of_sets)
print(result)
```
Output:
```
{ {<a,1>, <b,100>},
  {<a,1>, <b,101>},
  {<a,1>, <b,102>},
  {<a,2>, <b,100>},
  {<a,2>, <b,101>},
  {<a,2>, <b,102>},
  {<a,3>, <b,100>},
  {<a,3>, <b,101>},
  {<a,3>, <b,102>} }
```

## set<DiscreteConfig> ExpandCombFromTwoCombs(set<DiscreteConfig> \*one, set<DiscreteConfig> \*two)
Input: two combinations with different set of value of the elements' first member.
Output: one combination

Example
```
X = { {<a,1>, <b,1>},
      {<a,1>, <b,2>} }
Y = { {<c,1>, <d,1>},
      {<c,2>, <d,1>} }
result = ExpandConfgFromTwoConfgs(X,Y)
print(result)
```
Output
```
{ {<a,1>, <b,1>, <c,1>, <d,1>},
  {<a,1>, <b,1>, <c,2>, <d,1>},
  {<a,1>, <b,2>, <c,1>, <d,1>},
  {<a,1>, <b,2>, <c,2>, <d,1>} }
```

## bool FirstIsSubsetOfSecond(DiscreteConfig \*first, DiscreteConfig \*second)
A function that is used when multiplying two factors. <br/>
If each element in the first DiscreteConfig is in the second DiscreteConfig, return true. <br/>

Input: two `DiscreteConfig`s <br/>
Output: bool <br/>

Example (pseudocode)
```
fir_1 = { <a,1> , <b,2> }
sec_1 = { <a,1> , <b,2> , <c,3> , whatever more... }

fir_2 = { <a,0> , <b,2> }
sec_2 = { <a,1> , <b,2> , <c,3> , whatever more... }

fir_3 = { <a,1> , <b,2> , <c,3> }
sec_3 = { <a,1> , <b,2> }

print( FirstIsSubsetOfSecond(fir_1, sec_1),
       FirstIsSubsetOfSecond(fir_2, sec_2),
       FirstIsSubsetOfSecond(fir_3, sec_3) )
```
Output
```
true false false
```

## bool FirstCompatibleSecond(DiscreteConfig \*first, DiscreteConfig \*second)
A function that is used when multiplying two factors. <br/>
If elements in the first DiscreteConfig is compatible with the second DiscreteConfig, return true. <br/>

Input: two `DiscreteConfig`s <br/>
Output: bool <br/>

Example (pseudocode)
```
  f1={ <a,1> , <b,2> }
  s1={ <a,1> , <b,2> , <c,3> , whatever more... }

  f2={ <a,0> , <b,2> }
  s2={ <a,1> , <b,2> , <c,3> , whatever more... }

  f3={ <a,1> , <b,2> , <c,3> }
  s3={ <a,1> , <b,2> }

  f4={ <a,1> , <b,2> , <c,3> }
  s4={ <a,1> , <b,2> , <d,4> }

  f5={ <a,1> , <b,2> , <c,3> }
  s5={ <a,1> , <b,2> , <c,4> }

  f6={ <a,1> , <b,2> }
  s6={ <c,3> , <d,4> }

  print( EachFirstIsInSecond(f1, s1),
         EachFirstIsInSecond(f2, s2),
         EachFirstIsInSecond(f3, s3),
         EachFirstIsInSecond(f4, s4),
         EachFirstIsInSecond(f5, s5),
         FirstIsSubsetOfSecond(f6, s6), )

```
Output
```
true false true true false true
```

## bool Conflict(DiscreteConfig \*, DiscreteConfig \*)
Input: two `DiscreteConfig`s

Output: false (if they have different values on the same variable)
        true (else)


## bool OccurInCorrectOrder(int a, int b, vector<int> vec)
If `a` occurs before `b` in `vec`, return true. Otherwise, return false.


## int\* WidthFirstTraversalWithAdjacencyMatrix(int \*\*graph, int num_nodes, int start)
Input: graph: 2-d array representation of the adjacency matrix of the graph
       num_nodes: number of nodes in the graph
       start: the index of the starting node

Output: an array containing the node indexes in the order of width-first traversal

## vector<int> TopoSortOfDAGZeroInDegreeFirst(int \*\*graph, int num_nodes)
Input: graph: 2-d array representation of the adjacency matrix of the graph
       num_nodes: number of nodes in the graph

Output: an array containing the node indexes in the topological sorted order

## string TrimRight(string)
Trim all `\t`, `\n`, `\r` and whitespace characters on the right of a string.

## string TrimLeft(string)
Trim all `\t`, `\n`, `\r` and whitespace characters on the left of a string.

## string Trim(string)
`TrimRight` and `TrimLeft`.


## vector<string> Split(string &s, string delimiter)
Split the string into a `vector` of strings by the delimiter.


## int FactorialForSmallInteger(int)
As the name suggests.

## double LogOfFactorial(int)
The reason to us log is that the factorial value may be huge causing overflow while the log may not.

Input: an non-negative integer

Output: the log of the factorial of the integer

## vector<vector<int>> NaryCount(vector<int> vec_range_each_digit)
N-ary count given the range of each digit. The left-most digit is the most significant digit. The domain of each digit start at 0, so the max value of this digit is one smaller than the range.

Example (pseudocode)
```
digit_210 = [2, 2, 3]
counts = NaryCount(digit_210)
print(counts)
```
Output:
```
[
  [0, 0, 0],
  [0, 0, 1],
  [0, 0, 2],
  [0, 1, 0],
  [0, 1, 1],
  [0, 1, 2],
  [1, 0, 0],
  [1, 0, 1],
  [1, 0, 2],
  [1, 1, 0],
  [1, 1, 1],
  [1, 1, 2]
]
```