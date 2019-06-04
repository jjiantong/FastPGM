# gadget.h

## set<Combination> GenAllCombFromSets(set<Combination> \*)
A recursive function generate all possible combinations from several sets. <br/>

Input: several sets (set of sets) <br/>
Output: all possible combinations of elements of each set, by picking one element in each set. <br/>

Example (pseudocode)
```
A = {1, 3, 5}
B = {a, b, c}
set_of_sets = {A, B}
result = GenAllCombFromSets(set_of_sets)
print(result)
```
Output:
```
{1a, 1b, 1c, 3a, 3b, 3c, 5a, 5b, 5c}
```

## bool EachFirstIsInSecond(Combination \*first, Combination \*second)
A function that is used when multipling two factors. <br/>
If each element in the first Combination is in the second Combination, return true. <br/>

Input: two `Combination`s <br/>
Output: bool <br/>

Example (pseudocode)
```
fir_1 = { <a,1> , <b,2> }
sec_1 = { <a,1> , <b,2> , <c,3> , whatever more... }

fir_2 = { <a,0> , <b,2> }
sec_2 = { <a,1> , <b,2> , <c,3> , whatever more... }

fir_3 = { <a,1> , <b,2> , <c,3> }
sec_3 = { <a,1> , <b,2> }

print( EachFirstIsInSecond(fir_1, sec_1),
       EachFirstIsInSecond(fir_2, sec_2),
       EachFirstIsInSecond(fir_3, sec_3) )
```
Output
```
true false false
```

## bool FirstCompatibleSecond(Combination \*first, Combination \*second)
A function that is used when multipling two factors. <br/>
If elements in the first Combination is compatible with the second Combination, return true. <br/>

Input: two `Combination`s <br/>
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
         EachFirstIsInSecond(f6, s6), )

```
Output
```
true false true true false true
```

## bool Conflict(Combination \*, Combination \*)
Input: two `Combination`s

Output: false (if they have different values on the same variable)
        true (else)


## int\* WidthFirstTraversalWithAdjacencyMatrix(int \*\*graph, int num_nodes, int start)
Input: graph:2-d array representation of the adjacency matrix of the graph
       num_nodes: number of nodes in the graph
       start: the index of the starting node

Output: an array containing the node indexes in the order of width-first traversal

## vector<int> TopoSortOfDAGZeroInDegreeFirst(int \*\*graph, int num_nodes)
Input: graph:2-d array representation of the adjacency matrix of the graph
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