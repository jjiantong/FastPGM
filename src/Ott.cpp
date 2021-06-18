//
// Created by jjt on 2021/6/17.
//

#include "Ott.h"

/**
 * @brief: learn the structure with Ott's algorithm.
     * Ott, Sascha, Seiya Imoto, and Satoru Miyano. "Finding optimal models for small gene networks."
     * In Biocomputing 2004, pp. 557-567. 2003.
 */
void Ott::StructLearnByOtt(Dataset *dts, vector<int> topo_ord_constraint) {//TODO: double check correctness

    map<Node*, map<set<Node*>, double>> dynamic_program_for_F;
    map< pair<set<Node*>, vector<int>>,  pair<double, vector<pair<Node*, set<Node*>>> > > dynamic_program_for_Q;

    set<Node*> set_node_ptr_container;
    for (auto id_np : network->map_idx_node_ptr) {
        set_node_ptr_container.insert(id_np.second); // insert all node ptrs of the network
    }

    // if "topo_ord_constraint" = "best"; no order is provided (i.e. no constraint)
    if (topo_ord_constraint.empty() || topo_ord_constraint.size() != network->num_nodes) {
        map<set<Node*>, vector<int>> dynamic_program_for_M;
        vector<int> m_of_all_nodes = M(set_node_ptr_container, dts, dynamic_program_for_F, dynamic_program_for_Q,
                                       dynamic_program_for_M);
        topo_ord_constraint = m_of_all_nodes; // provide a order constraint
    }

    pair<double, vector<pair<Node*, set<Node*>>>> score_vec_node_parents = Q(set_node_ptr_container,
                                                                             topo_ord_constraint, dts,
                                                                             dynamic_program_for_F,
                                                                             dynamic_program_for_Q);
    vector<pair<Node*, set<Node*>>> vec_node_parents = score_vec_node_parents.second;

    cout << "==================================================" << '\n'
         << "Setting children and parents......" << endl;
    for (auto p : vec_node_parents) {
        Node *chi = p.first;
        for (auto par : p.second) {
            network->SetParentChild(par, chi);
        }
    }

    cout << "==================================================" << '\n'
         << "Generating parents combinations for each node......" << endl;
    network->GenDiscParCombsForAllNodes();
}

/**
 * @brief: compute F(node, candidate_parents) using dynamic programming
 * used in Ott's algorithm for structure learning
 * the paper chooses minimum score but this program chooses maximum score, this depends on the specific scoring function
 * @param dynamic_program_for_F: map<Node*, map<set<Node*>, double>>, where Node* refers to "node", set<Node*> refers to "candidate_parents"
 * @return pair<double, set<Node*>>: function value, candidate_parents
 */
pair<double, set<Node*>> Ott::F(Node *node, set<Node*> &candidate_parents, Dataset *dts,
                                map<Node*, map<set<Node*>, double>> &dynamic_program_for_F) {

    map<set<Node*>, double> &this_node_dynamic_program = dynamic_program_for_F[node];

    // look up the table and return the value
    if (this_node_dynamic_program.find(candidate_parents) != this_node_dynamic_program.end()) {
        return pair<double, set<Node*>>(this_node_dynamic_program[candidate_parents], candidate_parents);
    }

    pair<double, set<Node*>> max_score_parents;
    max_score_parents.first = -DBL_MAX;

    // Look up the dynamic programming table.
    for (auto n : candidate_parents) {
        // recursively compute F of (node, candidate_parents_temp), i.e., F(node, candidate_parents-{n})
        set<Node*> candidate_parents_temp = candidate_parents;
        candidate_parents_temp.erase(n);

        // if F(node, candidate_parents-{n}) has not been computed
        if (this_node_dynamic_program.find(candidate_parents_temp) == this_node_dynamic_program.end()) {
            this_node_dynamic_program[candidate_parents_temp] =
                    F(node, candidate_parents_temp, dts, dynamic_program_for_F).first;
        }

        // find the maximum of F(node, candidate_parents-{n})
        if (this_node_dynamic_program[candidate_parents_temp] > max_score_parents.first) {
            max_score_parents.first = this_node_dynamic_program[candidate_parents_temp];
            max_score_parents.second = candidate_parents_temp;
        }
    }

    // if F(node, candidate_parents) has not been computed
    if (this_node_dynamic_program.find(candidate_parents)==this_node_dynamic_program.end()) {

        // construct a network with node and candidate_parents
        DiscreteNode node_copy = *dynamic_cast<DiscreteNode*>(node);
        vector<DiscreteNode> candidate_parents_copy;
        for (auto n : candidate_parents) {
            candidate_parents_copy.push_back(*(DiscreteNode*)(n));
        }

        Network temp_net;
        temp_net.AddNode(&node_copy);
        for (auto n : candidate_parents_copy) {
            temp_net.AddNode(&n);
            temp_net.SetParentChild(&n, &node_copy);
        }
        temp_net.GenDiscParCombsForAllNodes();

        // compute the scoring function s(node, candidate_parents)
        ScoreFunction sf(&temp_net, dts);
        this_node_dynamic_program[candidate_parents] = sf.ScoreForNode(&node_copy,"log BDeu");

    }

    // find the maximum and get F(node, candidate_parents)
    if (this_node_dynamic_program[candidate_parents] > max_score_parents.first) {
        max_score_parents.first = this_node_dynamic_program[candidate_parents];
        max_score_parents.second = candidate_parents;
    }

    return max_score_parents;
}

/*!
 * @brief: compute Q^{set_nodes}(M(set_nodes)) = Q^{set_nodes}(topo_ord)
 * @param topo_ord = M(set_nodes)
 * @param dynamic_program_for_Q: map<pair<set<Node*>, vector<int>>,   pair<double, vector<pair<Node*, set<Node*>>>>>
 *              1. set<Node*>:      set_nodes
 *              2. vector<int>>:    topo_ord
 *              3. double:          function value
 *              4. Node*:           node for F
 *              5. set<Node*>:      candidate_parents for F
 * @return pair<double, vector<pair<Node*, set<Node*>>>>: function value, (one node in order, set_nodes)?
 */
pair<double, vector<pair<Node*, set<Node*>>>>
Ott::Q(set<Node*> &set_nodes, vector<int> topo_ord, Dataset *dts,
       map<Node*, map<set<Node*>, double>> &dynamic_program_for_F,
       map<pair<set<Node*>, vector<int>>, pair<double, vector<pair<Node*, set<Node*>>>>> dynamic_program_for_Q) {

    // look up the table and return the value
    pair<set<Node*>, vector<int>> to_find(set_nodes, topo_ord);
    if (dynamic_program_for_Q.find(to_find)!=dynamic_program_for_Q.end()) {
        return dynamic_program_for_Q[to_find];
    }

    double score = 0;
    vector<pair<Node*, set<Node*>>> vec_node_parents;

    // accumulate Q^{set_nodes}(topo_ord)
    // Q^{set_nodes}(topo_ord) = sum of F(n, pre nodes of n according to topo_ord) over n
    for (auto &n : set_nodes) { // for each node n
        set<Node*> candidate_parents;
        // insert all the pre nodes of n according to the topological ordering "topo_ord"
        for (const auto &i : topo_ord) {
            if (n->GetNodeIndex() == i) {
                break;
            }
            candidate_parents.insert(network->FindNodePtrByIndex(i));
        }
        pair<double, set<Node*>> best_score_parents = F(n, candidate_parents, dts, dynamic_program_for_F);

        score += best_score_parents.first;
        vec_node_parents.push_back(pair<Node*, set<Node*>>(n, best_score_parents.second));
    }

    return pair<double, vector<pair<Node*, set<Node*>>>>(score, vec_node_parents);
}

/*!
 * @brief: compute M(set_nodes) using dynamic programming
 * used in Ott's algorithm for structure learning
 * @param set_nodes: all node ptrs of the network
 * @return M(set_nodes), which is a subset of all nodes, # also equals to |set_nodes|
 */
vector<int> Ott::M(set<Node*> &set_nodes, Dataset *dts, map<Node*, map<set<Node*>, double>> &dynamic_program_for_F,
                   map<pair<set<Node*>, vector<int>>, pair<double, vector<pair<Node*, set<Node*>>>>> dynamic_program_for_Q,
                   map<set<Node*>, vector<int>> dynamic_program_for_M) {

    if (set_nodes.empty()) { // M(∅) = ∅
        return vector<int> {};
    }

    // look up the table and return the value
    if (dynamic_program_for_M.find(set_nodes) != dynamic_program_for_M.end()) {
        return dynamic_program_for_M[set_nodes];
    }

    Node *g_star = nullptr;
    double score = -DBL_MAX;
    for (auto n : set_nodes) {
        // recursively compute M of set_nodes_temp, i.e., M(set_nodes-{n})
        set<Node*> set_nodes_temp = set_nodes;
        set_nodes_temp.erase(n);
        vector<int> m_of_set_nodes_temp = M(set_nodes_temp, dts,dynamic_program_for_F,
                                            dynamic_program_for_Q, dynamic_program_for_M);

        // compute F + Q TODO: double-check
        double score_temp = F(n, set_nodes_temp, dts, dynamic_program_for_F).first
                            + Q(set_nodes_temp, m_of_set_nodes_temp, dts,
                                dynamic_program_for_F, dynamic_program_for_Q).first;

        // find the maximum.
        if (score_temp > score) {
            score = score_temp;
            g_star = n; // get the g*
        }
    }

    // compute M(set_nodes) and return
    set<Node*> set_nodes_remove_g_star = set_nodes;
    set_nodes_remove_g_star.erase(g_star);
    // for 1 <= i < |set_nodes|, M(set_nodes)[i] = M(set_nodes - {g*})[i]
    vector<int> result = M(set_nodes_remove_g_star, dts, dynamic_program_for_F,
                           dynamic_program_for_Q, dynamic_program_for_M);
    // M(set_nodes)[|set_nodes|] = g*
    result.push_back(g_star->GetNodeIndex());

    return result;
}

void Ott::StructLearnCompData(Dataset *dts, bool print_struct, string topo_ord_constraint, int max_num_parents) {

    cout << "==================================================" << '\n'
         << "Begin structural learning with complete data using K2 algorithm...." << endl;

    struct timeval start, end;
    double diff;
    gettimeofday(&start,NULL);

    AssignNodeInformation(dts);
    vector<int> ord = AssignNodeOrder(topo_ord_constraint);

    StructLearnByOtt(dts, ord);

    cout << "==================================================" << '\n'
         << "Finish structural learning." << endl;

    gettimeofday(&end,NULL);
    diff = (end.tv_sec-start.tv_sec) + ((double)(end.tv_usec-start.tv_usec))/1.0E6;
    setlocale(LC_NUMERIC, "");//formatting the output
    cout << "==================================================" << '\n'
         << "The time spent to learn the structure is " << diff << " seconds" << endl;

    if (print_struct) {

        cout << "==================================================" << '\n'
             << "Topological sorted permutation generated using width-first-traversal: " << endl;
        auto topo = network->GetTopoOrd();
        for (int m = 0; m < network->num_nodes; ++m) {
            cout << topo.at(m) << ", ";
        }
        cout << endl;

        cout << "==================================================" << '\n'
             << "Each node's parents: " << endl;
        network->PrintEachNodeParents();
    }
}