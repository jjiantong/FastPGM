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
         << "Begin finding the skeleton" << endl << "Level 0... ";

    for (int i = 0; i < network->num_nodes; i++) { // find neighbor set of each node i
        set<int> adjacency;
        for (int j = 0; j < network->num_nodes; j++) { // all nodes except for i itself are neighbors of i
            if (i == j)
                continue;
            adjacency.insert(j);
        }
        network->adjacencies.insert(pair<int, set<int>>(i, adjacency));
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
            network->adjacencies[node_idx1].erase(node_idx2);
            network->adjacencies[node_idx2].erase(node_idx1);
            // 3. add conditioning set (an empty set for level 0) to sepset
            ci_test->sepset.insert(make_pair(make_pair(node_idx1, node_idx2), empty_set));
            ci_test->sepset.insert(make_pair(make_pair(node_idx2, node_idx1), empty_set));
        } else { // the edge remains
            edge_it++;
        }
    }

    for (int d = 1; d < depth; ++d) {
        cout << "Level " << d << "... ";

        bool more = SearchAtDepth(d);

        if (!more) {
            break;
        }
    }

    cout << "\n==================================================" << '\n'
         << "Begin orienting v-structure " << endl;

    OrientVStructure();


}

/**
 * @brief: search for each level (c_depth, c_depth > 0) except for level 0
 */
bool PCStable::SearchAtDepth(int c_depth) {
    /**
     * the copied adjacency sets of all nodes are used and kept unchanged at each particular level c_depth
     * consequently, an edge deletion at one level does not affect the conditioning sets of the other nodes
     * and thus the output is independent with the variable ordering, called PC-stable
     */
    map<int, set<int>> adjacencies_copy = network->adjacencies;
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
            network->adjacencies[x->GetNodeIndex()].erase(y->GetNodeIndex());
            network->adjacencies[y->GetNodeIndex()].erase(x->GetNodeIndex());
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
    // copy to a vector to access by position, which will be used for choice generating
    vector<int> vec_adjx;
    for (int adj : adjx) {
        vec_adjx.push_back(adj);
    }

    if (adjx.size() >= c_depth) {
        ChoiceGenerator cg (adjx.size(), c_depth);
        vector<int> choice;

        while (!(choice = cg.Next()).empty()) {
            set<int> Z;
            for (int z_idx : choice) {
                Z.insert(vec_adjx.at(z_idx));
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

/**
 * @brief: set all direction of the edges that are part of a v-structure
 * for each triplet a -- b -- c, if
 *      1) a is not adjacent to c
 *      2) b is not in the sepset of (a, c)
 * replace it with the v-structure a -> b <- c
 *
 * note that for PC-stable, the skeleton is estimated order-independently but not the v-structures!
 * conflicting edges are simply overwritten
 * this means that the orientation of one conflicting edge is determined by the v-structure that is last considered
 */
void PCStable::OrientVStructure() {

    for (int b = 0; b < network->num_nodes; ++b) { // for all nodes in the graph
        set<int> adjacent_nodes = network->adjacencies[b];
        vector<int> vec_adjacent_nodes;
        for (int adjs : adjacent_nodes) {
            vec_adjacent_nodes.push_back(adjs);
        }

        if (adjacent_nodes.size() < 2) {
            continue;
        }
        ChoiceGenerator cg(adjacent_nodes.size(), 2); // to find two adjacent nodes of this node and check
        vector<int> combination;

        while (!(combination = cg.Next()).empty()) {
            int a = vec_adjacent_nodes.at(combination.at(0));
            int c = vec_adjacent_nodes.at(combination.at(1));

            // 1) a is not adjacent to c; skip if a is adjacent to c
            if (network->IsAdjacentTo(a, c)) {
                continue;
            }
            // 2) b is not in the sepset of (a, c)
            set<int> sepset = ci_test->sepset[make_pair(a, c)];
            if (sepset.find(b) == sepset.end()) {
//            if (!sepset.empty() && sepset.find(b) == sepset.end()) {
                if (network->DeleteUndirectedEdge(a, b) ||
                    network->DeleteDirectedEdge(b, a)) { // it means a conflict if DeleteDirectedEdge returns true
                    network->AddDirectedEdge(a, b);
                }
                if (network->DeleteUndirectedEdge(c, b) ||
                    network->DeleteDirectedEdge(b, c)) {
                    network->AddDirectedEdge(c, b);
                }
            }
        }
    }
}
