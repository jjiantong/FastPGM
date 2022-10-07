#pragma clang diagnostic push
#pragma ide diagnostic ignored "openmp-use-default-none"

#include "Network.h"
#include "DiscreteNode.h"
#include "ContinuousNode.h"

Network::Network(): Network(true) {}

Network::Network(bool pure_disc) {
  this->pure_discrete = pure_disc;
}

Network::Network(vector<Node*> nodes, string name) {
    this->network_name = name;
    this->num_nodes = nodes.size();
    this->pure_discrete = true;
    for(auto &n: nodes) {
        if(!n->is_discrete) {
            this->pure_discrete = false;
            break;
        }
    }

    for (auto &n: nodes) {
        int index = n->GetNodeIndex();
        map_idx_node_ptr[index] = n;
    }
}

Network::Network(Network &net) {
  //need to test correctness

  network_name = net.network_name;
  num_nodes = net.num_nodes;
  pure_discrete = net.pure_discrete;
  vec_default_elim_ord = net.vec_default_elim_ord;
  topo_ord = net.GetTopoOrd();//this sequence/order is used for variable elimination.

  //copy the map for mapping id to node ptr
  map_idx_node_ptr = net.map_idx_node_ptr;
  for (int i = 0; i < num_nodes; ++i) {
    auto old_node = map_idx_node_ptr.at(i);
    Node *n = nullptr;
    if (old_node->is_discrete) {
      auto disc_old_node = (DiscreteNode*)old_node;
      n = new DiscreteNode(*disc_old_node);
    } else {
      auto cont_old_node = (ContinuousNode*)old_node;
      n = new ContinuousNode(*cont_old_node);
    }
    map_idx_node_ptr[i] = n;
  }
}

Network::~Network() {
    for (int i = 0; i < num_nodes; ++i) {
        SAFE_DELETE(map_idx_node_ptr[i]);
//        delete map_idx_node_ptr[i];
//        map_idx_node_ptr[i] = nullptr;
    }
}

//TODO: double-check correctness
/*!
 * @brief: print the parents of all the nodes.
 * each line corresponds to a node and its parents.
 */
void Network::PrintEachNodeParents() {
  for (const auto &id_node_ptr : map_idx_node_ptr) { // for each <int, Node*> pair
    auto node_ptr = id_node_ptr.second; // get the Node*
    cout << node_ptr->node_name << ":\t";
    for (const auto &par_node_ptr : GetParentPtrsOfNode(node_ptr->GetNodeIndex())) {
      cout << par_node_ptr->node_name << '\t';
    }
    cout << endl;
  }
}

//TODO: double-check correctness
void Network::PrintEachNodeChildren() {//print the child nodes of all the nodes
  for (const auto &id_node_ptr : map_idx_node_ptr) {
    auto node_ptr = id_node_ptr.second;
    cout << node_ptr->node_name << ":\t";
    for (const auto &chi_node_ptr : GetChildrenPtrsOfNode(node_ptr->GetNodeIndex())) {
      cout << chi_node_ptr->node_name << '\t';
    }
    cout << endl;
  }
}

/**
 * @brief: find node ptr by id
 */
Node* Network::FindNodePtrByIndex(const int &index) const {
    if (index < 0 || index >= num_nodes) {  // The node indexes are consecutive integers start at 0.
        fprintf(stderr, "Error in function %s! \nInvalid index [%d]!", __FUNCTION__, index);
        exit(1);
    }
    return map_idx_node_ptr.at(index);
}

/**
 * @brief: find node ptr by name
 */
Node* Network::FindNodePtrByName(const string &name) const {
  Node* node_ptr = nullptr;
  for (const auto &i_n_ptr : map_idx_node_ptr) {
    auto n_ptr = i_n_ptr.second;
    if (n_ptr->node_name==name) {
      node_ptr = n_ptr;
      break;
    }
  }
  return node_ptr;
}

/**
 * @brief: add a node to the map
 */
void Network::AddNode(Node *node_ptr) {
  map_idx_node_ptr[node_ptr->GetNodeIndex()] = node_ptr;
  ++num_nodes;
}

/**
 * @brief: remove a node based on id; not used yet.
 */
void Network::RemoveNode(int node_index) {
  map_idx_node_ptr.erase(node_index);
  --num_nodes;
}

/**
 * @brief: check whether the node belongs to the network
 */
bool Network::NodeIsInNetwork(Node *node_ptr) {
    return NodeIsInNetwork(node_ptr->GetNodeIndex());
}

bool Network::NodeIsInNetwork(int node_idx) {
    if (map_idx_node_ptr.find(node_idx) == map_idx_node_ptr.end()) {
        return false;
    } else {
        return true;
    }
}

int Network::GetUndirectedEdge(Node *node1, Node *node2) {
    Edge edge(node1, node2);
    vector<Edge>::iterator it = find(vec_edges.begin(), vec_edges.end(), edge);
    if (it == vec_edges.end()) {
        // vec_edges does not contain edge
        return -1;
    } else {
        return distance(vec_edges.begin(), it);
    }
}

int Network::GetDirectedEdge(Node *node1, Node *node2) {
    Edge edge(node1, node2, TAIL, ARROW);
    vector<Edge>::iterator it = find(vec_edges.begin(), vec_edges.end(), edge);
    if (it == vec_edges.end()) {
        // vec_edges does not contain edge
        return -1;
    } else {
        return distance(vec_edges.begin(), it);
    }
}

int Network::GetDirectedEdgeFromEdgeOrder(Node *node1, Node *node2) {
    Edge edge(node1, node2, TAIL, ARROW);
    vector<Edge>::iterator it = find(edge_order.begin(), edge_order.end(), edge);
    if (it == edge_order.end()) {
        return -1;
    } else {
        return distance(edge_order.begin(), it);
    }
}

void Network::PrintEachEdgeWithIndex() {
    for (int i = 0; i < num_edges; ++i) {
        cout << i << ". ";
        Edge edge = vec_edges.at(i);
        if (!edge.IsDirected()) {
            cout << edge.GetNode1()->GetNodeIndex() << " -- " << edge.GetNode2()->GetNodeIndex() << endl;
        } else if (edge.GetEndPoint1() == TAIL){
            cout << edge.GetNode1()->GetNodeIndex() << " -> " << edge.GetNode2()->GetNodeIndex() << endl;
        } else {
            cout << edge.GetNode2()->GetNodeIndex() << " -> " << edge.GetNode1()->GetNodeIndex() << endl;
        }
    }
    cout << "num nodes = " << num_nodes << endl;
    cout << "num edges = " << num_edges << endl;
}

void Network::PrintEachEdgeWithName() {
    for (int i = 0; i < num_edges; ++i) {
        cout << i << ". ";
        Edge edge = vec_edges.at(i);
        if (!edge.IsDirected()) {
            cout << edge.GetNode1()->node_name << " -- " << edge.GetNode2()->node_name << endl;
        } else if (edge.GetEndPoint1() == TAIL){
            cout << edge.GetNode1()->node_name << " -> " << edge.GetNode2()->node_name << endl;
        } else {
            cout << edge.GetNode2()->node_name << " -> " << edge.GetNode1()->node_name << endl;
        }
    }
    cout << "num nodes = " << num_nodes << endl;
    cout << "num edges = " << num_edges << endl;
}

int Network::GetEdge(Node *node1, Node *node2) {
    Edge edge;
    int position;
    if ((position = GetUndirectedEdge(node1, node2)) == -1 &&
        (position = GetDirectedEdge(node1, node2)) == -1 &&
        (position = GetDirectedEdge(node2, node1)) == -1) {
        return -1;
    } else {
        return position;
    }
}

/**
 * @brief: add an edge/arc to the network
 * @return true if not form a circle; false if form a circle (also delete the added arc)
 */
bool Network::AddDirectedEdge(int p_index, int c_index) {
    // first check the two nodes
//    if(!(NodeIsInNetwork(p_index) && NodeIsInNetwork(c_index))) {
//        fprintf(stderr, "Error in function [%s].\nNode [%d] and/or [%d] do not belong to this network!",
//                __FUNCTION__, p_index, c_index);
//        exit(1);
//    }

    Node* node1 = FindNodePtrByIndex(p_index);
    Node* node2 = FindNodePtrByIndex(c_index);
    SetParentChild(node1, node2); // set parent and child relationship

    Edge edge(node1, node2, TAIL, ARROW);
    vec_edges.push_back(edge);
//    ++num_edges;

//    if (GetDirectedEdge(node1, node2) == -1) { //TODO
//        // vec_edges does not contain edge: add edge
//        Edge edge(node1, node2, TAIL, ARROW);
//        vec_edges.push_back(edge);
//        ++num_edges;
//    }

    bool contain_circle = ContainCircle();
    // the edge/arc shouldn't be added, because it leads to loops in the network.
    if (contain_circle) {
        DeleteDirectedEdge(p_index, c_index);
    }
    return !contain_circle;
}

bool Network::DeleteDirectedEdge(int p_index, int c_index) {
    // first check the two nodes
//    if(!(NodeIsInNetwork(p_index) && NodeIsInNetwork(c_index))) {
//        fprintf(stderr, "Error in function [%s].\nNode [%d] and/or [%d] do not belong to this network!",
//                __FUNCTION__, p_index, c_index);
//        exit(1);
//    }

    Node* node1 = FindNodePtrByIndex(p_index);
    Node* node2 = FindNodePtrByIndex(c_index);

    int pos = GetDirectedEdge(node1, node2);
    if (pos == -1) {
        return false;
    } else {
        RemoveParentChild(node1, node2);
        vec_edges.erase(vec_edges.begin() + pos);
//        --num_edges;
        return true;
    }
}

/**
 * @brief: swap a parent and child relationship
 * @return true if not form a circle; false if form a circle (also delete the added arc)
 */
bool Network::ReverseDirectedEdge(int p_index, int c_index) {
  DeleteDirectedEdge(p_index, c_index);
  return AddDirectedEdge(c_index, p_index);
}

/**
 * @brief: add an undirected edge/arc to the network
 * have an order
 */
void Network::AddUndirectedEdge(int p_index, int c_index) {
    // first check the two nodes
//    if(!(NodeIsInNetwork(p_index) && NodeIsInNetwork(c_index))) {
//        fprintf(stderr, "Error in function [%s].\nNode [%d] and/or [%d] do not belong to this network!",
//                __FUNCTION__, p_index, c_index);
//        exit(1);
//    }

    Node* node1 = FindNodePtrByIndex(p_index);
    Node* node2 = FindNodePtrByIndex(c_index);

    Edge edge(node1, node2);
    vec_edges.push_back(edge);
//    ++num_edges; // TODO

//    if (GetUndirectedEdge(node1, node2) == -1) {
//        // vec_edges does not contain edge: add edge
//        Edge edge(node1, node2);
//        vec_edges.push_back(edge);
//        ++num_edges;
//    }
}

bool Network::DeleteUndirectedEdge(int p_index, int c_index) {
    // first check the two nodes
//    if(!(NodeIsInNetwork(p_index) && NodeIsInNetwork(c_index))) {
//        fprintf(stderr, "Error in function [%s].\nNode [%d] and/or [%d] do not belong to this network!",
//                __FUNCTION__, p_index, c_index);
//        exit(1);
//    }

    // then check the order
    if (p_index > c_index) {
        int tmp = p_index;
        p_index = c_index;
        c_index = tmp;
    }

    Node* node1 = FindNodePtrByIndex(p_index);
    Node* node2 = FindNodePtrByIndex(c_index);

    int pos = GetUndirectedEdge(node1, node2);
    if (pos == -1) {
        return false;
    } else {
        vec_edges.erase(vec_edges.begin() + pos);
//        --num_edges;
        return true;
    }
}

void Network::GenerateUndirectedCompleteGraph() {
    // |E| = n(n-1)/2
    num_edges = (num_nodes - 1) * num_nodes / 2;
    vec_edges.resize(num_edges);

    for (int i = 0; i < num_nodes; ++i) {
        for (int j = i + 1; j < num_nodes; ++j) {
            // compute the edge index in "vec_edges" according to the node indexes i and j
            int index = (2 * num_nodes - i - 1) * i / 2 + j - i - 1;
            Edge edge(FindNodePtrByIndex(i), FindNodePtrByIndex(j));
            vec_edges[index] = edge;
        }
    }
}

/**
 * @brief: to check whether two nodes are adjacent
 * which also means whether an edge (either directed and undirected) between two nodes exists
 */
bool Network::IsAdjacentTo(int node_idx1, int node_idx2) {
//    set<int> adjacent_nodes = adjacencies[node_idx1];
    set<int> adjacent_nodes;
    for (auto it = adjacencies[node_idx1].begin(); it != adjacencies[node_idx1].end(); ++it) {
        adjacent_nodes.insert((*it).first);
    }
    if (adjacent_nodes.find(node_idx2) == adjacent_nodes.end()) {
        return false;
    } else {
        return true;
    }
}

/**
 * @brief: to check whether an edge from node1->node2 exists
 * the way is to check whether node1 is a parent of node2
 */
bool Network::IsDirectedFromTo(int node_idx1, int node_idx2) {
    Node* node1 = FindNodePtrByIndex(node_idx1);
    Node* node2 = FindNodePtrByIndex(node_idx2);
    return node2->IsParentOfThisNode(node1);
}

/**
 * @brief: to check whether an edge node1--node2 exists
 * the way is to check three conditions:
 *      1) node1 is adjacent to node2 (via IsAdjacentTo)
 *      2) node1 is not a parent of node2 (via IsDirectedFromTo)
 *      3) node2 is not a parent of node1 (via IsDirectedFromTo)
 */
bool Network::IsUndirectedFromTo(int node_idx1, int node_idx2) {
    return (IsAdjacentTo(node_idx1, node_idx2) &&
            !IsDirectedFromTo(node_idx1, node_idx2) &&
            !IsDirectedFromTo(node_idx2, node_idx1));
}

/**
 * @brief: set up the parent and child relationship
 * @param p_index: parent index
 * @param c_index: child index
 */
void Network::SetParentChild(int p_index, int c_index) {
  if (map_idx_node_ptr.find(p_index) == map_idx_node_ptr.end() ||
      map_idx_node_ptr.find(c_index) == map_idx_node_ptr.end()) {
    fprintf(stderr, "Error in function [%s].\nThe nodes [%d] and [%d] do not belong to this network!",
            __FUNCTION__, p_index, c_index);
    exit(1);
  }//end checking whether the nodes belong to the network
  // convert the index format into node ptr format
  Node *p = FindNodePtrByIndex(p_index), *c = FindNodePtrByIndex(c_index);
  SetParentChild(p,c);
}

/**
 * @brief: set parent and child relationship.
 * @param p: parent node ptr
 * @param c: child node ptr
 * add c to p as a child, and add p to c as a parent
 */
void Network::SetParentChild(Node *p, Node *c) {
  p->AddChild(c);
  c->AddParent(p);
}

/**
 * @brief: remove parent child relationship
 * @param p_index: parent index
 * @param c_index: child index
 * remove c to p as a child, and remove p to c as a parent
 */
void Network::RemoveParentChild(int p_index, int c_index) {
  // convert the index format into node ptr format
  Node *p = FindNodePtrByIndex(p_index), *c = FindNodePtrByIndex(c_index);
  RemoveParentChild(p,c);
}

/**
 * @brief: remove parent child relationship using pointers
 * @param p: parent node ptr
 * @param c: child node ptr
 */
void Network::RemoveParentChild(Node *p, Node *c) {
//  if (map_idx_node_ptr.find(p->GetNodeIndex()) == map_idx_node_ptr.end()
//      ||
//      map_idx_node_ptr.find(c->GetNodeIndex())==map_idx_node_ptr.end()) {
//    fprintf(stderr, "Error in function [%s].\nThe nodes [%d] and [%d] do not belong to this network!",
//            __FUNCTION__, p->GetNodeIndex(), c->GetNodeIndex());
//    exit(1);
//  }//end checking whether the nodes belong to the network
  p->RemoveChild(c);
  c->RemoveParent(p);
}

/**
 * @brief: find parents given a node id; used to generate discrete configurations
 * @return a set of pointers to the parents of a node
 */
set<Node*> Network::GetParentPtrsOfNode(int node_index) {
  set<Node*> set_par_ptrs;
  Node *node = map_idx_node_ptr.at(node_index); // TODO: function "FindNodePtrByIndex"
  for (const auto &idx : node->set_parent_indexes) { // "set_parent_indexes" contains both discrete and continuous parents
    set_par_ptrs.insert(map_idx_node_ptr.at(idx));
  }
  return set_par_ptrs;
}

/**
 * @brief: find the children given a node id
 */
set<Node*> Network::GetChildrenPtrsOfNode(int node_index) {
  set<Node*> set_chi_ptrs;
  Node *node = map_idx_node_ptr.at(node_index); // TODO: function "FindNodePtrByIndex"
  for (const auto &idx : node->set_children_indexes) {
    set_chi_ptrs.insert(map_idx_node_ptr.at(idx));
  }
  return set_chi_ptrs;
}

set<int> Network::GetChildrenIdxesOfNode(int node_index) {
    set<int> set_chi_idxes;
    Node *node = map_idx_node_ptr.at(node_index); // TODO: function "FindNodePtrByIndex"
    for (const auto &idx : node->set_children_indexes) {
        set_chi_idxes.insert(idx);
    }
    return set_chi_idxes;
}

/**
 * @brief: generate all the configurations of the parents for each node
 */
void Network::GenDiscParCombsForAllNodes() {
  for (const auto &id_np : this->map_idx_node_ptr) { // for each node (id-node_ptr pair) in the network
    auto np = id_np.second;
    np->GenDiscParCombs(GetParentPtrsOfNode(np->GetNodeIndex()));
  }
}

/**
 * @brief: obtain topological order
 * @return a vector<int>, the elements is the indexes of the nodes
 */
vector<int> Network::GetTopoOrd() {
  if (topo_ord.empty()) {
    this->GenTopoOrd();
  }
  return topo_ord;
}

/**
 * @brief: obtain reverse topological order
 */
vector<int> Network::GetReverseTopoOrd() {
  auto ord = this->GetTopoOrd();
  reverse(ord.begin(), ord.end());
  return ord;
}

/*!
 * @brief: generate the topological order
 * @return a vector<int>, the elements is the indexes of the nodes
 * generate 1. a directed adjacency matrix and 2. an in-degree array to generate the topological order
 */
// TODO: maybe not need to generate a directed adjacency matrix
// just use in-degree array and "set_children_indexes" to generate the ordering
// TODO: potential bug in "TopoSortOfDAGZeroInDegreeFirst" -> "TopoSortOfDAGZeroInDegreeFirst"
vector<int> Network::GenTopoOrd() {

  if (this->pure_discrete) {

    // convert the network to a directed adjacency matrix (n*n)
    // direct: p->c (i.e., graph[p][c] = 1)
    // TODO: use function "ConvertDAGNetworkToAdjacencyMatrix"
    // TODO: adjacency matrix or adjacency list? more memory for adjacency matrix
    int **graph = new int*[num_nodes];
    for (int i=0; i<num_nodes; ++i) {
      graph[i] = new int[num_nodes]();
    }

    // TODO: calculate the in-degrees here instead of in "TopoSortOfDAGZeroInDegreeFirst"
    for (auto &i_n_p : map_idx_node_ptr) { // for each node
      auto n_p = i_n_p.second;
      for (const auto &c_p : GetChildrenPtrsOfNode(n_p->GetNodeIndex())) { // TODO: use "GetChildrenIdxesOfNode"
        // TODO: each time assigning 1, add 1 to the in-degree of "c_p->GetNodeIndex()"
        graph[n_p->GetNodeIndex()][c_p->GetNodeIndex()] = 1;
      }
    }

    topo_ord = TopoSortOfDAGZeroInDegreeFirst(graph, num_nodes);

    for (int i=0; i<num_nodes; ++i) {
        SAFE_DELETE_ARRAY(graph[i]);
    }
      SAFE_DELETE_ARRAY(graph);
  }
  else { // TODO: double-check, not check for the continuous cases

    // If the network is not pure discrete, then it is conditional Gaussian.
    // Discrete nodes should not have continuous parents.
    // Continuous nodes can have both discrete and continuous parents.
    // In topological ordering, all discrete nodes should occur before any continuous node.
    // todo: test correctness of the case of Gaussian network

    set<Node*> set_disc_node_ptr, set_cont_node_ptr;
    for (const auto &i_n_p : map_idx_node_ptr) {
      auto n_p = i_n_p.second;
      if (n_p->is_discrete) {
        set_disc_node_ptr.insert(n_p);
      } else {
        set_cont_node_ptr.insert(n_p);
      }
    }
    int **graph_disc = new int*[set_disc_node_ptr.size()];
    int **graph_cont = new int*[set_cont_node_ptr.size()];
    for (int i=0; i<set_disc_node_ptr.size(); ++i) {
      graph_disc[i] = new int[set_disc_node_ptr.size()]();
    }
    for (int i=0; i<set_cont_node_ptr.size(); ++i) {
      graph_disc[i] = new int[set_disc_node_ptr.size()]();
      graph_cont[i] = new int[set_cont_node_ptr.size()]();
    }

    // Generate the ordering for discrete nodes.
    map<int, int> disc_order_index, disc_index_order;
    int disc_ord = 0;
    for (const auto &n_p : set_disc_node_ptr) {
      disc_order_index[disc_ord] = n_p->GetNodeIndex();
      disc_index_order[n_p->GetNodeIndex()] = disc_ord;
      ++disc_ord;
    }
    for (const auto &n_p : set_disc_node_ptr) {
      for (const auto &c_p : GetChildrenPtrsOfNode(n_p->GetNodeIndex())) {
        if (!c_p->is_discrete) { continue; }
        graph_disc[ disc_index_order[n_p->GetNodeIndex()] ]
                  [ disc_index_order[c_p->GetNodeIndex()] ] = 1;
      }
    }
    vector<int> topo_ord_disc = TopoSortOfDAGZeroInDegreeFirst(graph_disc, set_disc_node_ptr.size());

    // Generate the ordering for continuous nodes.
    map<int, int> cont_order_index, cont_index_order;
    int cont_ord = 0;
    for (const auto &n_p : set_cont_node_ptr) {
      cont_order_index[cont_ord] = n_p->GetNodeIndex();
      cont_index_order[n_p->GetNodeIndex()] = cont_ord;
      ++cont_ord;
    }
    for (const auto &n_p : set_cont_node_ptr) {
      for (const auto &c_p : GetChildrenPtrsOfNode(n_p->GetNodeIndex())) { // TODO: use "GetChildrenIdxesOfNode"
        graph_cont[ cont_index_order[n_p->GetNodeIndex()] ]
                  [ cont_index_order[c_p->GetNodeIndex()] ] = 1;
      }
    }
    vector<int> topo_ord_cont = TopoSortOfDAGZeroInDegreeFirst(graph_cont, set_cont_node_ptr.size());
    // Restore the index from the ordering.
    for (auto &o : topo_ord_cont) {
      o = cont_order_index[o];
    }

    // Concatinate topo_ord_disc and topo_ord_cont.
    topo_ord_disc.insert(topo_ord_disc.end(), topo_ord_cont.begin(), topo_ord_cont.end());
    this->topo_ord = topo_ord_disc;

    for (int i=0; i<set_disc_node_ptr.size(); ++i) {
        SAFE_DELETE_ARRAY(graph_disc[i]);
    }
    for (int i=0; i<set_cont_node_ptr.size(); ++i) {
        SAFE_DELETE_ARRAY(graph_cont[i]);
    }
      SAFE_DELETE_ARRAY(graph_disc);
      SAFE_DELETE_ARRAY(graph_cont);
  }
  return topo_ord;
}

/**
 * @brief: convert network to a dense directed adjacency matrix (n*n)
 */
int** Network::ConvertDAGNetworkToAdjacencyMatrix() {
  int **matrix = new int* [num_nodes];
  for (int i=0; i<num_nodes; ++i) {
    matrix[i] = new int[num_nodes]();
  }

  // TODO: calculate the in-degrees here
  // TODO: instead of in "TopoSortOfDAGZeroInDegreeFirst" and "DirectedGraphContainsCircleByBFS"
  // direct: node_ptr->child_ptr (i.e., graph[node_ptr][child_ptr] = 1)
  for (const auto &id_node_ptr : map_idx_node_ptr) { // for each node
    auto node_ptr = id_node_ptr.second;
    auto node_idx = id_node_ptr.first;

    for (const auto &child_ptr : GetChildrenPtrsOfNode(node_ptr->GetNodeIndex())) { // TODO: use "GetChildrenIdxesOfNode"
      // TODO: each time assigning 1, add 1 to the in-degree of "child_ptr->GetNodeIndex()"
      matrix[node_ptr->GetNodeIndex()][child_ptr->GetNodeIndex()] = 1;
    }
//      for (const auto &child_idx : GetChildrenIdxesOfNode(node_idx)) {
//          // TODO: each time assigning 1, add 1 to the in-degree of "child_ptr->GetNodeIndex()"
//          matrix[node_idx][child_idx] = 1;
//      }
  }
  return matrix;
}

/**
 * @brief: check if network has loops.
 */
bool Network::ContainCircle() {
  int **graph = ConvertDAGNetworkToAdjacencyMatrix();
  bool result = DirectedGraphContainsCircleByBFS(graph, num_nodes);

  for (int i = 0; i < num_nodes; ++i) {
      SAFE_DELETE_ARRAY(graph[i]);
  }
    SAFE_DELETE_ARRAY(graph);
  return result;
}

/**
 * @brief: get the number of parameters of the network, based on probability tables
 *         (and other parameters in continuous variables)
 */
int Network::GetNumParams() const {
  int result = 0;
  for (const auto &i_n : map_idx_node_ptr) { // TODO: function "FindNodePtrByIndex"
    result += i_n.second->GetNumParams(); // TODO: Node::GetNumParams is a virtual function
  }
  return result;
}

/**
 * @brief: this is a virtual function;
 * The performance of variable elimination relies heavily on the elimination ordering
 */
vector<int> Network::SimplifyDefaultElimOrd(DiscreteConfig evidence) {//TODO: use C++ pure virtual function
  fprintf(stderr, "Function [%s] not implemented yet!", __FUNCTION__);
  exit(1);
}

/**
 * @brief: get the Markov Blanket of a node
 */
set<int> Network::GetMarkovBlanketIndexesOfNode(Node *node_ptr) {
  // node: set does not contain repeated elements
  set<int> markov_blanket_node_index;

  // Add parents.
  for (const auto &par_ptr : GetParentPtrsOfNode(node_ptr->GetNodeIndex())) {
    markov_blanket_node_index.insert(par_ptr->GetNodeIndex());
  }

  // Add children and parents of children.
  for (const auto &chil_ptr : GetChildrenPtrsOfNode(node_ptr->GetNodeIndex())) { // TODO: use "GetChildrenIdxesOfNode"
    markov_blanket_node_index.insert(chil_ptr->GetNodeIndex());
    for (const auto &par_chil_ptr : GetParentPtrsOfNode(chil_ptr->GetNodeIndex())) {
      markov_blanket_node_index.insert(par_chil_ptr->GetNodeIndex());
    }
  }

  markov_blanket_node_index.erase(node_ptr->GetNodeIndex());

  return markov_blanket_node_index;
}

/**
 * @brief: order the edges based on the topological order of the nodes
 * according to Chickering, "A Transformational Characterization of Equivalent Bayesian Network Structures", 1995
 */
void Network::OrderEdge() {
    vector<Edge> tmp_edge_order;

    // Perform a topological sort on the NODES in g
    vector<int> topo_order = GetTopoOrd();

    for (Edge edge : vec_edges) {
        edge.is_ordered = false;
    }

    for (int j = 0; j < topo_order.size(); ++j) {
        // Let y be the lowest ordered NODE that has an unordered EDGE incident into it
        int y_idx = topo_order.at(j);
        Node* y = FindNodePtrByIndex(y_idx);
        if (!y->set_parent_indexes.empty()) {
            // all the parents of y must precede y in the topo order, so we traverse from j-1 to 0
            for (int k = j - 1; k >= 0; --k) {
                // Let x be the highest ordered NODE for which x->y is not ordered
                int x_idx = topo_order.at(k);
                if (y->set_parent_indexes.find(x_idx) != y->set_parent_indexes.end()) { // x->y exists
                    Node* x = FindNodePtrByIndex(x_idx);
                    int edge_pos = GetDirectedEdge(x, y);
                    if (!vec_edges.at(edge_pos).is_ordered) { // find unordered edge x->y
                        vec_edges.at(edge_pos).is_ordered = true;
                        tmp_edge_order.push_back(vec_edges.at(edge_pos));
                    }
                }
            }
        }
    }
    if (tmp_edge_order.size() != vec_edges.size()) {
        fprintf(stderr, "Error in function %s! \nnum of edges = %d but num of ordered edges = %d!", __FUNCTION__,
                vec_edges.size(), tmp_edge_order.size());
        exit(1);
    }
    edge_order = tmp_edge_order;
}

/**
 * @brief: label each edge as either compelled or reversible
 * change the reversible edges to undirected edges
 * according to Chickering, "A Transformational Characterization of Equivalent Bayesian Network Structures", 1995
 */
void Network::FindCompelled() {
    /*
     * Label every edge in g as "unknown"
     * every edge in "edge_order" is considered to be UNKNOWN
     * when one edge should be labeled as either COMPELLED or REVERSIBLE, its label in "vec_edges" is changed,
     * and at the same time, this edge will be erase from "edge_order", which means it is not UNKNOWN now
     * when "edge_order" has no element, all edges are labeled and the process terminates.
     */
    // While there are edges labelled "unknown" in g
    while (!edge_order.empty()) {
        // Let x->y be the lowest ordered edge that is labelled "unknown"
        Node* x = edge_order.at(0).GetNode1();
        Node* y = edge_order.at(0).GetNode2();

        bool tmp1 = false; // to label whether the IF conditions are satisfied
        for (const int &w_idx : x->set_parent_indexes) {
            Node* w = FindNodePtrByIndex(w_idx);
            if (vec_edges.at(GetDirectedEdge(w, x)).label == COMPELLED) {
                // For every edge w->x labelled "compelled"
                if (!y->IsParentOfThisNode(w)) {
                    /**
                     * If w is not a parent of y, then label x->y and every edge incident into y with "compelled"
                     * and goto 3 (the while loop)
                     */
                    tmp1 = true;
                    // 1. x->y: edge_order[0]
                    vec_edges.at(GetDirectedEdge(x, y)).label = COMPELLED;
                    edge_order.erase(edge_order.begin());
                    // 2. every edge incident into y
                    for (const int &par_y_idx : y->set_parent_indexes) {
                        Node* par_y = FindNodePtrByIndex(par_y_idx);
                        int pos_edge_order = GetDirectedEdgeFromEdgeOrder(par_y, y);
                        if (pos_edge_order != -1) { // this edge has not been labeled
                            vec_edges.at(GetDirectedEdge(par_y, y)).label = COMPELLED;
                            edge_order.erase(edge_order.begin() + pos_edge_order);
                        } else {
                            vec_edges.at(GetDirectedEdge(par_y, y)).label = COMPELLED;
                        }
                    }
                    break;
                } else {
                    // Else label w->y with "compelled"
                    int pos_edge_order = GetDirectedEdgeFromEdgeOrder(w, y);
                    if (pos_edge_order != -1) { // this edge has not been labeled
                        vec_edges.at(GetDirectedEdge(w, y)).label = COMPELLED;
                        edge_order.erase(edge_order.begin() + pos_edge_order);
                    }
                }
            }
        }

        if (!tmp1) { // otherwise, directly go to the while loop and do not
            bool tmp2 = false; // to label whether the IF conditions are satisfied
            for (const int &z_idx : y->set_parent_indexes) {
                if (z_idx != x->GetNodeIndex()) {
                    Node* z = FindNodePtrByIndex(z_idx);
                    if (!x->IsParentOfThisNode(z)) {
                        /**
                         * If there exists an edge z->y such that z != x and z is not a parent of x,
                         * then label x->y and all "unknown" edges incident into y with "compelled"
                         */
                        tmp2 = true;
                        // 1. x->y: edge_order[0]
                        vec_edges.at(GetDirectedEdge(x, y)).label = COMPELLED;
                        edge_order.erase(edge_order.begin());
                        // 2. every "unknown" edge incident into y
                        for (const int &par_y_idx : y->set_parent_indexes) {
                            Node* par_y = FindNodePtrByIndex(par_y_idx);
                            int pos_edge_order = GetDirectedEdgeFromEdgeOrder(par_y, y);
                            if (pos_edge_order != -1) { // this edge has not been labeled
                                vec_edges.at(GetDirectedEdge(par_y, y)).label = COMPELLED;
                                edge_order.erase(edge_order.begin() + pos_edge_order);
                            }
                        }
                    }
                }
            }
            if (!tmp2) {
                // Else label x->y and all "unknown" edges incident into y with "reversible"
                // 1. x->y: edge_order[0]
                vec_edges.at(GetDirectedEdge(x, y)).label = REVERSIBLE;
                edge_order.erase(edge_order.begin());
                // 2. every "unknown" edge incident into y
                for (const int &par_y_idx : y->set_parent_indexes) {
                    Node* par_y = FindNodePtrByIndex(par_y_idx);
                    int pos_edge_order = GetDirectedEdgeFromEdgeOrder(par_y, y);
                    if (pos_edge_order != -1) { // this edge has not been labeled
                        vec_edges.at(GetDirectedEdge(par_y, y)).label = REVERSIBLE;
                        edge_order.erase(edge_order.begin() + pos_edge_order);
                    }
                }
            }
        }
    }
}

bool Network::IsDAG() {
    for (Edge edge : vec_edges) {
        if (!edge.IsDirected()) {
            return false;
        }
    }
    return true;
}


#pragma clang diagnostic pop