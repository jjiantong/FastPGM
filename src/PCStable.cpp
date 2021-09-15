//
// Created by jjt on 2021/6/23.
//

#include "PCStable.h"

PCStable::PCStable(Network *net, double a, int d) {
    network = net;
    num_ci_test = 0;
    num_dependence_judgement = 0;
    depth = d;
    alpha = a;
    timer = new Timer();
}

PCStable::~PCStable() {
    delete network;
    delete timer;
}

void PCStable::StructLearnCompData(Dataset *dts, int group_size, int num_threads, bool print_struct, bool verbose) {
    cout << "==================================================" << '\n'
         << "Begin structural learning with PC-stable" << endl;

    // record time
    timer->Start("pc-stable");
    AssignNodeInformation(dts);
    StructLearnByPCStable(dts, num_threads, group_size, print_struct, verbose);
    timer->Stop("pc-stable");
    setlocale(LC_NUMERIC, "");

    cout << "==================================================" << endl;
    cout << "# of CI-tests is " << num_ci_test << ", # of dependence judgements is " << num_dependence_judgement << endl;
    timer->Print("pc-stable");
    timer->Print("pc-stable step 1"); cout << " (" << timer->time["pc-stable step 1"] / timer->time["pc-stable"] * 100 << "%)";
    timer->Print("pc-stable step 2"); cout << " (" << timer->time["pc-stable step 2"] / timer->time["pc-stable"] * 100 << "%)";
    timer->Print("pc-stable step 3"); cout << " (" << timer->time["pc-stable step 3"] / timer->time["pc-stable"] * 100 << "%)";

    timer->Print("new & delete"); cout << " (" << timer->time["new & delete"] / timer->time["pc-stable step 1"] * 100 << "%)";
    timer->Print("config + count"); cout << " (" << timer->time["config + count"] / timer->time["pc-stable step 1"] * 100 << "%)";
    timer->Print("marginals"); cout << " (" << timer->time["marginals"] / timer->time["pc-stable step 1"] * 100 << "%)";
    timer->Print("g2 & df + p value"); cout << " (" << timer->time["g2 & df + p value"] / timer->time["pc-stable step 1"] * 100 << "%)" << endl;
}

void PCStable::StructLearnByPCStable(Dataset *dts, int num_threads, int group_size, bool print_struct, bool verbose) {
    timer->Start("pc-stable step 1");
    cout << "==================================================" << '\n'
         << "Generating complete undirected graph" << endl;

    for (int i = 0; i < network->num_nodes; ++i) {
        for (int j = i + 1; j < network->num_nodes; ++j) {
            network->AddUndirectedEdge(i, j);
        }
    }

    cout << "==================================================" << '\n'
         << "Begin finding the skeleton" << endl << "Level 0... " << endl;

    for (int i = 0; i < network->num_nodes; ++i) { // find neighbor set of each node i
        map<int, double> adjacency;
        for (int j = 0; j < network->num_nodes; ++j) { // all nodes except for i itself are neighbors of i
            if (i == j)
                continue;
            adjacency.insert(make_pair(j, 1.0));
        }
        network->adjacencies.insert(make_pair(i, adjacency));
    }

    for (int i = 0; i < network->num_edges; ++i) {
        int node_idx1 = network->vec_edges[i].GetNode1()->GetNodeIndex();
        int node_idx2 = network->vec_edges[i].GetNode2()->GetNodeIndex();
        set<int> empty_set;

        if (verbose) {
            cout << "--------------------------------------------------" << endl
                 << "* investigating " << network->FindNodePtrByIndex(node_idx1)->node_name
                 << " -- " << network->FindNodePtrByIndex(node_idx2)->node_name
                 << ", conditioning sets of size 0." << endl;
        }

        num_ci_test++;
        IndependenceTest *ci_test = new IndependenceTest(dts, alpha);
        IndependenceTest::Result result = ci_test->IndependenceResult(node_idx1, node_idx2, vector<int>(),
                                                                      "g square", timer);
        delete ci_test;
        bool independent = result.is_independent;
        if (verbose) {
            cout << "    > node " << network->FindNodePtrByIndex(node_idx1)->node_name << " is ";
            if (independent) {
                cout << "independent";
            } else {
                cout << "dependent";
            }
            cout << " on " << network->FindNodePtrByIndex(node_idx2)->node_name;
            cout << " (p-value: " << result.p_value << ")." << endl;
        }

        if (!independent) { // the edge remains
            num_dependence_judgement++;
            //-------------------------------- heuristic ---------------------------------//
            // store the p value - smaller means a stronger association
//            network->adjacencies[node_idx1][node_idx2] = result.p_value;
//            network->adjacencies[node_idx2][node_idx1] = result.p_value;
            //-------------------------------- heuristic ---------------------------------//
        } else {
            network->vec_edges[i].need_remove = true;
            // the edge node1 -- node2 should be removed
            // 1. delete the edge 2. remove each other from adjacency set
            // (1 and 2 will be done in the loop below)
            // 3. add conditioning set (an empty set for level 0) to sepset
            sepset.insert(make_pair(make_pair(node_idx1, node_idx2), empty_set));
//            sepset.insert(make_pair(make_pair(node_idx2, node_idx1), empty_set));
        }
    }

    for (int i = 0; i < network->num_edges; ++i) {
        int node_idx1 = network->vec_edges[i].GetNode1()->GetNodeIndex();
        int node_idx2 = network->vec_edges[i].GetNode2()->GetNodeIndex();

        if (network->vec_edges[i].need_remove) {
            // the edge node1 -- node2 should be removed
            // 1. delete the edge
            // note that using "DeleteUndirectedEdge(node_idx1, node_idx2)" will cause some (slight) extra computations
            network->vec_edges.erase(network->vec_edges.begin() + i);
            --network->num_edges;
            // 2. remove each other from adjacency set
            network->adjacencies[node_idx1].erase(node_idx2);
            network->adjacencies[node_idx2].erase(node_idx1);
            // 3. add conditioning set (an empty set for level 0) to sepset (is done in the loop above)
            i--;
        }
    }

    if (verbose) {
        cout << "* remaining edges:" << endl;
        network->PrintEachEdgeWithName();
    }

    for (int d = 1; d < depth; ++d) {
        cout << "Level " << d << "... " << endl;

        bool more = SearchAtDepth(dts, d, num_threads, group_size, verbose);

        if (verbose) {
            cout << "* remaining edges:" << endl;
            network->PrintEachEdgeWithName();
        }

        if (!more) {
            break;
        }
    }

    timer->Stop("pc-stable step 1");

    cout << "\n==================================================" << '\n'
         << "Begin orienting v-structure" << endl;

    timer->Start("pc-stable step 2");
    OrientVStructure();
    timer->Stop("pc-stable step 2");


    cout << "==================================================" << '\n'
         << "Begin orienting other undirected edges" << endl;

    timer->Start("pc-stable step 3");
    OrientImplied();
    timer->Stop("pc-stable step 3");

    if (print_struct) {
        cout << endl;
//        network->PrintEachEdgeWithIndex();
        network->PrintEachEdgeWithName();
    }
}

/**
 * @brief: search for each level (c_depth, c_depth > 0) except for level 0
 * use a stack "stack_edge_id" to store all edges in this level
 * each time we pop "num_threads" edges (or less for the last times) and handle them (in parallel)
 * according to the results, some of the edges will push back again and some will not
 * repeat this process until the stack is empty, which means all edges in this level have been finished
 */
bool PCStable::SearchAtDepth(Dataset *dts, int c_depth, int num_threads, int group_size, bool verbose) {
    /**
     * the copied adjacency sets of all nodes are used and kept unchanged at each particular level c_depth
     * consequently, an edge deletion at one level does not affect the conditioning sets of the other nodes
     * and thus the output is independent with the variable ordering, called PC-stable
     */
    map<int, map<int, double>> adjacencies_copy = network->adjacencies;

    // push all edges into stack
    stack<int> stack_edge_id;
    for (int i = network->num_edges - 1; i >= 0; --i) {
        stack_edge_id.push(i);
    }

    int *processing_edge_id = new int[num_threads];
    while (stack_edge_id.size() >= num_threads) {
        // pop 8 edges at one time
        for (int i = 0; i < num_threads; ++i) {
            processing_edge_id[i] = stack_edge_id.top();
            stack_edge_id.pop();
        }

//#pragma omp parallel for num_threads(num_threads)
        for (int i = 0; i < num_threads; ++i) {
            CheckEdge(dts, adjacencies_copy, c_depth, processing_edge_id[i], group_size, verbose);
        }
        for (int i = num_threads - 1; i >= 0; --i) {
            if (network->vec_edges[processing_edge_id[i]].need_to_push == true) {
                // if this edge has not been finished, push back to the stack
                stack_edge_id.push(processing_edge_id[i]);
            }
        }
    }
    while (!stack_edge_id.empty()) { // have 1~7 left
        int size = stack_edge_id.size();
        for (int i = 0; i < size; ++i) {
            processing_edge_id[i] = stack_edge_id.top();
            stack_edge_id.pop();
        }
        for (int i = 0; i < size; ++i) {
            CheckEdge(dts, adjacencies_copy, c_depth, processing_edge_id[i], group_size, verbose);
        }
        for (int i = size - 1; i >= 0; --i) {
            if (network->vec_edges[processing_edge_id[i]].need_to_push == true) {
                // if this edge has not been finished, push back to the stack
                stack_edge_id.push(processing_edge_id[i]);
            }
        }
    }
    delete[] processing_edge_id;
    processing_edge_id = nullptr;

    for (int i = 0; i < network->num_edges; ++i) {
        int node_idx1 = network->vec_edges[i].GetNode1()->GetNodeIndex();
        int node_idx2 = network->vec_edges[i].GetNode2()->GetNodeIndex();

        if (network->vec_edges[i].need_remove) {
            // the edge x -- y should be removed
            // 1. delete the edge
            // note that using "DeleteUndirectedEdge(x->GetNodeIndex(), y->GetNodeIndex())" will cause some (slight) extra computations
            network->vec_edges.erase(network->vec_edges.begin() + i);
            --network->num_edges;
            // 2. remove each other from adjacency set
            network->adjacencies[node_idx1].erase(node_idx2);
            network->adjacencies[node_idx2].erase(node_idx1);
            // 3. add conditioning set (an empty set for level 0) to sepset (is done in the loop above)
            i--;
        }
    }

    return (FreeDegree(network->adjacencies) > c_depth);
}

/**
 * handle one edge
 * the core is to first construct a "cg" for this edge if necessary,
 *                then call the function to handle a group of ci tests for this edge
 * according to the results, determine whether the edge needs to be removed
 *                                             the edge has been finished handling
 *                                             the edge needs to be pushed back to the stack
 */
void PCStable::CheckEdge(Dataset *dts, const map<int, map<int, double>> &adjacencies, int c_depth,
                         int edge_id, int group_size, bool verbose) {
    int x_idx = network->vec_edges[edge_id].GetNode1()->GetNodeIndex();
    int y_idx = network->vec_edges[edge_id].GetNode2()->GetNodeIndex();

    if (verbose) {
        cout << "--------------------------------------------------" << endl
             << "* investigating " << network->vec_edges[edge_id].GetNode1()->node_name << " -- "
             << network->vec_edges[edge_id].GetNode2()->node_name << ", conditioning sets of size " << c_depth << "." << endl;
    }

    if (network->vec_edges[edge_id].process == NO) {
        /**
         * case 1: this edge have not been processed
         * a. get the neighbors of node1 and store them in "vec_adj"
         * b. construct cg to get ready for choice generation, set "process" = NODE1
         *   c. if the number of neighbors of node1 is less than c_depth, do a and b for node2
         *     d. if the number of neighbors of node2 is less than c_depth:
         *          no need to push, no need to remove, process = NO
         */
        // get the neighbors of node x
        int num_adj = FindAdjacencies(dts, adjacencies, edge_id, x_idx, y_idx);

        // prepare to generate choice and set "process" = NODE1
        if (num_adj >= c_depth) {
            network->vec_edges[edge_id].cg = new ChoiceGenerator(num_adj, c_depth);
            network->vec_edges[edge_id].process = NODE1;
        } else {
            // get the neighbors of node y
            int num_adj = FindAdjacencies(dts, adjacencies, edge_id, y_idx, x_idx);

            // prepare to generate choice and set "process" = NODE2
            if (num_adj >= c_depth) {
                network->vec_edges[edge_id].cg = new ChoiceGenerator(num_adj, c_depth);
                network->vec_edges[edge_id].process = NODE2;
            } else {
                network->vec_edges[edge_id].need_to_push = false;
                network->vec_edges[edge_id].need_remove  = false;
                network->vec_edges[edge_id].process = NO;
                return;
            }
        }
    } else if (network->vec_edges[edge_id].process == ENODE1) {
        /**
         * case 2: this edge has finished processing node1
         * a. get the neighbors of node2 and store them in "vec_adj"
         * b. construct cg to get ready for choice generation, set "process" = NODE2
         *   c. if the number of neighbors of node2 is less than c_depth:
         *          no need to push, no need to remove, process = NO
         */
        // get the neighbors of node y
        int num_adj = FindAdjacencies(dts, adjacencies, edge_id, y_idx, x_idx);

        // prepare to generate choice and set "process" = NODE2
        if (num_adj >= c_depth) {
            network->vec_edges[edge_id].cg = new ChoiceGenerator(num_adj, c_depth);
            network->vec_edges[edge_id].process = NODE2;
        } else {
            network->vec_edges[edge_id].need_to_push = false;
            network->vec_edges[edge_id].need_remove  = false;
            network->vec_edges[edge_id].process = NO;
            return;
        }
    }

    /**
     * do the next "group_size" ci tests
     * if independent: delete "cg", need remove, no need to push
     * if dependent:
     *      if finish = false: need to push
     *      if finish = true, process = NODE1: delete "cg", need to push, process = ENODE1
     *      if finish = true, process = NODE2: delete "cg", no need remove, no need to push, process = NO
     */
    bool ind = Testing(dts, c_depth, edge_id, x_idx, y_idx, group_size, verbose);

    if (ind) {
        delete network->vec_edges[edge_id].cg;
        network->vec_edges[edge_id].cg = nullptr;
        network->vec_edges[edge_id].need_remove  = true;
        network->vec_edges[edge_id].need_to_push = false;
    } else {
        if (!network->vec_edges[edge_id].finish) {
            network->vec_edges[edge_id].need_to_push = true;
        } else {
            if (network->vec_edges[edge_id].process == NODE1) {
                delete network->vec_edges[edge_id].cg;
                network->vec_edges[edge_id].cg = nullptr;
                network->vec_edges[edge_id].need_to_push = true;
                network->vec_edges[edge_id].process = ENODE1;
            } else {
                delete network->vec_edges[edge_id].cg;
                network->vec_edges[edge_id].cg = nullptr;
                network->vec_edges[edge_id].need_remove  = false;
                network->vec_edges[edge_id].need_to_push = false;
                network->vec_edges[edge_id].process = NO;
            }
        }
    }
}

/**
 * find the adjacent nodes of node "x_idx" except for node "y_idx"
 * store them in its "vec_adj" and return the number of its adjacent nodes
 */
int PCStable::FindAdjacencies(Dataset *dts, const map<int, map<int, double>> &adjacencies, int edge_id, int x_idx, int y_idx) {
    // get the neighbors of node x
    set<int> set_adjx;
    for (auto it = adjacencies.at(x_idx).begin(); it != adjacencies.at(x_idx).end(); ++it) {
        set_adjx.insert((*it).first);
    }
    set_adjx.erase(y_idx);

    // copy to a vector to access by position, which will be used for choice generating
    network->vec_edges[edge_id].vec_adj.clear();
    network->vec_edges[edge_id].vec_adj.reserve(set_adjx.size());
    for (const auto &adjx : set_adjx) {
        network->vec_edges[edge_id].vec_adj.push_back(adjx);
    }
    return set_adjx.size();
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
bool PCStable::Testing(Dataset *dts, int c_depth, int edge_idx, int x_idx, int y_idx, int group_size, bool verbose) {
    // fetch multiple ci tests at one time
    vector<vector<int>> choices = network->vec_edges[edge_idx].cg->NextN(group_size);

    if (!choices[0].empty()) { // the first is not empty means we need to test this group
        // vector Z contains a group of conditioning set elements
        vector<int> Z;
        Z.reserve(group_size * c_depth);
        int i;
        for (i = 0; i < group_size; ++i) {
            if (!choices[i].empty()) {
                for (int j = 0; j < c_depth; ++j) {
                    Z.push_back(network->vec_edges[edge_idx].vec_adj[choices[i][j]]);
                }
            } else {
                break;
            }
        } // i means the number of valid conditioning set in this group when existing the loop
        // i != group_size (i < group_size) only when for the last group of the edge

        num_ci_test += i;
        IndependenceTest *ci_test = new IndependenceTest(dts, alpha);
        IndependenceTest::Result result = ci_test->IndependenceResult(x_idx, y_idx, Z, "g square", timer, i);
        delete ci_test;
        bool independent = result.is_independent;

        if (independent) {
            int first_id = result.first; // get the first independent one

            int node_idx1, node_idx2;
            if (x_idx > y_idx) {
                node_idx1 = y_idx;
                node_idx2 = x_idx;
            } else {
                node_idx1 = x_idx;
                node_idx2 = y_idx;
            }
            set<int> conditioning_set;
            for (int j = 0; j < c_depth; ++j) {
                conditioning_set.insert(network->vec_edges[edge_idx].vec_adj[choices[first_id][j]]);
            }
            sepset.insert(make_pair(make_pair(node_idx1, node_idx2), conditioning_set));

            if (verbose) {
                cout << "    **** finish this group: ";
                cout << "node " << network->FindNodePtrByIndex(x_idx)->node_name << " is independent";
                cout << " on " << network->FindNodePtrByIndex(y_idx)->node_name << " given ";
                for (const auto &z_idx : conditioning_set) {
                    cout << network->FindNodePtrByIndex(z_idx)->node_name << " ";
                }
                cout << "(group id = " << first_id << ")." << endl;
            }
            return true;
        }

        // dependent
        if (verbose) {
            cout << "    **** finish this group: ";
            cout << "node " << network->FindNodePtrByIndex(x_idx)->node_name << " is dependent";
            cout << " on " << network->FindNodePtrByIndex(y_idx)->node_name << " in this group." << endl;
        }

        if (i == group_size) {
            network->vec_edges[edge_idx].finish = false;
        } else {
            network->vec_edges[edge_idx].finish = true;
        }
        return false;
    }

    network->vec_edges[edge_idx].finish = true;
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
        vector<int> vec_adjacent_nodes;
        for (const auto &adj_w : network->adjacencies[b]) {
            vec_adjacent_nodes.push_back(adj_w.first);
        }

        if (network->adjacencies[b].size() < 2) {
            continue;
        }
        ChoiceGenerator cg(network->adjacencies[b].size(), 2); // to find two adjacent nodes of this node and check
        vector<int> combination;

        while (!(combination = cg.Next()).empty()) {
            int a = vec_adjacent_nodes[combination[0]];
            int c = vec_adjacent_nodes[combination[1]];

            // 1) a is not adjacent to c; skip if a is adjacent to c
            if (network->IsAdjacentTo(a, c)) {
                continue;
            }
            // 2) b is not in the sepset of (a, c)
            set<int> sepset = this->sepset[make_pair(a, c)];
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

