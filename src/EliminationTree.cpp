//
// Created by LinjianLi on 2019/9/1.
//

#include "EliminationTree.h"

EliminationTree::EliminationTree(Network *net) {

  struct timeval start, end;
  double diff;
  gettimeofday(&start,NULL);

  network = net;
  int **direc_adjac_matrix = ConvertDAGNetworkToAdjacencyMatrix(network);
  Moralize(direc_adjac_matrix, network->num_nodes);
  int **undirec_adjac_matrix = direc_adjac_matrix;  // Change a name because it has been moralized.
  vector<int> elim_ord = MinNeighbourElimOrd(undirec_adjac_matrix, network->num_nodes);
  Triangulate(network, undirec_adjac_matrix, network->num_nodes, elim_ord, set_clique_ptr_container);

  // Theoretically, this step is not necessary.
  ElimRedundantCliques();

  FormJunctionTree(set_clique_ptr_container);

  NumberTheCliquesAndSeparators();

  AssignPotentials();
  BackUpJunctionTree();

  gettimeofday(&end,NULL);
  diff = (end.tv_sec-start.tv_sec) + ((double)(end.tv_usec-start.tv_usec))/1.0E6;
  setlocale(LC_NUMERIC, "");
  cout << "==================================================" << '\n'
       << "The time spent to construct junction tree is " << diff << " seconds" << endl;
  delete[] direc_adjac_matrix;
}