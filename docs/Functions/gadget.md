# gadget.h

## set<set<T>> GenAllCombinationsFromSets(set<set<T>> \*)
Given several sets, generate all possible combinations such that for each combination, all elements come from different sets. That is, drawing one element from each set to get a combination.

The function uses recursion to generate all possible combinations. 
1. select one set. 
2. generate all combinations on the remaining sets (where the recursion appears). 
3. for each element in the set seleted at the step 1, make combinations with every combination generated in the second step.

Time complexity: Suppose there are *m* sets and all sets have the same cardinality of *n*. 
```
T(m) = n * T(m-1) + n * ((m-1)*n)
```
where the first term is for the recursion of the step 2 described above and the second term is for the step 3. 
```
(LaTeX)
$T(m) = \sum_{i=1}^{m-1} (m-i) n^{i+1}$
```

Input: several sets (set of sets)

Output: all possible combinations of elements of each set, by picking one element in each set.

Example (pseudocode)
```
A = {<a,1>, <a,3>, <a,5>}
B = {<b,100>, <b,101>, <b,102>}
set_of_sets = {A, B}
result = GenAllCombinationsFromSets(set_of_sets)
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

Time complexity: most of the cost comes from calling `GenAllCombinationsFromSets` while the other cost is negligible.

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
A function that is used when multiplying two factors. 

If each element in the first DiscreteConfig is in the second DiscreteConfig, return true. 


Input: two `DiscreteConfig`s 

Output: bool 


Time complexity: O( |first| \* |second| )


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
A function that is used when multiplying two factors. 

If elements in the first DiscreteConfig is compatible with the second DiscreteConfig, return true. 


Input: two `DiscreteConfig`s 

Output: bool 


Time complexity: O( |first| \* |second| )


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

## bool Conflict(DiscreteConfig \*cfg1, DiscreteConfig \*cfg2)

Input: two `DiscreteConfig`s

Output: false (if they have different values on the same variable)
        true (else)

Time complexity: O( |cfg1| \* |cfg2| )


## bool OccurInCorrectOrder(int a, int b, vector\<int\> vec)

If `a` occurs before `b` in `vec`, return true. Otherwise, return false.

Time complexity: O( |vec| )


## bool DAGObeyOrdering(int \*\*graph, int num_nodes, vector\<int\> ord)

If `ord` is one of the topological orderings of the `graph`, return `true`.
Else, return `false`.

Time complexity: O( n^2 )


## int\* WidthFirstTraversalWithAdjacencyMatrix(int \*\*graph, int num_nodes, int start)

Input: graph: 2-d array representation of the adjacency matrix of the graph
       num_nodes: number of nodes in the graph
       start: the index of the starting node

Output: an array containing the node indexes in the order of width-first traversal

Time complexity: O( n^2 )

## vector\<int\> TopoSortOfDAGZeroInDegreeFirst(int \*\*graph, int num_nodes)

Input: graph: 2-d array representation of the adjacency matrix of the graph
       num_nodes: number of nodes in the graph

Output: an array containing the node indexes in the topological sorted order

Time complexity: O( n^2 )

## string TrimRight(string)

Trim all `\t`, `\n`, `\r` and whitespace characters on the right of a string.

## string TrimLeft(string)

Trim all `\t`, `\n`, `\r` and whitespace characters on the left of a string.

## string Trim(string)

`TrimRight` and `TrimLeft`.


## vector<string> Split(string &s, string delimiter)

Split the string into a `vector` of strings by the delimiter.


## int FactorialForSmallInteger(int)

As the name suggests. Use loop to avoid recursion.

Time complexity: O( n )

## double LogOfFactorial(int)

The reason to us log is that the factorial value may be huge causing overflow while the log may not.

Input: an non-negative integer

Output: the log of the factorial of the integer

Time complexity: O( n )

## vector<vector\<int\>> NaryCount(vector\<int\> vec_range_each_digit)

N-ary count given the range of each digit. The left-most digit is the most significant digit. The domain of each digit start at 0, so the max value of this digit is one smaller than the range.

Time complexity: O( multiply the elements in the vector together )

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