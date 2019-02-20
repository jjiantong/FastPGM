//
// Created by LinjianLi on 2019/2/16.
//

#include "JunctionTree.h"

JunctionTree::JunctionTree(Network *net) {

}


int** JunctionTree::ConvertDAGNetworkToAdjacencyMatrix(Network *net) {
  int num_nodes = net->n_nodes;
  int **adjac_matrix = new int* [n_nodes];
  for (int i=0; i<n_nodes; ++i) {
    adjac_matrix[i] = new int[n_nodes]();
  }
  for (auto &node_ptr : net->set_node_ptr_container) {
    int from, from2, to;
    from = node_ptr->GetNodeIndex();
    for (auto &child_ptr : node_ptr->set_children_ptrs) {
      to = child_ptr->GetNodeIndex();
      adjac_matrix[from][to] = 1;
    }
    return adjac_matrix;
  }
}


void JunctionTree::Moralize(int **direc_adjac_matrix, int num_nodes) {
  for (int i=0; i<num_nodes; ++i) {
    for (int j=0; j<num_nodes; ++j) {
      if (direc_adjac_matrix[i][j]==1 || direc_adjac_matrix[j][i]==1) {
        direc_adjac_matrix[i][j] = 1;
        direc_adjac_matrix[j][i] = 1;
      }
    }
  }
}


void JunctionTree::Triangulate(Network *net,
                               int **adjac_matrix,
                               const int &num_nodes,
                               vector<int> elim_ord,
                               set<Clique*> &cliques) {
  if (elim_ord.size()==1) {return;}
  set<int> set_neighbours;
  set<Node*> set_node_ptrs_to_form_a_clique;
  int first_node_in_elim_ord = elim_ord.front();
  set_node_ptrs_to_form_a_clique.insert(net->GivenIndexToFindNodePointer(first_node_in_elim_ord));
  for (int j=0; j<num_nodes; ++j) {
    if (adjac_matrix[first_node_in_elim_ord][j]==1) {
      set_neighbours.insert(j);
    }
  }

  // Form a clique.
  for (auto &nei : set_neighbours) {
    for (auto &index2 : set_neighbours) {
      if (nei!=index2) {
        adjac_matrix[nei][index2] = 1;
        adjac_matrix[index2][nei] = 1;
      }
    }
    set_node_ptrs_to_form_a_clique.insert(net->GivenIndexToFindNodePointer(nei));
  }

  cliques.insert(new Clique(set_node_ptrs_to_form_a_clique));
  
  // Remove the first node in elimination ordering, which has already form a clique.
  // The node has been removed, so the edges connected to it should be removed too.
  elim_ord.erase(elim_ord.begin());
  for (auto &nei : set_neighbours) {
    adjac_matrix[first_node_in_elim_ord][nei] = 0;
    adjac_matrix[nei][first_node_in_elim_ord] = 0;
  }

  Triangulate(net, adjac_matrix, num_nodes, elim_ord, cliques);
}


void JunctionTree::FormJunctionTree(set<Clique*> &cliques) {
  for (auto &clique_ptr : cliques) {
    for (auto &clique_ptr_2 : cliques) {
      set<int> common_related_variables;
      set_intersection(clique_ptr->related_variables.begin(),clique_ptr->related_variables.end(),
                       clique_ptr_2->related_variables.begin(),clique_ptr_2->related_variables.end(),
                       std::inserter(common_related_variables,common_related_variables.begin()));
      set_separator_ptr_container.insert(new Separator(common_related_variables));

    }
  }

}