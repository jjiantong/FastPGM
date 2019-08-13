# class Node

## void GenParCombs()
Generate parents combinations. <br/>

Example (pseudocode) 
```
Node A, B, C
A.Domain = {0, 1}
B.Domain = {8, 9}
C.AddParent(A)
C.AddParent(B)
C.GenParCombs()
print(C.parents_combinations)
```
Output
```
{ {<A,0>, <B,8>},
  {<A,1>, <B,8>},
  {<A,0>, <B,9>},
  {<A,1>, <B,9>} }
```

## int GetNodeIndex()
Return the index of the node.

## void SetNodeIndex(int index)
Set the node index.

## void AddChild(Node \*node_ptr)
Add a child to this node.

Input: pointer to the child node

## void AddParent(Node \*node_ptr)
Add a parent to this node.

Input: pointer to the parent node

## void RemoveChild(Node \*node_ptr)
Remove a child to this node.

Input: pointer to the child node

## void RemoveParent(Node \*node_ptr)
Remove a parent to this node.

Input: pointer to the parent node