# class CustomNetwork

## void StructLearnCompData(Dataset \*)

Empty function. Because a custom network's structure should be specified by user with a file. There is no need to learn the structure.

## pair<int\*, int> SimplifyDefaultElimOrd(DiscreteConfig evidence)

Not implemented yet.

## void GetNetFromXMLBIFFile(string file_path)

As the name suggests. First, get a `vector` of pointers of connnected nodes from the XMLBIF file. Second, insert the node pointers in the `vector` into the `set_node_ptr_container`.

Input: the path to the XMLBIF file

Output: none (but a network has been generated)