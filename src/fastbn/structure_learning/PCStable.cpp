//
// Created by jjt on 2021/6/23.
//

#include "fastbn/structure_learning/PCStable.h"

PCStable::PCStable(Network *net, double a, int d) {
    network = net;
    num_ci_test = 0;
    depth = d;
    alpha = a;
}

PCStable::~PCStable() {
    SAFE_DELETE(network);
}

/**
 * Reference:
 *      Jiang, Jiantong, Zeyi Wen, and Ajmal Mian.
 *      "Fast parallel Bayesian network structure learning."
 *      (IPDPS 2022).
 * implements the algorithm Fast-BNS for structure learning as described in the paper.
 */
void PCStable::StructLearnCompData(Dataset *dts, int group_size, int num_threads,
                                   bool dag, bool add_root, bool save_struct, string struct_file, int verbose) {
    if (verbose > 0) {
        cout << "--------------------------------------------------" << '\n'
             << "Structure learning with PC-Stable..." << endl;
    }

    Timer *timer = new Timer();
    // record time
    timer->Start("pc-stable");
    AssignNodeInformation(dts);
    StructLearnByPCStable(dts, num_threads, group_size, dag, timer, verbose);
    timer->Stop("pc-stable");

    if (add_root) {
        cout << "--------------------------------------------------" << '\n'
             << "Handling multiple independent sub-graphs..." << endl;
        cout << "Finding roots for sub-graphs and adding the ROOT... " << endl;
        vector<int> roots = FindRootsInDAGForest();
        AddRootNode(roots);
        cout << "roots: ";
        for (int i = 0; i < roots.size(); ++i) {
            cout << roots[i] << ", ";
        }
        cout << endl;
    }
    if (save_struct) {
        SaveBNStructure(struct_file);
    }

    if (verbose > 0) {
        cout << "--------------------------------------------------" << '\n';
        cout << "# of CI-tests = " << num_ci_test << endl;
        timer->Print("pc-stable step 0");
        cout << " (" << timer->time["pc-stable step 0"] / timer->time["pc-stable"] * 100 << "%)" << endl;
        timer->Print("pc-stable step 1");
        cout << " (" << timer->time["pc-stable step 1"] / timer->time["pc-stable"] * 100 << "%)" << endl;
//        timer->Print("pc-stable step 2");
//        cout << " (" << timer->time["pc-stable step 2"] / timer->time["pc-stable"] * 100 << "%)" << endl;
//        timer->Print("pc-stable step 3");
//        cout << " (" << timer->time["pc-stable step 3"] / timer->time["pc-stable"] * 100 << "%)" << endl;
//
//        timer->Print("cg"); cout << " (" << timer->time["cg"] / timer->time["pc-stable step 1"] * 100 << "%)" << endl;
//        timer->Print("ci"); cout << " (" << timer->time["ci"] / timer->time["pc-stable step 1"] * 100 << "%)" << endl;
//
//        timer->Print("new & delete");
//        cout << " (" << timer->time["new & delete"] / timer->time["pc-stable step 1"] * 100 << "%)" << endl;
//        timer->Print("config + count");
//        cout << " (" << timer->time["config + count"] / timer->time["pc-stable step 1"] * 100 << "%)" << endl;
//        timer->Print("marginals");
//        cout << " (" << timer->time["marginals"] / timer->time["pc-stable step 1"] * 100 << "%)" << endl;
//        timer->Print("g2 & df + p value");
//        cout << " (" << timer->time["g2 & df + p value"] / timer->time["pc-stable step 1"] * 100 << "%)" << endl;
    }
    timer->Print("pc-stable");

    SAFE_DELETE(timer);
}

void PCStable::StructLearnByPCStable(Dataset *dts, int num_threads, int group_size, bool dag,
                                     Timer *timer, int verbose) {

    if (verbose > 0) {
        cout << "--------------------------------------------------" << '\n'
             << "Generating undirected complete graph..." << endl;
    }

    timer->Start("pc-stable step 0");
    network->GenerateUndirectedCompleteGraph();
    /**
     * it takes quite a long time for the original implementation
     */
//    for (int i = 0; i < network->num_nodes; ++i) {
//        for (int j = i + 1; j < network->num_nodes; ++j) {
//            network->AddUndirectedEdge(i, j);
//        }
//    }
    timer->Stop("pc-stable step 0");

    if (verbose > 0) {
        cout << "--------------------------------------------------" << '\n'
             << "Removing edges level by level..." << endl << "Level 0... " << endl;
    }

    timer->Start("pc-stable step 1");

    /**
     * you can simply ignore this part as this part is not used in the current implementation.
     * key is node index, value is a map, contains the set of neighbor node indexes of the node and their weights. the
     * weight refers to the strength of association between the node and its neighbor, which is in fact the p-value
     * obtained from the level 0 of PC alg. step 1, smaller weight means stronger association. however, the strength is
     * not used in the current implementation.
     */
    map<int, map<int, double>> adjacencies;
    for (int i = 0; i < network->num_nodes; ++i) { // find neighbor set of each node i
        map<int, double> adjacency;
        for (int j = 0; j < network->num_nodes; ++j) { // all nodes except for i itself are neighbors of i
            if (i == j)
                continue;
            adjacency.insert(make_pair(j, 1.0));
        }
        adjacencies.insert(make_pair(i, adjacency));
    }

    /**
     * handles level (depth) 0. since the conditioning set is an empty set, only one CI test is required for each edge,
     * which is a marginal independence test. thus, we don't need the CI-level parallelism in level (depth) 0.
     */
#pragma omp parallel for num_threads(num_threads)
    for (int i = 0; i < network->num_edges; ++i) {
        int node_idx1 = network->vec_edges[i].GetNode1()->GetNodeIndex();
        int node_idx2 = network->vec_edges[i].GetNode2()->GetNodeIndex();

        if (verbose > 1) {
            cout << "--------------------------------------------------" << endl
                 << "* investigating " << network->FindNodePtrByIndex(node_idx1)->node_name
                 << " -- " << network->FindNodePtrByIndex(node_idx2)->node_name
                 << ", conditioning sets of size 0." << endl;
        }

        num_ci_test++;
        IndependenceTest *ci_test = new IndependenceTest(dts, alpha);
        IndependenceTest::Result result = ci_test->IndependenceResult(node_idx1, node_idx2, vector<int>(),
                                                                      "g square", timer);
        SAFE_DELETE(ci_test);
        bool independent = result.is_independent;
        if (verbose > 1) {
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
            //-------------------------------- heuristic ---------------------------------//
            // store the p value - smaller means a stronger association
//            adjacencies[node_idx1][node_idx2] = result.p_value;
//            adjacencies[node_idx2][node_idx1] = result.p_value;
            //-------------------------------- heuristic ---------------------------------//
        } else {
            network->vec_edges[i].need_remove = true;
            // the edge node1 -- node2 should be removed
            // 1. delete the edge 2. remove each other from adjacency set
            // (1 and 2 will be done in the loop below)
            // 3. add conditioning set (an empty set for level 0) to sepset
            sepset.insert(make_pair(make_pair(node_idx1, node_idx2), set<int>()));
//            sepset.insert(make_pair(make_pair(node_idx2, node_idx1), set<int>()));
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
            adjacencies[node_idx1].erase(node_idx2);
            adjacencies[node_idx2].erase(node_idx1);
            // 3. add conditioning set (an empty set for level 0) to sepset (is done in the loop above)
            i--;
        }
    }

    if (verbose > 1) {
        cout << "* remaining edges:" << endl;
        network->PrintEdges();
    }

    double tmp = omp_get_wtime();
    if (verbose > 0) {
        cout << "# of CI-tests is " << num_ci_test << endl;
        cout << "# remaining edges = " << network->num_edges
             << ", time = " << tmp - timer->start_time["pc-stable step 1"] << endl;
    }

    for (int d = 1; d < depth; ++d) {
        if (verbose > 0) {
            cout << "Level " << d << "... " << endl;
        }

        bool more = SearchAtDepth(dts, d, num_threads, adjacencies, timer, group_size, verbose);

        if (verbose > 1) {
            cout << "* remaining edges:" << endl;
            network->PrintEdges();
        }

        double tmp = omp_get_wtime();
        if (verbose > 0) {
            cout << "# of CI-tests is " << num_ci_test << endl;
            cout << "# remaining edges = " << network->num_edges
                 << ", time = " << tmp - timer->start_time["pc-stable step 1"] << endl;
        }

        if (!more) {
            break;
        }
    }
    timer->Stop("pc-stable step 1");

    if (verbose > 0) {
        cout << "--------------------------------------------------" << '\n'
             << "Orienting v-structure" << endl;
    }

//    timer->Start("pc-stable step 2");
    OrientVStructure(adjacencies);
//    timer->Stop("pc-stable step 2");


    if (verbose > 0) {
        cout << "--------------------------------------------------" << '\n'
             << "Orienting other undirected edges" << endl;
    }

//    timer->Start("pc-stable step 3");
    OrientImplied(adjacencies);
//    timer->Stop("pc-stable step 3");

    if (dag) {
        DirectLeftEdges(adjacencies);
    }
}

/**
 * @brief: search for each level (c_depth, c_depth > 0) except for level 0
 * use a stack "stack_edge_id" to store all edges in this level
 * each time we pop "num_threads" edges (or less for the last times) and handle them (in parallel)
 * according to the results, some of the edges will push back again and some will not
 * repeat this process until the stack is empty, which means all edges in this level have been finished
 */
bool PCStable::SearchAtDepth(Dataset *dts, int c_depth, int num_threads, map<int, map<int, double>> &adjacencies,
                             Timer *timer, int group_size, int verbose) {
    /**
     * the copied adjacency sets of all nodes are used and kept unchanged at each particular level c_depth
     * consequently, an edge deletion at one level does not affect the conditioning sets of the other nodes
     * and thus the output is independent with the variable ordering, called PC-stable
     */
    map<int, map<int, double>> adjacencies_copy = adjacencies;

    // push all edges into stack
    stack<int> stack_edge_id;
    for (int i = network->num_edges - 1; i >= 0; --i) {
        network->vec_edges[i].process = NO;
        stack_edge_id.push(i);
    }

    int processing_edge_id[128];
    bool need_to_push[128];
    while (stack_edge_id.size() >= 128) {
        // pop 128 edges at one time
        for (int i = 0; i < 128; ++i) {
            processing_edge_id[i] = stack_edge_id.top();
            stack_edge_id.pop();
        }

#pragma omp parallel for num_threads(num_threads)
        for (int i = 0; i < 128; ++i) {
            need_to_push[i] = CheckEdge(dts, adjacencies_copy, c_depth, processing_edge_id[i],
                                        timer, group_size, verbose);
        }

        for (int i = 127; i >= 0; --i) {
            if (need_to_push[i]) {
                // if this edge has not been finished, push back to the stack
                stack_edge_id.push(processing_edge_id[i]);
            }
        }
    }

    while (!stack_edge_id.empty()) { // have 1~127 left
        int size = stack_edge_id.size();
        for (int i = 0; i < size; ++i) {
            processing_edge_id[i] = stack_edge_id.top();
            stack_edge_id.pop();
        }

        int p = (size > num_threads) ? num_threads : size;
#pragma omp parallel for num_threads(p)
        for (int i = 0; i < size; ++i) {
            need_to_push[i] = CheckEdge(dts, adjacencies_copy, c_depth, processing_edge_id[i],
                                        timer, group_size, verbose);
        }

        for (int i = size - 1; i >= 0; --i) {
            if (need_to_push[i]) {
                // if this edge has not been finished, push back to the stack
                stack_edge_id.push(processing_edge_id[i]);
            }
        }
    }

    /**------------------- using task (with bugs), and the sequential while-loop version -------------------**/
//#pragma omp parallel num_threads(4)
//#pragma omp single
//    {
//        while (!stack_edge_id.empty()) {
//            // pop one edge
//            int processing_edge_id = stack_edge_id.top();
//            stack_edge_id.pop();
//            cout << omp_get_thread_num() << ": pop edge " << processing_edge_id << endl;
//
//#pragma omp task
//            {
//                cout << omp_get_thread_num() << ": begin testing edge " << processing_edge_id
//                     << ", process = " << network->vec_edges[processing_edge_id].process << endl;
//                // handle one edge
//                bool push = CheckEdge(dts, adjacencies_copy, c_depth, processing_edge_id, group_size, verbose);
//                cout << omp_get_thread_num() << ": end testing edge " << processing_edge_id << endl;
//
//                if (push) {
//                    // if this edge has not been finished, push back to the stack
//#pragma omp critical
//                    stack_edge_id.push(processing_edge_id);
//                    cout << omp_get_thread_num() << ": push edge " << processing_edge_id << endl;
//                }
//            }
//        }
//    }

//    while (!stack_edge_id.empty()) {
//        // pop one edge
//        int processing_edge_id = stack_edge_id.top();
//        stack_edge_id.pop();
//
//        // handle one edge
//        bool push = CheckEdge(dts, adjacencies_copy, c_depth, processing_edge_id, group_size, verbose);
//
//        if (push) {
//            // if this edge has not been finished, push back to the stack
//            stack_edge_id.push(processing_edge_id);
//        }
//    }
    /**------------------- using task (with bugs), and the sequential while-loop version -------------------**/

    for (int i = 0; i < network->num_edges; ++i) {
        int node_idx1 = network->vec_edges[i].GetNode1()->GetNodeIndex();
        int node_idx2 = network->vec_edges[i].GetNode2()->GetNodeIndex();

        if (network->vec_edges[i].need_remove) {
            // the edge x -- y should be removed
            // 1. delete the edge
            // note that using "DeleteUndirectedEdge(x->GetNodeIndex(), y->GetNodeIndex())"
            // will cause some (slight) extra computations
            network->vec_edges.erase(network->vec_edges.begin() + i);
            --network->num_edges;
            // 2. remove each other from adjacency set
            adjacencies[node_idx1].erase(node_idx2);
            adjacencies[node_idx2].erase(node_idx1);
            // 3. add conditioning set (an empty set for level 0) to sepset (is done in the loop above)
            i--;
        }
    }
    return (FreeDegree(adjacencies) > c_depth);
}

/**
 * handle one edge
 * the core is to first construct a "cg" for this edge if necessary,
 *                then call the function to handle a group of ci tests for this edge
 * according to the results, determine whether the edge needs to be removed
 *                                             the edge has been finished handling
 *                                             the edge needs to be pushed back to the stack
 * @return whether the edge need to be pushed or not
 */
bool PCStable::CheckEdge(Dataset *dts, const map<int, map<int, double>> &adjacencies, int c_depth,
                         int edge_id, Timer *timer, int group_size, int verbose) {
    int x_idx = network->vec_edges[edge_id].GetNode1()->GetNodeIndex();
    int y_idx = network->vec_edges[edge_id].GetNode2()->GetNodeIndex();

    if (verbose > 1) {
        cout << "--------------------------------------------------" << endl
             << "* investigating " << network->vec_edges[edge_id].GetNode1()->node_name << " -- "
             << network->vec_edges[edge_id].GetNode2()->node_name << ", conditioning sets of size "
             << c_depth << "." << endl;
    }

//    timer->Start("cg");
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
        int num_adj = FindAdjacencies(adjacencies, edge_id, x_idx, y_idx);

        // prepare to generate choice and set "process" = NODE1
        if (num_adj >= c_depth) {
            network->vec_edges[edge_id].cg = new ChoiceGenerator(num_adj, c_depth);
            network->vec_edges[edge_id].process = NODE1;
        } else {
            // get the neighbors of node y
            int num_adj = FindAdjacencies(adjacencies, edge_id, y_idx, x_idx);

            // prepare to generate choice and set "process" = NODE2
            if (num_adj >= c_depth) {
                network->vec_edges[edge_id].cg = new ChoiceGenerator(num_adj, c_depth);
                network->vec_edges[edge_id].process = NODE2;
            } else {
                network->vec_edges[edge_id].need_remove = false;
                return false;
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
        int num_adj = FindAdjacencies(adjacencies, edge_id, y_idx, x_idx);

        // prepare to generate choice and set "process" = NODE2
        if (num_adj >= c_depth) {
            network->vec_edges[edge_id].cg = new ChoiceGenerator(num_adj, c_depth);
            network->vec_edges[edge_id].process = NODE2;
        } else {
            network->vec_edges[edge_id].need_remove = false;
            return false;
        }
    }
//    timer->Stop("cg");

//    timer->Start("ci");
    /**
     * do the next "group_size" ci tests
     * if independent: delete "cg", need remove, no need to push
     * if dependent:
     *      if finish = false: need to push
     *      if finish = true, process = NODE1: delete "cg", need to push, process = ENODE1
     *      if finish = true, process = NODE2: delete "cg", no need remove, no need to push, process = NO
     */
    bool ind = Testing(dts, c_depth, edge_id, x_idx, y_idx, timer, group_size, verbose);
//    timer->Stop("ci");

    if (ind) {
        SAFE_DELETE(network->vec_edges[edge_id].cg);
        network->vec_edges[edge_id].need_remove = true;
        return false;
    } else {
        if (!network->vec_edges[edge_id].finish) {
            return true;
        } else {
            if (network->vec_edges[edge_id].process == NODE1) {
                SAFE_DELETE(network->vec_edges[edge_id].cg);
                network->vec_edges[edge_id].process = ENODE1;
                return true;
            } else {
                SAFE_DELETE(network->vec_edges[edge_id].cg);
                network->vec_edges[edge_id].need_remove  = false;
                return false;
            }
        }
    }
}

/**
 * find the adjacent nodes of node "x_idx" except for node "y_idx"
 * store them in its "vec_adj" and return the number of its adjacent nodes
 */
int PCStable::FindAdjacencies(const map<int, map<int, double>> &adjacencies, int edge_id, int x_idx, int y_idx) {
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
bool PCStable::Testing(Dataset *dts, int c_depth, int edge_idx, int x_idx, int y_idx,
                       Timer *timer, int group_size, int verbose) {
//    cout << omp_get_thread_num() << ": current choice: ";
//    if (!network->vec_edges[edge_idx].cg.choice.empty()) {
//        for (int i = 0; i < c_depth; ++i) {
//            cout << network->vec_edges[edge_idx].cg.choice[i] << " ";
//        }
//        cout << endl;
//    } else {
//        cout << "no " << endl;
//    }

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
//                    cout << omp_get_thread_num() << ": edge " << edge_idx
//                         << ", choice " << choices[i][j]
//                         << ", adja " << network->vec_edges[edge_idx].vec_adj[choices[i][j]] << endl;
                    Z.push_back(network->vec_edges[edge_idx].vec_adj[choices[i][j]]); // TODO
//                    cout << omp_get_thread_num() << ": pushed to z " << endl;
                }
            } else {
                break;
            }
        } // i means the number of valid conditioning set in this group when existing the loop
        // i != group_size (i < group_size) only when for the last group of the edge

        num_ci_test += i;
        IndependenceTest *ci_test = new IndependenceTest(dts, alpha);
        IndependenceTest::Result result = ci_test->IndependenceResult(x_idx, y_idx, Z, "g square",
                                                                      timer, i);
        SAFE_DELETE(ci_test);
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

            if (verbose > 1) {
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
        if (verbose > 1) {
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
void PCStable::OrientVStructure(const map<int, map<int, double>> &adjacencies) {
    for (int b = 0; b < network->num_nodes; ++b) { // for all nodes in the graph
        vector<int> vec_adjacent_nodes;
        for (const auto &adj_w : adjacencies.at(b)) {
            vec_adjacent_nodes.push_back(adj_w.first);
        }

        if (adjacencies.at(b).size() < 2) {
            continue;
        }
        ChoiceGenerator cg(adjacencies.at(b).size(), 2); // find two adjacent nodes of this node
        vector<int> combination;

        while (!(combination = cg.Next()).empty()) {
            int a = vec_adjacent_nodes[combination[0]];
            int c = vec_adjacent_nodes[combination[1]];

            // 1) a is not adjacent to c; skip if a is adjacent to c
            if (network->IsAdjacentTo(adjacencies, a, c)) {
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
                bool deleted_directed1;   // if the directed edge b->a exists and being deleted
                bool deleted_undirected1 = false; // if the undirected edge a--b exists and being deleted
                bool to_add1; // if the directed edge a->b should be added
                bool added1 = false;  // if the directed edge a->b is successfully added without causing a circle
                bool deleted_directed2;   // if the directed edge b->c exists and being deleted
                bool deleted_undirected2 = false; // if the undirected edge c--b exists and being deleted
                bool to_add2; // if the directed edge c->b should be added
                bool added2 = false;  // if the directed edge c->b is successfully added without causing a circle

                deleted_directed1 = network->DeleteDirectedEdge(b, a);
                if (!deleted_directed1) { // b->a does not exist, check a--b
                    deleted_undirected1 = network->DeleteUndirectedEdge(a, b);
                }
                to_add1 = deleted_directed1 | deleted_undirected1;
                deleted_directed2 = network->DeleteDirectedEdge(b, c);
                if (!deleted_directed2) { // b->c does not exist, check c--b
                    deleted_undirected2 = network->DeleteUndirectedEdge(c, b);
                }
                to_add2 = deleted_directed2 | deleted_undirected2;

                if (to_add1) { // a->b should be added
                    added1 = network->AddDirectedEdge(a, b);
                }
                if (to_add2) { // c->b should be added
                    added2 = network->AddDirectedEdge(c, b);
                }

                // if a->b should be added but is not successfully added, or
                // c->b should be added but is not successfully added, add them back
                if (to_add1 && !added1 || to_add2 && !added2) {
                    if (to_add1) {
                        if (deleted_directed1) { // b->a is deleted, then add b->a
                            network->AddDirectedEdge(b, a);
                        } else { // a--b is deleted, then add a--b
                            network->AddUndirectedEdge(a, b);
                        }
                    }
                    if (to_add2) {
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
}

/**
 * @brief: orient remaining undirected edge as many as possible according to 3 rules
 *      1) if a->b, b--c, and a is not adj to c, then b->c (to avoid v-structures)
 *      2) if a->b->c, a--c, then a->c (to avoid circles)
 *      3) if d--a, d--b, d--c, b->a, c->a, b and c are not adjacent, then orient d->a
 *
 * note that for PC-stable, the skeleton is estimated order-independently but not the edge orientations!
 */
void PCStable::OrientImplied(const map<int, map<int, double>> &adjacencies) {
    bool oriented = true;
    /**
     * in each iteration we traverse all undirected edges and try to orient it
     * it stops if none edge is oriented during one iteration
     */
    while (oriented) {
        oriented = false;
        /**
         * note that the for loop does not have "edge_it++", "edge_it++" only happens when (*edge_it) is not erased.
         * for the case of erasing (*edge_it), the iterator will point to the next edge after erasing the current edge
         * (erasing operation is in Network::DeleteUndirectedEdge)
         */
        for (auto edge_it = network->vec_edges.begin(); edge_it != network->vec_edges.end();) {
            int x_idx = (*edge_it).GetNode1()->GetNodeIndex();
            int y_idx = (*edge_it).GetNode2()->GetNodeIndex();

            // if the edge is undirected, check the 3 rules
            if (network->IsUndirected(adjacencies, x_idx, y_idx)) {
                if (Rule1(adjacencies, x_idx, y_idx) ||
                    Rule1(adjacencies, y_idx, x_idx) ||
                    Rule2(adjacencies, x_idx, y_idx) ||
                    Rule2(adjacencies, y_idx, x_idx) ||
                    Rule3(adjacencies, x_idx, y_idx) ||
                    Rule3(adjacencies, y_idx, x_idx)) {
                    oriented = true;
                } else { // if the undirected edge x--y remains
                    edge_it++;
                }
            } else { // if the edge is not undirected
                // Instead of checking the next edge, here we just jump out this loop, because in our implementation,
                // the directed edges are always behind the undirected ones in `vec_edges`. Thus, if we meet a directed
                // edge, we don't need to check the following edges.
                break;
//                edge_it++;
            }
        } // finish checking all edges
    }
}

/**
 * orientation rule1: if a->b, b--c, and a is not adj to c, then b->c (to avoid v-structures)
 */
bool PCStable::Rule1(const map<int, map<int, double>> &adjacencies, int b_idx, int c_idx) {
    for (const auto &a_idx: network->GetParentIdxesOfNode(b_idx)) { // for every parent a of b
        if (network->IsAdjacentTo(adjacencies, c_idx, a_idx)) continue; // skip the case if a is adjacent to c
        if (Direct(b_idx, c_idx)) { // then b->c
            return true;
        }
    }
    return false;
}

/**
 * orientation rule2: if a->b->c, a--c, then a->c (to avoid circles)
 */
bool PCStable::Rule2(const map<int, map<int, double>> &adjacencies, int a_idx, int c_idx) {
    // get common neighbors of a and c
    vector<int> common_idx = GetCommonAdjacents(adjacencies, a_idx, c_idx);

    for (const int &b_idx : common_idx) { // check every common neighbor b of a and c
        if (network->IsDirectedFromTo(a_idx, b_idx) &&
            network->IsDirectedFromTo(b_idx, c_idx)) { // a->b && b->c
            if (Direct(a_idx, c_idx)) { // then a->c
                return true;
            }
        }
    }
    return false;
}

/**
 * orientation rule3: if d--a, d--b, d--c, b->a, c->a, b and c are not adjacent, then orient d->a
 */
bool PCStable::Rule3(const map<int, map<int, double>> &adjacencies, int d_idx, int a_idx) {
    // get common neighbors of a and d
    vector<int> common_idx = GetCommonAdjacents(adjacencies, a_idx, d_idx);

    if (common_idx.size() < 2) {
        return false;
    }
    // a and d has more than or equal to 2 common adjacents
    for (int i = 0; i < common_idx.size(); ++i) {
        for (int j = i + 1; j < common_idx.size(); ++j) {
            // find two adjacents b and c, b and c are not adjacent
            int b_idx = common_idx[i];
            int c_idx = common_idx[j];
            if (!network->IsAdjacentTo(adjacencies, b_idx, c_idx)) {
                if (R3Helper(adjacencies, a_idx, d_idx, b_idx, c_idx)) {
                    return true;
                }
            }
        }
    }
    return false;
}

/**
 * @brief: R3Helper is to check the following 4 edges' direction:
 * d--b, d--c, b->a, c->a
 */
bool PCStable::R3Helper(const map<int, map<int, double>> &adjacencies, int a_idx, int d_idx, int b_idx, int c_idx) {
    bool oriented = false;

    bool b5 = network->IsUndirected(adjacencies, d_idx, b_idx);
    bool b6 = network->IsUndirected(adjacencies, d_idx, c_idx);
    bool b7 = network->IsDirectedFromTo(b_idx, a_idx);
    bool b8 = network->IsDirectedFromTo(c_idx, a_idx);

    if (b5 && b6 && b7 && b8) {
        oriented = Direct(d_idx, a_idx);
    }
    return oriented;
}
