//
// Created by jjt on 2021/6/23.
//

#include "PCStable.h"

PCStable::PCStable(Network *net) {
    network = net;
    ci_test = new IndependenceTest();
    num_ci_test = 0;
    num_dependence_judgement = 0;
}

PCStable::PCStable(Network *net, int d, bool s) {
    network = net;
    ci_test = new IndependenceTest();
    num_ci_test = 0;
    num_dependence_judgement = 0;
    depth = d;
    stable = s;
}

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
            // the edge node1 -- node2 should be removed
            // 1. delete the edge
            network->DeleteUndirectedEdge(node_idx1, node_idx2);
            // 2. remove each other from adjacency set
            adjacencies[node_idx1].erase(node_idx2);
            adjacencies[node_idx2].erase(node_idx1);
            // 3. add conditioning set (an empty set for level 0) to sepset
            ci_test->sepset.insert(make_pair(make_pair(node_idx1, node_idx2), empty_set));
            ci_test->sepset.insert(make_pair(make_pair(node_idx2, node_idx1), empty_set));
        } else { // the edge remains
            edge_it++;
        }
    }

    for (int d = 1; d < depth; ++d) {
        cout << "==================================================" << '\n'
             << "Begin level " << d << endl;

        bool more = SearchAtDepth(adjacencies, d);

        if (!more) {
            break;
        }
    }
}

/**
 * @brief: search for each level (c_depth, c_depth > 0) except for level 0
 */
bool PCStable::SearchAtDepth(map<int, set<int>> &adjacencies, int c_depth) {
    /**
     * the copied adjacency sets of all nodes are used and kept unchanged at each particular level c_depth
     * consequently, an edge deletion at one level does not affect the conditioning sets of the other nodes
     * and thus the output is independent with the variable ordering, called PC-stable
     */
    map<int, set<int>> adjacencies_copy = adjacencies;
    /**
     * note that the for loop does not have "edge_it++", "edge_it++" only happens when (*edge_it) is not erased
     * for the case of erasing (*edge_it), the iterator will point to the next edge after erasing the current edge
     * (erasing operation is in Network::DeleteUndirectedEdge)
     */
    for (auto edge_it = network->vec_edges.begin(); edge_it != network->vec_edges.end();) {
        Node* x = (*edge_it).GetNode1();
        Node* y = (*edge_it).GetNode2();

        if (CheckSide(adjacencies_copy, c_depth, x, y) ||
            CheckSide(adjacencies_copy, c_depth, y, x)) {
            /**
             * the edge x -- y  should be removed
             * note that the sepsets have already been added,
             * so we only need to 1) delete the edge; 2) remove each other from adjacency set
             */
            network->DeleteUndirectedEdge(x->GetNodeIndex(), y->GetNodeIndex());
            adjacencies[x->GetNodeIndex()].erase(y->GetNodeIndex());
            adjacencies[y->GetNodeIndex()].erase(x->GetNodeIndex());
        } else { // the edge x -- y remains
            edge_it++;
        }
    }
    return (FreeDegree(adjacencies_copy) > c_depth);
}

/**
 * @brief: check whether a subset Z of adjacency of x can be found,
 *         such that x _||_ y | Z, and the size of Z is "c_depth"
 *         if such a Z can be found, add Z into the sepset of x,y and y,x
 * @return true if such a Z can be found, which means edge x -- y should be deleted
 */
bool PCStable::CheckSide(map<int, set<int>> adjacencies, int c_depth, Node* x, Node* y) {
    int x_idx = x->GetNodeIndex();
    int y_idx = y->GetNodeIndex();

    set<int> adjx = adjacencies[x_idx];
    if (adjx.find(y_idx) == adjx.end()) {
        fprintf(stderr, "Function [%s]: Node %d is not the neighbor of node %d!",
                __FUNCTION__, y_idx, x_idx);
        exit(1);
    }
    adjx.erase(y_idx);

    if (adjx.size() >= c_depth) {
        ChoiceGenerator cg (adjx.size(), c_depth);
        vector<int> choice;

        while (!(choice = cg.Next()).empty()) {
            set<int> Z;
            for (int z : choice) {
                Z.insert(z);
            }
            num_ci_test++;
            bool independent = ci_test->IsIndependent(x_idx, y_idx, Z, "");
            if (!independent) {
                num_dependence_judgement++;
            } else {
                // add conditioning set to sepset
                ci_test->sepset.insert(make_pair(make_pair(x_idx, y_idx), Z));
                ci_test->sepset.insert(make_pair(make_pair(y_idx, x_idx), Z));
                return true;
            }
        }
    }
    return false;
}

/**
 * @return the max |adj(X)\{Y}| for all pairs of nodes (X,Y)
 * I think it is just equal to the max |adj(X)| - 1 for all nodes X
 */
int PCStable::FreeDegree(map<int, set<int>> adjacencies) {
    int max = 0;
    for (int i = 0; i < network->num_nodes; ++i) {
        max = (adjacencies[i].size() > max) ? adjacencies[i].size() : max;
    }
    return (max - 1);
}