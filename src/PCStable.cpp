//
// Created by jjt on 2021/6/23.
//

#include "PCStable.h"

PCStable::PCStable(Network *net, Dataset *dataset, double alpha) {
    network = net;
    ci_test = new IndependenceTest(dataset, alpha);
    num_ci_test = 0;
    num_dependence_judgement = 0;
}

PCStable::PCStable(Network *net, int d, Dataset *dataset, double alpha) {
    network = net;
    ci_test = new IndependenceTest(dataset, alpha);
    num_ci_test = 0;
    num_dependence_judgement = 0;
    depth = d;
}

void PCStable::StructLearnCompData(Dataset *dts, bool print_struct) {
    // record time
    timer.Start("pc-stable");

    cout << "==================================================" << '\n'
         << "Begin structural learning with PC-stable" << endl;

    depth = (depth == -1) ? 1000 : depth; // depth = -1 means no limitation
    AssignNodeInformation(dts);
    StructLearnByPCStable(print_struct);

    timer.Stop("pc-stable");
    setlocale(LC_NUMERIC, "");
    cout << "==================================================" << endl;
    cout << "# of CI-tests is " << num_ci_test << ", # of dependence judgements is " << num_dependence_judgement << endl;
    timer.Print("pc-stable");
    timer.Print("pc-stable step 1");
    timer.Print("pc-stable step 2");
    timer.Print("pc-stable step 3");
    ci_test->timer.Print("counting1");
    ci_test->timer.Print("counting2");
    ci_test->timer.Print("computing p-value");
}

void PCStable::StructLearnByPCStable(bool print_struct) {
    timer.Start("pc-stable step 1");
    cout << "==================================================" << '\n'
         << "Generating complete undirected graph" << endl;

    for (int i = 0; i < network->num_nodes; ++i) {
        for (int j = i + 1; j < network->num_nodes; ++j) {
            network->AddUndirectedEdge(i, j);
        }
    }

    cout << "==================================================" << '\n'
         << "Begin finding the skeleton" << endl << "Level 0... ";

    for (int i = 0; i < network->num_nodes; ++i) { // find neighbor set of each node i
        map<int, double> adjacency;
        for (int j = 0; j < network->num_nodes; ++j) { // all nodes except for i itself are neighbors of i
            if (i == j)
                continue;
            adjacency.insert(make_pair(j, 1.0));
        }
        network->adjacencies.insert(make_pair(i, adjacency));
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

        num_ci_test++;
//        bool independent = ci_test->IsIndependent(node_idx1, node_idx2, empty_set, "g square");
        IndependenceTest::Result result = ci_test->IndependenceResult(node_idx1, node_idx2, empty_set, "g square");
        bool independent = result.is_independent;

        if (!independent) { // the edge remains
            num_dependence_judgement++;
            // store the p value - smaller means a stronger association
            network->adjacencies.at(node_idx1).at(node_idx2) = result.p_value;
            network->adjacencies.at(node_idx2).at(node_idx1) = result.p_value;
            edge_it++;
        } else {
            // the edge node1 -- node2 should be removed
            // 1. delete the edge
            network->DeleteUndirectedEdge(node_idx1, node_idx2);
            // 2. remove each other from adjacency set
            network->adjacencies[node_idx1].erase(node_idx2);
            network->adjacencies[node_idx2].erase(node_idx1);
            // 3. add conditioning set (an empty set for level 0) to sepset
            ci_test->sepset.insert(make_pair(make_pair(node_idx1, node_idx2), empty_set));
            ci_test->sepset.insert(make_pair(make_pair(node_idx2, node_idx1), empty_set));
        }
    }

    for (int d = 1; d < depth; ++d) {
        cout << "Level " << d << "... ";

        bool more = SearchAtDepth(d);

        if (!more) {
            break;
        }
    }

    timer.Stop("pc-stable step 1");

//    if (print_struct) {
//        network->PrintEachEdgeWithIndex();
//    }

    cout << "\n==================================================" << '\n'
         << "Begin orienting v-structure" << endl;

    timer.Start("pc-stable step 2");
    OrientVStructure();
    timer.Stop("pc-stable step 2");

//    if (print_struct) {
//        network->PrintEachEdgeWithIndex();
//    }

    cout << "==================================================" << '\n'
         << "Begin orienting other undirected edges" << endl;

    timer.Start("pc-stable step 3");
    OrientImplied();
    timer.Stop("pc-stable step 3");

//    if (print_struct) {
//        network->PrintEachEdgeWithIndex();
//    }
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
    map<int, map<int, double>> adjacencies_copy = network->adjacencies;
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
    return (FreeDegree(network->adjacencies) > c_depth);
}

/**
 * @brief: check whether a subset Z of adjacency of x can be found,
 *         such that x _||_ y | Z, and the size of Z is "c_depth"
 *         if such a Z can be found, add Z into the sepset of x,y and y,x
 * heuristic: perform a set of the most promising tests for each edge
 *            candidate variables of Z are that have a stronger association with X and Y
 *            lower p-value indicates stronger dependence and stronger association between the variables
 * @return true if such a Z can be found, which means edge x -- y should be deleted
 */
bool PCStable::CheckSide(const map<int, map<int, double>> &adjacencies, int c_depth, Node* x, Node* y) {
    int x_idx = x->GetNodeIndex();
    int y_idx = y->GetNodeIndex();

    //----------------------------- traditional method -----------------------------//
//    set<int> set_adjx;
//    for (auto it = adjacencies.at(x_idx).begin(); it != adjacencies.at(x_idx).end(); ++it) {
//        set_adjx.insert((*it).first);
//    }
//    set_adjx.erase(y_idx);
//    // copy to a vector to access by position, which will be used for choice generating
//    vector<int> vec_adjx;
//    for (const auto &adjx : set_adjx) {
//        vec_adjx.push_back(adjx);
//    }
//
//    if (vec_adjx.size() >= c_depth) {
//        ChoiceGenerator cg (vec_adjx.size(), c_depth);
//
//        vector<int> choice;
//        while (!(choice = cg.Next()).empty()) {
//            set<int> Z;
//            for (int i = 0; i < c_depth; ++i) {
//                Z.insert(vec_adjx[choice[i]]);
//            }
//            num_ci_test++;
//            bool independent = ci_test->IsIndependent(x_idx, y_idx, Z, "g square");
//            if (!independent) {
//                num_dependence_judgement++;
//            } else {
//                // add conditioning set to sepset
//                ci_test->sepset.insert(make_pair(make_pair(x_idx, y_idx), Z));
//                ci_test->sepset.insert(make_pair(make_pair(y_idx, x_idx), Z));
//                return true;
//            }
//        }
//    }
    //---------------------------- traditional method -----------------------------//

    //------------------------------ extra heuristic -------------------------------//
    map<int, double> map_adjx_w(adjacencies.at(x_idx));
    map_adjx_w.erase(y_idx);

    // copy to a vector to access by position, which will be used for choice generating
    vector<pair<int, double>> vec_adjx_w(map_adjx_w.begin(), map_adjx_w.end());

    if (vec_adjx_w.size() >= c_depth) {
        ChoiceGenerator cg (vec_adjx_w.size(), c_depth);

        vector<pair<vector<int>, double>> vec_choice_w;
        vector<int> choice;
        while (!(choice = cg.Next()).empty()) {
            double weight = 0.0;
            for (int i = 0; i < choice.size(); ++i) {
                weight += vec_adjx_w.at(choice[i]).second;
            }
            vec_choice_w.push_back(make_pair(choice, weight));
        }

        sort(vec_choice_w.begin(), vec_choice_w.end(), CmpByValue());

        for (int j = 0; j < vec_choice_w.size(); ++j) {
            set<int> Z;
            for (int i = 0; i < c_depth; ++i) {
                Z.insert(vec_adjx_w.at(vec_choice_w.at(j).first.at(i)).first);
            }
            num_ci_test++;
            bool independent = ci_test->IsIndependent(x_idx, y_idx, Z, "g square");
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
    //------------------------------ extra heuristic -------------------------------//
    return false;
}

/**
 * @return the max |adj(X)\{Y}| for all pairs of nodes (X,Y)
 * I think it is just equal to the max |adj(X)| - 1 for all nodes X
 */
int PCStable::FreeDegree(const map<int, map<int, double>> &adjacencies) {
    int max = 0;
    for (int i = 0; i < network->num_nodes; ++i) {
        max = (adjacencies.at(i).size() > max) ? adjacencies.at(i).size() : max;
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
//        set<int> set_adjacent_nodes = network->adjacencies[b];
        int* adjacent_nodes = new int[network->adjacencies[b].size()];
        int index = 0;
        for (const auto &adj_w : network->adjacencies[b]) {
            adjacent_nodes[index++] = adj_w.first;
        }

        if (network->adjacencies[b].size() < 2) {
            continue;
        }
        ChoiceGenerator cg(network->adjacencies[b].size(), 2); // to find two adjacent nodes of this node and check
        vector<int> combination;

        while (!(combination = cg.Next()).empty()) {
            int a = adjacent_nodes[combination[0]];
            int c = adjacent_nodes[combination[1]];

            // 1) a is not adjacent to c; skip if a is adjacent to c
            if (network->IsAdjacentTo(a, c)) {
                continue;
            }
            // 2) b is not in the sepset of (a, c)
            set<int> sepset = ci_test->sepset[make_pair(a, c)];
            if (sepset.find(b) == sepset.end()) {
                /**
                 * the original code causes problems when adding a new edge generates a circle,
                 * then the added edge will be deleted, but the related deleted edge is not added,
                 * so the total number of edge is changed.
                 */
//                if (network->DeleteDirectedEdge(b, a) || // it means a conflict if DeleteDirectedEdge returns true
//                    network->DeleteUndirectedEdge(a, b)) {
//                    network->AddDirectedEdge(a, b);
//                }
//                if (network->DeleteDirectedEdge(b, c) ||
//                    network->DeleteUndirectedEdge(c, b)) {
//                    network->AddDirectedEdge(c, b);
//                }
                bool deleted_directed1;   // whether the directed edge b->a exists and being deleted
                bool deleted_undirected1; // whether the undirected edge a--b exists and being deleted
                bool to_add1; // whether the directed edge a->b should be added
                bool added1;  // whether the directed edge a->b is successfully added (which means it does not cause a circle)
                bool deleted_directed2;   // whether the directed edge b->c exists and being deleted
                bool deleted_undirected2; // whether the undirected edge c--b exists and being deleted
                bool to_add2; // whether the directed edge c->b should be added
                bool added2;  // whether the directed edge c->b is successfully added (which means it does not cause a circle)

                deleted_directed1 = network->DeleteDirectedEdge(b, a);
                if (!deleted_directed1) { // b->a does not exist, check a--b
                    deleted_undirected1 = network->DeleteUndirectedEdge(a, b);
                } else { // b->a exists and being deleted, a--b cannot exist
                    deleted_undirected1 = false;
                }
                to_add1 = deleted_directed1 | deleted_undirected1;
                deleted_directed2 = network->DeleteDirectedEdge(b, c);
                if (!deleted_directed2) { // b->c does not exist, check c--b
                    deleted_undirected2 = network->DeleteUndirectedEdge(c, b);
                } else { // b->c exists, c--b cannot exist
                    deleted_undirected2 = false;
                }
                to_add2 = deleted_directed2 | deleted_undirected2;

                if (to_add1) { // a->b should be added
                    added1 = network->AddDirectedEdge(a, b);
                } else {
                    added1 = false;
                }
                if (to_add2) { // c->b should be added
                    added2 = network->AddDirectedEdge(c, b);
                } else {
                    added2 = false;
                }

                // if a->b should be added but is not successfully added,
                // find which edge is deleted and then add it back
                if (to_add1 && !added1) {
                    if (deleted_directed1) { // b->a is deleted, then add b->a
                        network->AddDirectedEdge(b, a);
                    } else { // a--b is deleted, then add a--b
                        network->AddUndirectedEdge(a, b);
                    }
                }
                // if c->b should be added but is not successfully added,
                // find which edge is deleted and then add it back
                if (to_add2 && !added2) {
                    if (deleted_directed2) { // b->c is deleted, then add b->c
                        network->AddDirectedEdge(b, c);
                    } else { // c--b is deleted, then add c--b
                        network->AddUndirectedEdge(c, b);
                    }
                }
            }
        }
//        delete [] combination;
        delete [] adjacent_nodes;
    }
}

/**
 * @brief: orient remaining undirected edge as much as possible according to 3 rules
 *      1) if a->b, b--c, and a not adj to c, then b->c (to avoid v-structures)
 *      2) if a->b->c, a--c, then a->c (to avoid circles)
 *      3) if d--a, d--b, d--c, b->a, c->a, b and c are not adjacent, then orient d->a
 *
 * note that for PC-stable, the skeleton is estimated order-independently but not the edge orientations!
 */
void PCStable::OrientImplied() {
    // The initial list of nodes to visit.
//    set<int> visited;

    bool oriented = true;
    /**
     * in each iteration we traverse all undirected edges and try to orient it
     * it stops if none edge is oriented during one iteration
     */
    while (oriented) {
        oriented = false;
        /**
         * note that the for loop does not have "edge_it++", "edge_it++" only happens when (*edge_it) is not erased
         * for the case of erasing (*edge_it), the iterator will point to the next edge after erasing the current edge
         * (erasing operation is in Network::DeleteUndirectedEdge)
         */
        for (auto edge_it = network->vec_edges.begin(); edge_it != network->vec_edges.end();) {
            int x_idx = (*edge_it).GetNode1()->GetNodeIndex();
            int y_idx = (*edge_it).GetNode2()->GetNodeIndex();

            if (network->IsUndirectedFromTo(x_idx, y_idx)) { // if the edge is undirected, check the 3 rules
                if (Rule1(x_idx, y_idx) ||
                    Rule1(y_idx, x_idx) ||
                    Rule2(x_idx, y_idx) ||
                    Rule2(y_idx, x_idx) ||
                    Rule3(x_idx, y_idx) ||
                    Rule3(y_idx, x_idx)) {
                    oriented = true;
                } else { // if the undirected edge x--y remains
                    edge_it++;
                }
            } else { // if the edge is not undirected, go to check the next edge
                edge_it++;
            }
        } // finish checking all edges
    }
}

bool PCStable::Direct(int a, int c) {
    /**
     * the original code causes problems when adding a new edge generates a circle,
     * then the added edge will be deleted, but the related deleted edge is not added,
     * so the total number of edge is changed.
     */
//    if (network->DeleteUndirectedEdge(a, c)) {
//        network->AddDirectedEdge(a, c);
//        return true;
//    }
    bool to_add; // whether the undirected edge a--c exists and being deleted,
                 // which also means whether the directed edge a->c should be added
    bool added;  // whether the directed edge a->c is successfully added (which means it does not cause a circle)

    to_add = network->DeleteUndirectedEdge(a, c);
    if (to_add) { // a->c should be added
        added = network->AddDirectedEdge(a, c);
    } else {
        added = false;
    }

    // if a->c should be added but is not successfully added, add back the undirected edge
    if (to_add && !added) {
        network->AddUndirectedEdge(a, c);
    }

    return added;
}

/**
 * @brief: find common neighbors of x and y
 * @param x_idx index of node x
 * @param y_idx index of node y
 * @return common neighbor set
 */
set<int> PCStable::GetCommonAdjacents(int x_idx, int y_idx) {
//    set<int> adjacent_x = network->adjacencies[x_idx];
//    set<int> adjacent_y = network->adjacencies[y_idx];
//    set<int> common_idx;
    set<int> adjacent_x, adjacent_y, common_idx;
    for (auto it = network->adjacencies[x_idx].begin(); it != network->adjacencies[x_idx].end(); ++it) {
        adjacent_x.insert((*it).first);
    }
    for (auto it = network->adjacencies[y_idx].begin(); it != network->adjacencies[y_idx].end(); ++it) {
        adjacent_y.insert((*it).first);
    }
    set_intersection(adjacent_x.begin(), adjacent_x.end(),
                     adjacent_y.begin(), adjacent_y.end(),
                     inserter(common_idx, common_idx.begin()));
    return common_idx;
}

/**
 * orientation rule1: if a->b, b--c, and a not adj to c, then b->c (to avoid v-structures)
 */
bool PCStable::Rule1(int b_idx, int c_idx) {
    for (const Node* a : network->GetParentPtrsOfNode(b_idx)) { // for every parent a of b
        int a_idx = a->GetNodeIndex();
        if (network->IsAdjacentTo(c_idx, a_idx)) continue; // skip the case if a is adjacent to c
        if (Direct(b_idx, c_idx)) { // then b->c
            return true;
        }
    }
    return false;
}

/**
 * orientation rule2: if a->b->c, a--c, then a->c (to avoid circles)
 */
bool PCStable::Rule2(int a_idx, int c_idx) {
    // get common neighbors of a and c
    set<int> common_idx = GetCommonAdjacents(a_idx, c_idx);

    for (const int &b_idx : common_idx) { // check every common neighbor b of a and c
        if (network->IsDirectedFromTo(a_idx, b_idx) && network->IsDirectedFromTo(b_idx, c_idx)) { // a->b && b->c
            if (Direct(a_idx, c_idx)) { // then a->c
                return true;
            }
        }
        else if (network->IsDirectedFromTo(c_idx, b_idx) && network->IsDirectedFromTo(b_idx, a_idx)) { // c->b && b->a
            if (Direct(c_idx, a_idx)) { // then c->a
                return true;
            }
        }
    }
    return false;
}

/**
 * orientation rule3: if d--a, d--b, d--c, b->a, c->a, b and c are not adjacent, then orient d->a
 */
bool PCStable::Rule3(int d_idx, int a_idx) {
    // get common neighbors of a and d
    set<int> common_idx = GetCommonAdjacents(a_idx, d_idx);

    if (common_idx.size() < 2) {
        return false;
    }
    // a and d has more than or equal to 2 common adjacents
    for (int b_idx = 0; b_idx < common_idx.size(); ++b_idx) {
        for (int c_idx = b_idx + 1; c_idx < common_idx.size(); ++c_idx) {
            // find two adjacents b and c, b and c are not adjacent
            if (!network->IsAdjacentTo(b_idx, c_idx)) {
                if (R3Helper(a_idx, d_idx, b_idx, c_idx)) {
                    return true;
                }
            }
        }
    }
    return false;
}

/**
 * @brief: R3Helper is to check the following 5 edges' direction:
 * d--a, d--b, d--c, b->a, c->a
 * orient d->a if all 5 edges satisfy the condition
 */
bool PCStable::R3Helper(int a_idx, int d_idx, int b_idx, int c_idx) {
    bool oriented = false;

    bool b4 = network->IsUndirectedFromTo(d_idx, a_idx);
    bool b5 = network->IsUndirectedFromTo(d_idx, b_idx);
    bool b6 = network->IsUndirectedFromTo(d_idx, c_idx);
    bool b7 = network->IsDirectedFromTo(b_idx, a_idx);
    bool b8 = network->IsDirectedFromTo(c_idx, a_idx);

    if (b4 && b5 && b6 && b7 && b8) {
        oriented = Direct(d_idx, a_idx);
    }
    return oriented;
}

