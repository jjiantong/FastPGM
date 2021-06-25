//
// Created by jjt on 2021/6/23.
//

#include "PCStable.h"

void PCStable::StructLearnCompData(Dataset *dts, bool print_struct) {
    // record time
    struct timeval start, end;
    double diff;
    gettimeofday(&start,NULL);

    cout << "==================================================" << '\n'
         << "Begin structural learning with PC-stable" << endl;

    depth = (depth == -1) ? 1000 : depth; // depth = -1 means no limitation
    AssignNodeInformation(dts);
    StructLearnByPCStable(dts, print_struct);

    // print time
    gettimeofday(&end,NULL);
    diff = (end.tv_sec-start.tv_sec) + ((double)(end.tv_usec-start.tv_usec))/1.0E6;
    setlocale(LC_NUMERIC, "");
    cout << "==================================================" << '\n'
         << "The time spent to generate CPDAG with PC-stable is " << diff << " seconds" << endl;
}

void PCStable::StructLearnByPCStable(Dataset *dts, bool print_struct) {
    cout << "==================================================" << '\n'
         << "Generating complete undirected graph" << endl;

    for (int i = 0; i < network->num_nodes; i++) {
        for (int j = i + 1; j < network->num_nodes; j++) {
            network->AddUndirectedEdge(i, j);
        }
    }

    cout << "==================================================" << '\n'
         << "Begin level 0" << endl;

    map<int, set<int>> adjacencies; // key is node index, value is the set of neighbor node index of the node

    for (int i = 0; i < network->num_nodes; i++) { // find neighbor set of each node i
        set<int> adjacency;
        for (int j = 0; j < network->num_nodes; j++) { // all nodes except for i itself are neighbors of i
            if (i == j)
                continue;
            adjacency.insert(j);
        }
        adjacencies.insert(pair<int, set<int>>(i, adjacency));
    }

    /**
     * note that the for loop does not have "edge_it++", "edge_it++" only happens when (*edge_it) is not erased
     * for the case of erasing (*edge_it), the iterator will point to the next edge after erasing the current edge
     * (erasing operation is in Network::DeleteUndirectedEdge)
     */
    for (auto edge_it = network->vec_edges.begin(); edge_it != network->vec_edges.end();) {
        int node_idx1 = (*edge_it).GetNode1()->GetNodeIndex();
        int node_idx2 = (*edge_it).GetNode2()->GetNodeIndex();
        set<int> empty_set;

        if (ci_test->IsIndependent(node_idx1, node_idx2, empty_set, "") || //TODO: I(x1, x2) = I(x2, x1)?
            ci_test->IsIndependent(node_idx2, node_idx1, empty_set, "")) {

            network->DeleteUndirectedEdge(node_idx1, node_idx2);
            adjacencies[node_idx1].erase(node_idx2);
            adjacencies[node_idx2].erase(node_idx1);
            ci_test->sepset.insert(make_pair(make_pair(node_idx1, node_idx2), empty_set));
            ci_test->sepset.insert(make_pair(make_pair(node_idx2, node_idx1), empty_set));
        } else {
            edge_it++;
        }
    }

    cout << "num nodes = " << network->num_nodes << endl;
    cout << "num edges = " << network->num_edges << endl;
    for (int i = 0; i < network->num_nodes; ++i) {
        cout << i << ": ";
        for (int adj : adjacencies[i]) {
            cout << adj << ", ";
        }
        cout << endl;
    }

}