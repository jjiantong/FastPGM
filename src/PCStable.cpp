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

//    for (Edge edge : network->vec_edges) {
//    }
//
//
//    for (Edge edge : new ArrayList<>(edges)) {
//        if (scores.get(edge) < 0
//            || (knowledge.isForbidden(edge.getNode1().getName(), edge.getNode2().getName())
//                && (knowledge.isForbidden(edge.getNode2().getName(), edge.getNode1().getName())))) {
//            edges.remove(edge);
//            adjacencies.get(edge.getNode1()).remove(edge.getNode2());
//            adjacencies.get(edge.getNode2()).remove(edge.getNode1());
//            sepset.set(edge.getNode1(), edge.getNode2(), new ArrayList<>());
//        }
//    }
}