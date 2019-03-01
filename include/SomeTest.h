//
// Created by Linjian Li on 2018/11/30.
//

#ifndef BAYESIANNETWORK_SOMETEST_H
#define BAYESIANNETWORK_SOMETEST_H

#include <iostream>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include "Trainer.h"
#include "Network.h"
#include "CustomNetwork.h"
#include "JunctionTree.h"

class SomeTest {
 public:
  void test1() {
    fprintf(stdout,"%s\n",__FUNCTION__);

    auto *network = new CustomNetwork();
    network->ConstructCustomNetworkStructFromFile("../data/example_custom_network_file2.txt");
    network->SetCustomNetworkParamsFromFile("../data/example_custom_network_file2.txt");

    Combination e;
    e.insert(pair<int,int>(1,0));
    e.insert(pair<int,int>(4,0));

    cout << "********************************** Variable Elimination Algorithm" << endl;
    int z[5] = {5,4,3,2,1};
    Node *node_ptr = network->GivenIndexToFindNodePointer(0);
    Factor f1 = network->VarElimInferReturnPossib(z,5,e,node_ptr);
    f1.PrintPotentials();
    int z1[5] = {5,4,3,2,0};
    node_ptr = network->GivenIndexToFindNodePointer(1);
    f1 = network->VarElimInferReturnPossib(z1,5,e,node_ptr);
    f1.PrintPotentials();
    int z2[5] = {5,4,3,1,0};
    node_ptr = network->GivenIndexToFindNodePointer(2);
    f1 = network->VarElimInferReturnPossib(z2,5,e,node_ptr);
    f1.PrintPotentials();
    int z3[5] = {5,4,2,1,0};
    node_ptr = network->GivenIndexToFindNodePointer(3);
    f1 = network->VarElimInferReturnPossib(z3,5,e,node_ptr);
    f1.PrintPotentials();
    int z4[5] = {5,3,2,1,0};
    node_ptr = network->GivenIndexToFindNodePointer(4);
    f1 = network->VarElimInferReturnPossib(z4,5,e,node_ptr);
    f1.PrintPotentials();
    int z5[5] = {4,3,2,1,0};
    node_ptr = network->GivenIndexToFindNodePointer(5);
    f1 = network->VarElimInferReturnPossib(z5,5,e,node_ptr);
    f1.PrintPotentials();

    JunctionTree *jt = new JunctionTree(network);
    jt->LoadEvidence(e);

    jt->MessagePassingUpdateJT();

    cout << "********************************** Junction Tree Algorithm" << endl;
    set<int> indexes;
    indexes.insert(0);
    Factor f2 = jt->InferenceForVarIndexsReturnPossib(indexes);
    f2.PrintPotentials();
    indexes.clear();
    indexes.insert(1);
    f2 = jt->InferenceForVarIndexsReturnPossib(indexes);
    f2.PrintPotentials();
    indexes.clear();
    indexes.insert(2);
    f2 = jt->InferenceForVarIndexsReturnPossib(indexes);
    f2.PrintPotentials();
    indexes.clear();
    indexes.insert(3);
    f2 = jt->InferenceForVarIndexsReturnPossib(indexes);
    f2.PrintPotentials();
    indexes.clear();
    indexes.insert(4);
    f2 = jt->InferenceForVarIndexsReturnPossib(indexes);
    f2.PrintPotentials();
    indexes.clear();
    indexes.insert(5);
    f2 = jt->InferenceForVarIndexsReturnPossib(indexes);
    f2.PrintPotentials();

    std::cout << "test1 finished" << std::endl;
  }


  void test2(Network* network, Trainer *tester, int i) {
    fprintf(stdout,"%s\n",__FUNCTION__);
    int e_num=network->n_nodes-1, *e_index=new int[e_num], *e_value=new int[e_num];
    for (int j=0; j<e_num; ++j) {
      e_index[j] = j+1;
      e_value[j] = tester->train_set_X[i][j];
    }

    Combination E;
    E = network->ConstructEvidence(e_index, e_value, e_num);
    cout << "Size of evidence: " << E.size() << endl;
    cout << "Evidence (only print 1): { ";
    for (auto &e : E) {
      cout << e.first << ':' << e.second << ' ';
    }
    cout << " }" << endl;

    Node *node_ptr = network->GivenIndexToFindNodePointer(i);
    Factor f1 = network->VarElimInferReturnPossib(E,node_ptr);
    f1.PrintPotentials();
    JunctionTree *jt = new JunctionTree(network);
    jt->LoadEvidence(E);
/*
    cout << "********************************** Before message passing" << endl;
    for (auto &c : jt->set_clique_ptr_container) {
      if (c->related_variables.find(0)!=c->related_variables.end()) {c->PrintPotentials();}
    }
*/
    jt->MessagePassingUpdateJT();

/*
    cout << "********************************** After message passing" << endl;
    for (auto &c : jt->set_clique_ptr_container) {
      if (c->related_variables.find(0)!=c->related_variables.end()) {c->PrintPotentials();}
    }
*/
    set<int> indexes;
    indexes.insert(i);
    Factor f2 = jt->InferenceForVarIndexsReturnPossib(indexes);
    f2.PrintPotentials();
  }

  void test3(Network* network, int i) {
    fprintf(stdout,"%s\n",__FUNCTION__);
    Combination E;
    E.insert(pair<int,int>(104,1));
    E.insert(pair<int,int>(112,1));
    E.insert(pair<int,int>(99,1));
    cout << "Size of evidence: " << E.size() << endl;
    cout << "Evidence (only print 1): { ";
    for (auto &e : E) {
      cout << e.first << ':' << e.second << ' ';
    }
    cout << " }" << endl;

    Node *node_ptr = network->GivenIndexToFindNodePointer(i);
    Factor f1 = network->VarElimInferReturnPossib(E,node_ptr);
    f1.PrintPotentials();


    JunctionTree *jt = new JunctionTree(network);
    jt->LoadEvidence(E);

    jt->MessagePassingUpdateJT();

    set<int> indexes;
    indexes.insert(i);
    Factor f2 = jt->InferenceForVarIndexsReturnPossib(indexes);
    f2.PrintPotentials();
  }

};


#endif //BAYESIANNETWORK_SOMETEST_H
