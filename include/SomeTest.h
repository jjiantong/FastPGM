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
    std::cout << "doing test1..." << std::endl;

    auto *network = new CustomNetwork();
    network->ConstructCustomNetworkStructFromFile("../data/example_custom_network_file.txt");
    network->SetCustomNetworkParamsFromFile("../data/example_custom_network_file.txt");

    JunctionTree *jt = new JunctionTree(network);
    Combination e;
    pair<int, int> p;
    p.first = 3;
    p.second = -1;
    e.insert(p);
    jt->LoadEvidence(e);
    jt->MessagePassingUpdateJT();

    std::cout << "test1 finished" << std::endl;
  }


  void test2(Network* network, Trainer *tester, int i) {
    cout << __FUNCTION__ << endl;
    int e_num=network->n_nodes-1, *e_index=new int[e_num], *e_value=new int[e_num];
    for (int j=0; j<e_num; ++j) {
      e_index[j] = j+1;
      e_value[j] = tester->train_set_X[i][j];
    }

    Combination E;
    //E = network->ConstructEvidence(e_index, e_value, e_num);
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

    cout << "**********************************" << endl;
    for (auto &c : jt->set_clique_ptr_container) {
      if (c->related_variables.find(0)!=c->related_variables.end()) {c->PrintPotentials();}
    }

    jt->MessagePassingUpdateJT();


    cout << "**********************************" << endl;
    for (auto &c : jt->set_clique_ptr_container) {
      if (c->related_variables.find(0)!=c->related_variables.end()) {c->PrintPotentials();}
    }

    set<int> indexes;
    indexes.insert(i);
    Factor f2 = jt->InferenceForVarIndexsReturnPossib(indexes);
    f2.PrintPotentials();
  }

};


#endif //BAYESIANNETWORK_SOMETEST_H
