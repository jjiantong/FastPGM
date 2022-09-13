#include "JunctionTree.h"

JunctionTree::JunctionTree(Network *net) : network(net) {
    cout << "begin construction function of JunctionTree..." << endl;

    Timer *timer = new Timer();
    // record time
    timer->Start("construct jt");

    tree = new JunctionTreeStructure(net);

    // Arbitrarily select a clique as the root.
    // TODO: find a better root that generate a more balanced tree structure
    auto iter = tree->vector_clique_ptr_container.begin();
    arb_root = *iter;

    MarkLevel();
    cout << "finish MarkLevel" << endl;

    BackUpJunctionTree();
    cout << "finish BackUpJunctionTree" << endl;

    timer->Stop("construct jt");
    cout << "==================================================";
    cout << endl; timer->Print("construct jt"); cout << endl;
    delete timer;
    timer = nullptr;
}

JunctionTree::~JunctionTree() {
    delete tree;

    delete [] clique_backup;
    delete [] separator_backup;
}

/**
 * @brief: do level traverse of the tree, at the same time:
 *      1 add all the cliques & separators in "nodes_by_level" by level
 *      2 add all the separators in "seps_by_level" by level
 *      3 mark both "ptr_upstream_clique" and "ptr_downstream_cliques"
 */
void JunctionTree::MarkLevel() {
    vector<Clique*> vec; // a set of cliques/seps in one level
    vec.push_back(arb_root); // push the root into vec
    nodes_by_level.push_back(vec); // the first level only has the root clique

    while (!vec.empty()) {
        vector<Clique*> vec2;
        vector<Separator*> vsep2;
        /*
         * think of nodes...:       clq, sep, clq, sep, ... clq
         * size of node by levels:  1,   2,   3,   4,   ...
         * if nodes.size % 2 == 0, "vec" is sep, then vec's downstream neighbors are clqs
         */
        bool is_sep = nodes_by_level.size() % 2 == 0 ? false : true;

        for (int i = 0; i < vec.size(); ++i) { // for each clique/sep in the current level
            Clique *clique = vec[i];
            for (auto &ptr_neighbor : clique->set_neighbours_ptr) {
                // all neighbor cliques of "clique" contain the upstream clique and downstream clique(s)
                // if the current neighbor "ptr_separator" is the upstream clique, do nothing
                if (ptr_neighbor == clique->ptr_upstream_clique) {
                    continue;
                }
                // the current neighbor "ptr_separator" is a downstream clique of "clique"
                clique->ptr_downstream_cliques.push_back(ptr_neighbor);
                ptr_neighbor->ptr_upstream_clique = clique;  // Let the callee know the caller.
                vec2.push_back(ptr_neighbor);
                if (is_sep) { // cast and push to the separator vector
                    Separator* ptr_sep = dynamic_cast<Separator*>(ptr_neighbor);
                    vsep2.push_back(ptr_sep);
                }
            }
        }

        nodes_by_level.push_back(vec2);
        if (is_sep) {
            separators_by_level.push_back(vsep2);
        }
        vec = vec2;
    }

    nodes_by_level.pop_back();
    separators_by_level.pop_back();
    max_level = nodes_by_level.size();
}

/**
 * backup & reset:
 * The inference process will modify the junction tree itself.
 * So, we need to backup the tree and restore it after an inference.
 * Otherwise, we need to re-construct the tree each time we want to make inference.
 */
void JunctionTree::BackUpJunctionTree() {
    clique_backup = new Clique[tree->vector_clique_ptr_container.size()];
    separator_backup = new Separator[tree->vector_separator_ptr_container.size()];
    int i = 0;
    for (const auto &c : tree->vector_clique_ptr_container) {
        clique_backup[i++] = *c;
    }
    i = 0;
    for (const auto &s : tree->vector_separator_ptr_container) {
        separator_backup[i++] = *s;
    }
}

void JunctionTree::ResetJunctionTree() {
    int i = 0;
    for (auto &c : tree->vector_clique_ptr_container) {
        *c = clique_backup[i++];
    }
    i = 0;
    for (auto &s : tree->vector_separator_ptr_container) {
        *s = separator_backup[i++];
    }
}

/**
 * @brief: when inferring, an evidence is given. The evidence needs to be loaded and propagate in the network.
 */
void JunctionTree::LoadDiscreteEvidence(const DiscreteConfig &E, int num_threads, Timer *timer) {

    for (auto &e: E) { // for each observation of variable
        timer->Start("pre-evi");
        // we need the index of the the evidence and the value index
        int index = e.first;
        int value = e.second;
        if (index >= network->num_nodes) {
            // todo: this is because the testing set has more features than the training set
            //  for this case, we just ignore such evidence in the current implementation
            continue;
        }

        int value_index;
        auto dn = dynamic_cast<DiscreteNode*>(network->FindNodePtrByIndex(index));
        for (int i = 0; i < dn->GetDomainSize(); ++i) {
            if (value == dn->vec_potential_vals[i]) {
                value_index = i;
                break;
            }
        }

//        /**
//         * 1. nested version, without flattening
//         */
//        for (auto &c: vector_clique_ptr_container) {
//            if (c->p_table.related_variables.find(index) != c->p_table.related_variables.end()) {
//                c->p_table.TableReduction(index, value_index, num_threads);
//            }
//        }
//        for (auto &c: vector_separator_ptr_container) {
//            if (c->p_table.related_variables.find(index) != c->p_table.related_variables.end()) {
//                c->p_table.TableReduction(index, value_index, num_threads);
//            }
//        }
//        timer->Stop("pre-evi");

        /**
         * 2. flatten version
         */
        vector<Clique*> vector_reduced_clique_and_separator_ptr;
        for (auto &c: tree->vector_clique_ptr_container) {
            if (c->p_table.related_variables.find(index) != c->p_table.related_variables.end()) {
                vector_reduced_clique_and_separator_ptr.push_back(c);
            }
        }
        for (auto &c: tree->vector_separator_ptr_container) {
            if (c->p_table.related_variables.find(index) != c->p_table.related_variables.end()) {
                vector_reduced_clique_and_separator_ptr.push_back(c);
            }
        }
        timer->Stop("pre-evi");

        LoadEvidenceToNodes(vector_reduced_clique_and_separator_ptr, index, value_index, num_threads, timer);
    }
}


void JunctionTree::LoadEvidenceToNodes(vector<Clique*> &vector_reduced_node_ptr,
                                       int index, int value_index, int num_threads, Timer *timer) {

    timer->Start("pre-evi");
    int red_size = vector_reduced_node_ptr.size();

    int *e_loc = new int[red_size];
    int **full_config = new int*[red_size];
    int **v_index = new int*[red_size];

    int *cum_sum = new int[red_size];
    int final_sum = 0;

    /**
     * pre-computing
     */
    for (int k = 0; k < red_size; ++k) {
        auto clique_ptr = vector_reduced_node_ptr[k];

        // update sum
        cum_sum[k] = final_sum;
        final_sum += clique_ptr->p_table.table_size;
    }

    timer->Start("parallel");
    omp_set_num_threads(num_threads);
#pragma omp parallel for
    for (int k = 0; k < red_size; ++k) {
        auto clique_ptr = vector_reduced_node_ptr[k];

        e_loc[k] = clique_ptr->p_table.GetVariableIndex(index);
        full_config[k] = new int[clique_ptr->p_table.table_size * clique_ptr->p_table.num_variables];
        v_index[k] = new int[clique_ptr->p_table.table_size];
    }

    timer->Stop("pre-evi");

    timer->Start("main-evi");
    // the main loop
    omp_set_num_threads(num_threads);
#pragma omp parallel for
    for (int s = 0; s < final_sum; ++s) {
        // compute k and i
        int k = -1;
        for (int m = red_size - 1; m >= 0; --m) {
            if (s >= cum_sum[m]) {
                k = m;
                break;
            }
        }
        int i = s - cum_sum[k];

        v_index[k][i] = vector_reduced_node_ptr[k]->p_table.TableReductionMain(i, full_config[k], e_loc[k]);
    }
    timer->Stop("main-evi");

    timer->Start("post-evi");
    /**
     * post-computing
     */
    omp_set_num_threads(num_threads);
#pragma omp parallel for
    for (int k = 0; k < red_size; ++k) {
        auto clique_ptr = vector_reduced_node_ptr[k];

        int new_size = clique_ptr->p_table.table_size / clique_ptr->p_table.var_dims[e_loc[k]];
        vector<double> new_potentials;
        new_potentials.resize(new_size);
        clique_ptr->p_table.TableReductionPost(index, value_index, v_index[k], new_potentials, e_loc[k]);

        delete[] full_config[k];
        delete[] v_index[k];
    }
    timer->Stop("parallel");

    delete[] e_loc;
    delete[] full_config;
    delete[] v_index;
    delete[] cum_sum;
    timer->Stop("post-evi");
}

/**
 *
 * Message passing is just COLLECT and DISTRIBUTE (these two words is used by paper and text book).
 * The order between COLLECT and DISTRIBUTE does not matter, but they must not interleave.
 * After message passing, any clique (junction tree node) contains the right distribution of the related variables.
 */
void JunctionTree::MessagePassingUpdateJT(int num_threads, Timer *timer) {

    /**
     * 1. omp task
     */
//    timer->Start("upstream");
//#pragma omp parallel num_threads(num_threads)
//    {
//#pragma omp single
//        {
//            arb_root->Collect2();
////            arb_root->Collect3(nodes_by_level, max_level);
//        }
//    }
//    timer->Stop("upstream");
//
//    timer->Start("downstream");
//#pragma omp parallel num_threads(num_threads)
//    {
//#pragma omp single
//        {
//            arb_root->Distribute2();
////            arb_root->Distribute3(nodes_by_level, max_level);
//        }
//    }
//    timer->Stop("downstream");

    /**
     * 2. omp parallel for
     */
    timer->Start("upstream");
//    arb_root->Collect3(nodes_by_level, max_level, num_threads, timer);
    Collect(num_threads, timer);
    timer->Stop("upstream");

    timer->Start("downstream");
//    arb_root->Distribute3(nodes_by_level, max_level, num_threads);
    Distribute(num_threads, timer);
    timer->Stop("downstream");
}

/**
 * operations for separators, including marginalization and division
 * @param is_collect  true for upstream passing, false for downstream passing
 * @param i  corresponds to level
 */
void JunctionTree::SeparatorLevelOperation(bool is_collect, int i, int num_threads, Timer *timer) {
    int size = separators_by_level[i/2].size();
    vector<PotentialTable> tmp_pt; // store all tmp pt used for table marginalization
    tmp_pt.resize(size);

    // store number_variables and cum_levels of the original table
    // rather than storing the whole potential table
    int *nv_old = new int[size];
    vector<vector<int>> cl_old;
    cl_old.resize(size);

    int *cum_sum = new int[size];
    int final_sum = 0;

    // set of arrays, showing the locations of the variables of the new table in the old table
    int **loc_in_old = new int*[size];
    int **full_config = new int*[size];
    int **partial_config = new int*[size];
    int **table_index = new int*[size];

    if (is_collect) {
        timer->Start("pre-up-sep");

        for (int j = 0; j < size; ++j) {
            auto separator = separators_by_level[i/2][j];
            auto child = separator->ptr_downstream_cliques[0]; // there is only one child for each separator
            // update sum
            cum_sum[j] = final_sum;
            final_sum += child->p_table.table_size;
        }

        /**
         * pre computing
         */
        timer->Start("parallel");
        omp_set_num_threads(num_threads);
#pragma omp parallel for
        for (int j = 0; j < size; ++j) { // for each separator in this level
            auto separator = separators_by_level[i/2][j];
            auto child = separator->ptr_downstream_cliques[0]; // there is only one child for each separator

            separator->old_ptable = separator->p_table; // used for division

            // find the variables to be marginalized
            set<int> set_external_vars;
            set_difference(child->p_table.related_variables.begin(), child->p_table.related_variables.end(),
                           separator->clique_variables.begin(), separator->clique_variables.end(),
                           inserter(set_external_vars, set_external_vars.begin()));

            // store the parent's table, used for update the child's table
            nv_old[j] = child->p_table.num_variables;
            cl_old[j] = child->p_table.cum_levels;

            child->p_table.TableMarginalizationPre(set_external_vars, tmp_pt[j]);

            // generate an array showing the locations of the variables of the new table in the old table
            loc_in_old[j] = new int[tmp_pt[j].num_variables];
            int k = 0;
            for (auto &v: tmp_pt[j].related_variables) {
                loc_in_old[j][k++] = child->p_table.GetVariableIndex(v);
            }
            table_index[j] = new int[child->p_table.table_size];

            // malloc in pre-, not to parallelize
            full_config[j] = new int[child->p_table.table_size * child->p_table.num_variables];
            partial_config[j] = new int[child->p_table.table_size * tmp_pt[j].num_variables];
        }
        timer->Stop("pre-up-sep");

        timer->Start("main-up-sep");
        // the main loop
        omp_set_num_threads(num_threads);
#pragma omp parallel for
        for (int s = 0; s < final_sum; ++s) {
            // compute j and k
            int j = -1;
            for (int m = size - 1; m >= 0; --m) {
                if (s >= cum_sum[m]) {
                    j = m;
                    break;
                }
            }
            int k = s - cum_sum[j];

            table_index[j][k] = tmp_pt[j].TableMarginalizationMain(k, full_config[j], partial_config[j],
                                                                   nv_old[j], cl_old[j], loc_in_old[j]);
        }
        timer->Stop("main-up-sep");

        timer->Start("post-up-sep");
        // post-computing
        omp_set_num_threads(num_threads);
#pragma omp parallel for
        for (int j = 0; j < size; ++j) { // for each separator in this level
            delete[] loc_in_old[j];
            delete[] full_config[j];
            delete[] partial_config[j];

            auto separator = separators_by_level[i/2][j];
            auto child = separator->ptr_downstream_cliques[0]; // there is only one child for each separator

            for (int k = 0; k < child->p_table.table_size; ++k) {
                // 4. potential[table_index]
                tmp_pt[j].potentials[table_index[j][k]] += child->p_table.potentials[k];
            }
            delete[] table_index[j];

            tmp_pt[j].TableDivision(separator->old_ptable);

            separator->p_table = tmp_pt[j];
        }
        timer->Stop("parallel");
        timer->Stop("post-up-sep");
    } else {
        timer->Start("pre-down-sep");
        /**
         * pre computing
         */
        for (int j = 0; j < size; ++j) {
            auto separator = separators_by_level[i/2][j];
            auto par = separator->ptr_upstream_clique;
            // update sum
            cum_sum[j] = final_sum;
            final_sum += par->p_table.table_size;
        }

        timer->Start("parallel");
        omp_set_num_threads(num_threads);
#pragma omp parallel for
        for (int j = 0; j < size; ++j) { // for each separator in this level
            auto separator = separators_by_level[i/2][j];
            auto par = separator->ptr_upstream_clique;

            separator->old_ptable = separator->p_table; // used for division

            // find the variables to be marginalized
            set<int> set_external_vars;
            set_difference(par->p_table.related_variables.begin(), par->p_table.related_variables.end(),
                           separator->clique_variables.begin(), separator->clique_variables.end(),
                           inserter(set_external_vars, set_external_vars.begin()));

            // store the parent's table, used for update the child's table
            nv_old[j] = par->p_table.num_variables;
            cl_old[j] = par->p_table.cum_levels;

            par->p_table.TableMarginalizationPre(set_external_vars, tmp_pt[j]);

            // generate an array showing the locations of the variables of the new table in the old table
            loc_in_old[j] = new int[tmp_pt[j].num_variables];
            int k = 0;
            for (auto &v: tmp_pt[j].related_variables) {
                loc_in_old[j][k++] = par->p_table.GetVariableIndex(v);
            }
            table_index[j] = new int[par->p_table.table_size];

            // malloc in pre-, not to parallelize
            full_config[j] = new int[par->p_table.table_size * par->p_table.num_variables];
            partial_config[j] = new int[par->p_table.table_size * tmp_pt[j].num_variables];
        }
        timer->Stop("pre-down-sep");

        timer->Start("main-down-sep");
        // the main loop
        omp_set_num_threads(num_threads);
#pragma omp parallel for
        for (int s = 0; s < final_sum; ++s) {
            // compute j and k
            int j = -1;
            for (int m = size - 1; m >= 0; --m) {
                if (s >= cum_sum[m]) {
                    j = m;
                    break;
                }
            }
            int k = s - cum_sum[j];

            table_index[j][k] = tmp_pt[j].TableMarginalizationMain(k, full_config[j], partial_config[j],
                                                                   nv_old[j], cl_old[j], loc_in_old[j]);
        }
        timer->Stop("main-down-sep");

        timer->Start("post-down-sep");
        // post-computing
        omp_set_num_threads(num_threads);
#pragma omp parallel for
        for (int j = 0; j < size; ++j) { // for each separator in this level
            timer->Start("post-down-sep-mem");
            auto separator = separators_by_level[i/2][j];
            auto par = separator->ptr_upstream_clique;

            for (int k = 0; k < par->p_table.table_size; ++k) {
                // 4. potential[table_index]
                tmp_pt[j].potentials[table_index[j][k]] += par->p_table.potentials[k];
            }
            timer->Stop("post-down-sep-mem");

            timer->Start("post-down-sep-del");
            delete[] loc_in_old[j];
            delete[] full_config[j];
            delete[] partial_config[j];
            delete[] table_index[j];
            timer->Stop("post-down-sep-del");

            timer->Start("post-down-sep-div");
            tmp_pt[j].TableDivision(separator->old_ptable);
            separator->p_table = tmp_pt[j];
            timer->Stop("post-down-sep-div");
        }
        timer->Stop("parallel");

        timer->Stop("post-down-sep");
    }

    timer->Start("post-sep-del");
    delete[] loc_in_old;
    delete[] full_config;
    delete[] partial_config;
    delete[] table_index;
    delete[] cum_sum;
    delete[] nv_old;
    timer->Stop("post-sep-del");
}

void JunctionTree::Collect(int num_threads, Timer *timer) {
    for (int i = max_level - 2; i >= 0 ; --i) { // for each level

        if (i % 2) {
            /**
             * case 1: levels 1, 3, 5, ... are separator levels
             * collect msg from its child (a clique) to it (a separator)
             * do marginalization + division for separator levels
             */
            SeparatorLevelOperation(true, i, num_threads, timer);
        }
        else {
            /**
             * case 2: levels 0, 2, 4, ... are clique levels
             * collect msg from its children (separators) to it (a clique)
             * do extension + multiplication for clique levels
             */
            timer->Start("pre-up-clq");
            int size = nodes_by_level[i].size();

            int max_num_children = 0;
            for (int j = 0; j < size; ++j) { // for each clique of this level
                /**
                 * there may be multiple children for a clique
                 * first process the first child of each clique in this level,
                 * then the second child of each clique (if has), ...
                 * until all the children of all the cliques in this level have been processed
                 */
                auto clique = nodes_by_level[i][j];
                // first, find the max number of children for this level
                if (clique->ptr_downstream_cliques.size() > max_num_children) {
                    max_num_children = clique->ptr_downstream_cliques.size();
                }
            }
            timer->Stop("pre-up-clq");

            for (int k = 0; k < max_num_children; ++k) { // process the k-th child
                timer->Start("pre-up-clq");
                // use a vector to mark which clique(s) has the k-th children
                vector<int> has_kth_child;
                has_kth_child.reserve(size);
                for (int j = 0; j < size; ++j) { // of each clique
                    auto clique = nodes_by_level[i][j];
                    if (clique->ptr_downstream_cliques.size() > k) {
                        // this clique has the k-th child
                        has_kth_child.push_back(j);
                    }
                }

                /**
                 * then, inside this loop (k), the following operations are similar to before:
                 * before: process "size" cliques in parallel, each update once
                 * now: process "process_size" cliques in parallel, each update once
                 */
                int process_size = has_kth_child.size();

                vector<PotentialTable> tmp_pt;
                tmp_pt.reserve(2 * process_size);

                vector<PotentialTable> multi_pt;
                multi_pt.resize(process_size);

                // store number_variables and cum_levels of the original table
                // rather than storing the whole potential table
                int *nv_old = new int[2 * process_size];
                vector<vector<int>> cl_old;
                cl_old.reserve(2 * process_size);

                int *cum_sum = new int[2 * process_size];
                int final_sum = 0;
                int sum_index = 0;

                // not all tables need to do the extension
                // there are "2 * process_size" tables in total
                // use a vector to show which tables need to do the extension
                vector<int> vector_extension;
                vector_extension.reserve(process_size);

                // set of arrays, showing the locations of the variables of the new table in the old table
                int **loc_in_new = new int*[2 * process_size];
                int **full_config = new int*[2 * process_size];
                int **partial_config = new int*[2 * process_size];
                int **table_index = new int*[2 * process_size];

                /**
                 * pre computing
                 */
                for (int j = 0; j < process_size; ++j) { // for each clique (has the k-th child) in this level
                    auto clique = nodes_by_level[i][has_kth_child[j]];
                    auto child = clique->ptr_downstream_cliques[k];

                    multi_pt[j] = child->p_table;

                    // pre processing for extension
                    set<int> all_related_variables;
                    set<int> diff1, diff2;
                    clique->p_table.MultiplicationPre(child->p_table, all_related_variables, diff1, diff2);

                    if (diff1.empty() && diff2.empty()) { // if both table1 and table2 should not be extended
                        // do nothing
                    } else if (!diff1.empty() && diff2.empty()) { // if table1 should be extended and table2 not
                        // record the index (that requires to do the extension)
                        vector_extension.push_back(j * 2 + 0);
                        nv_old[sum_index] = clique->p_table.num_variables;
                        cl_old.push_back(clique->p_table.cum_levels);

                        PotentialTable pt;
                        pt.ExtensionPre(all_related_variables, child->p_table.var_dims);

                        // get this table's location -- it is currently the last one
                        int last = vector_extension.size() - 1;
                        // generate an array showing the locations of the variables of the new table in the old table
                        loc_in_new[last] = new int[clique->p_table.num_variables];
                        int k = 0;
                        for (auto &v: clique->p_table.related_variables) {
                            loc_in_new[last][k++] = pt.GetVariableIndex(v);
                        }
                        table_index[last] = new int[pt.table_size];

                        tmp_pt.push_back(pt);

                        // malloc in pre-, not to parallelize
                        full_config[last] = new int[pt.table_size * pt.num_variables];
                        partial_config[last] = new int[pt.table_size * clique->p_table.num_variables];

                        // update sum
                        cum_sum[sum_index++] = final_sum;
                        final_sum += pt.table_size;
                    } else if (diff1.empty() && !diff2.empty()) { // if table2 should be extended and table1 not
                        // record the index (that requires to do the extension)
                        vector_extension.push_back(j * 2 + 1);
                        nv_old[sum_index] = child->p_table.num_variables;
                        cl_old.push_back(child->p_table.cum_levels);

                        PotentialTable pt;
                        pt.ExtensionPre(all_related_variables, clique->p_table.var_dims);

                        // get this table's location -- it is currently the last one
                        int last = vector_extension.size() - 1;
                        // generate an array showing the locations of the variables of the new table in the old table
                        loc_in_new[last] = new int[child->p_table.num_variables];
                        int k = 0;
                        for (auto &v: child->p_table.related_variables) {
                            loc_in_new[last][k++] = pt.GetVariableIndex(v);
                        }
                        table_index[last] = new int[pt.table_size];

                        tmp_pt.push_back(pt);

                        // malloc in pre-, not to parallelize
                        full_config[last] = new int[pt.table_size * pt.num_variables];
                        partial_config[last] = new int[pt.table_size * child->p_table.num_variables];

                        // update sum
                        cum_sum[sum_index++] = final_sum;
                        final_sum += pt.table_size;
                    } else { // if both table1 and table2 should be extended
                        // record the index (that requires to do the extension)
                        vector_extension.push_back(j * 2 + 0);
                        vector_extension.push_back(j * 2 + 1);
                        nv_old[sum_index] = clique->p_table.num_variables;
                        cl_old.push_back(clique->p_table.cum_levels);
                        nv_old[sum_index + 1] = child->p_table.num_variables;
                        cl_old.push_back(child->p_table.cum_levels);

                        PotentialTable tmp_pta, tmp_ptb;

                        vector<int> dims; // to save dims of the new related variables
                        dims.reserve(all_related_variables.size());
                        // to find the location of each new related variable
                        for (auto &v: all_related_variables) {
                            int loc = clique->p_table.GetVariableIndex(v);
                            if (loc < clique->p_table.related_variables.size()) { // find it in table1
                                dims.push_back(clique->p_table.var_dims[loc]);
                            } else { // cannot find in table1, we need to find it in table2
                                loc = child->p_table.GetVariableIndex(v);
                                dims.push_back(child->p_table.var_dims[loc]);
                            }
                        }

                        tmp_pta.ExtensionPre(all_related_variables, dims);
                        tmp_ptb.ExtensionPre(all_related_variables, dims);

                        // get this table's location -- it is currently the last one
                        int last = vector_extension.size() - 1;
                        // generate an array showing the locations of the variables of the new table in the old table
                        loc_in_new[last - 1] = new int[clique->p_table.num_variables];
                        int k = 0;
                        for (auto &v: clique->p_table.related_variables) {
                            loc_in_new[last][k++] = tmp_pta.GetVariableIndex(v);
                        }
                        table_index[last] = new int[tmp_pta.table_size];

                        tmp_pt.push_back(tmp_pta);

                        // malloc in pre-, not to parallelize
                        full_config[last - 1] = new int[tmp_pta.table_size * tmp_pta.num_variables];
                        partial_config[last] = new int[tmp_pta.table_size * clique->p_table.num_variables];

                        // generate an array showing the locations of the variables of the new table in the old table
                        loc_in_new[last] = new int[child->p_table.num_variables];
                        k = 0;
                        for (auto &v: child->p_table.related_variables) {
                            loc_in_new[last][k++] = tmp_ptb.GetVariableIndex(v);
                        }
                        table_index[last] = new int[tmp_ptb.table_size];

                        tmp_pt.push_back(tmp_ptb);

                        // malloc in pre-, not to parallelize
                        full_config[last] = new int[tmp_ptb.table_size * tmp_ptb.num_variables];
                        partial_config[last] = new int[tmp_ptb.table_size * child->p_table.num_variables];

                        // update sum
                        cum_sum[sum_index++] = final_sum;
                        final_sum += tmp_pta.table_size;
                        cum_sum[sum_index++] = final_sum;
                        final_sum += tmp_ptb.table_size;
                    }
                }
                timer->Stop("pre-up-clq");

                timer->Start("main-up-clq");
                int size_e = vector_extension.size(); // the number of variables to be extended

                timer->Start("parallel");
                // the main loop
                omp_set_num_threads(num_threads);
#pragma omp parallel for
                for (int s = 0; s < final_sum; ++s) {
                    // compute j and k
                    int j = -1;
                    for (int m = size_e - 1; m >= 0; --m) {
                        if (s >= cum_sum[m]) {
                            j = m;
                            break;
                        }
                    }
                    int k = s - cum_sum[j];

                    // 1. get the full config value of new table
                    tmp_pt[j].GetConfigValueByTableIndex(k, full_config[j] + k * tmp_pt[j].num_variables);
                    // 2. get the partial config value from the new table
                    for (int l = 0; l < nv_old[j]; ++l) {
                        partial_config[j][k * nv_old[j] + l] = full_config[j][k * tmp_pt[j].num_variables + loc_in_new[j][l]];
                    }
                    // 3. obtain the potential index
                    table_index[j][k] = tmp_pt[j].GetTableIndexByConfigValue(partial_config[j] + k * nv_old[j], nv_old[j], cl_old[j]);
                }
                timer->Stop("parallel");
                timer->Stop("main-up-clq");

                timer->Start("post-up-clq");
                // post-computing
                int *cum_sum2 = new int[process_size];
                int final_sum2 = 0;

                int l = 0;
                for (int j = 0; j < process_size; ++j) {
                    auto clique = nodes_by_level[i][has_kth_child[j]];
                    auto child = clique->ptr_downstream_cliques[k];

                    int m = j * 2 + 0;
                    if (l < size_e && m == vector_extension[l]) { // index k have done the extension
                        delete[] loc_in_new[l];
                        delete[] full_config[l];
                        delete[] partial_config[l];

                        for (int k = 0; k < tmp_pt[l].table_size; ++k) {
                            // 4. potential[table_index]
                            tmp_pt[l].potentials[k] = clique->p_table.potentials[table_index[l][k]];
                        }
                        delete[] table_index[l];

                        clique->p_table = tmp_pt[l];

                        l++;
                    }

                    m = j * 2 + 1;
                    if (l < size_e && m == vector_extension[l]) { // index j have done the extension
                        delete[] loc_in_new[l];
                        delete[] full_config[l];
                        delete[] partial_config[l];

                        for (int k = 0; k < tmp_pt[l].table_size; ++k) {
                            // 4. potential[table_index]
                            tmp_pt[l].potentials[k] = child->p_table.potentials[table_index[l][k]];
                        }
                        delete[] table_index[l];

                        multi_pt[j] = tmp_pt[l];

                        l++;
                    }

                    cum_sum2[j] = final_sum2;
                    final_sum2 += multi_pt[j].table_size;
                }

                delete[] loc_in_new;
                delete[] full_config;
                delete[] partial_config;
                delete[] table_index;
                delete[] cum_sum;
                delete[] nv_old;

//                for (int j = 0; j < process_size; ++j) {
//                    for (int k = 0; k < multi_pt[j].table_size; ++k) {
//                        nodes_by_level[i][has_kth_child[j]]->p_table.potentials[k] *= multi_pt[j].potentials[k];
//                    }
//                }

                timer->Start("parallel");
                omp_set_num_threads(num_threads);
#pragma omp parallel for
                for (int s = 0; s < final_sum2; ++s) {
                    // compute j and k
                    int j = -1;
                    for (int m = process_size - 1; m >= 0; --m) {
                        if (s >= cum_sum2[m]) {
                            j = m;
                            break;
                        }
                    }
                    int k = s - cum_sum2[j];

//                    nodes_by_level[i][has_kth_child[j]]->p_table.Normalize();
                    nodes_by_level[i][has_kth_child[j]]->p_table.potentials[k] *= multi_pt[j].potentials[k];
                }
                timer->Stop("parallel");

                delete[] cum_sum2;

                timer->Stop("post-up-clq");
            }
        }

//        /**
//         * there are some issues with datasets munin2, munin3, munin4
//         * after debugging -- caused by table multiplication
//         * don't have enough precision so it may cause 0 prob after multiplication
//         * therefore, I add a normalization after collection of each level
//         * we can remove this part for other datasets
//         */
//        timer->Start("norm");
//        omp_set_num_threads(num_threads);
//#pragma omp parallel for
//        for (int i = 0; i < vector_clique_ptr_container.size(); ++i) {
//            vector_clique_ptr_container[i]->p_table.Normalize();
//        }
//#pragma omp parallel for
//        for (int i = 0; i < vector_separator_ptr_container.size(); ++i) {
//            vector_separator_ptr_container[i]->p_table.Normalize();
//        }
//        timer->Stop("norm");
    }
}

void JunctionTree::Distribute(int num_threads, Timer *timer) {
    for (int i = 1; i < max_level; ++i) { // for each level

        if (i % 2) {
            /**
             * case 1: levels 1, 3, 5, ... are separator levels
             * distribute msg from its parent (a clique) to it (a separator)
             * do marginalization + division for separator levels
             */
            SeparatorLevelOperation(false, i, num_threads, timer);
        }
        else {
            /**
             * case 2: levels 0, 2, 4,... are clique levels
             * distribute msg from its parent (a separator) to it (a clique)
             * do extension + multiplication for clique levels
             */
            timer->Start("pre-down-clq");
            int size = nodes_by_level[i].size();

            vector<PotentialTable> tmp_pt;
            tmp_pt.reserve(2 * size);

            vector<PotentialTable> multi_pt;
            multi_pt.resize(size);

            // store number_variables and cum_levels of the original table
            // rather than storing the whole potential table
            int *nv_old = new int[2 * size];
            vector<vector<int>> cl_old;
            cl_old.reserve(2 * size);

            int *cum_sum = new int[2 * size];
            int final_sum = 0;
            int sum_index = 0;

            // not all tables need to do the extension
            // there are "2 * size" tables in total
            // use a vector to show which tables need to do the extension
            vector<int> vector_extension;
            vector_extension.reserve(size);

            // set of arrays, showing the locations of the variables of the new table in the old table
            int **loc_in_new = new int*[2 * size];
            int **full_config = new int*[2 * size];
            int **partial_config = new int*[2 * size];
            int **table_index = new int*[2 * size];

            /**
             * pre computing
             */
            for (int j = 0; j < size; ++j) { // for each clique in this level
                auto clique = nodes_by_level[i][j];
                auto par = clique->ptr_upstream_clique;

                multi_pt[j] = par->p_table;

                // pre processing for extension
                set<int> all_related_variables;
                set<int> diff1, diff2;
                clique->p_table.MultiplicationPre(par->p_table, all_related_variables, diff1, diff2);

                if (diff1.empty() && diff2.empty()) { // if both table1 and table2 should not be extended
                    // do nothing
                } else if (!diff1.empty() && diff2.empty()) { // if table1 should be extended and table2 not
                    // record the index (that requires to do the extension)
                    vector_extension.push_back(j * 2 + 0);
                    nv_old[sum_index] = clique->p_table.num_variables;
                    cl_old.push_back(clique->p_table.cum_levels);

                    PotentialTable pt;
                    pt.ExtensionPre(all_related_variables, par->p_table.var_dims);

                    // get this table's location -- it is currently the last one
                    int last = vector_extension.size() - 1;
                    // generate an array showing the locations of the variables of the new table in the old table
                    loc_in_new[last] = new int[clique->p_table.num_variables];
                    int k = 0;
                    for (auto &v: clique->p_table.related_variables) {
                        loc_in_new[last][k++] = pt.GetVariableIndex(v);
                    }
                    table_index[last] = new int[pt.table_size];

                    tmp_pt.push_back(pt);

                    // malloc in pre-, not to parallelize
                    full_config[last] = new int[pt.table_size * pt.num_variables];
                    partial_config[last] = new int[pt.table_size * clique->p_table.num_variables];

                    // update sum
                    cum_sum[sum_index++] = final_sum;
                    final_sum += pt.table_size;
                } else if (diff1.empty() && !diff2.empty()) { // if table2 should be extended and table1 not
                    // record the index (that requires to do the extension)
                    vector_extension.push_back(j * 2 + 1);
                    nv_old[sum_index] = par->p_table.num_variables;
                    cl_old.push_back(par->p_table.cum_levels);

                    PotentialTable pt;
                    pt.ExtensionPre(all_related_variables, clique->p_table.var_dims);

                    // get this table's location -- it is currently the last one
                    int last = vector_extension.size() - 1;
                    // generate an array showing the locations of the variables of the new table in the old table
                    loc_in_new[last] = new int[par->p_table.num_variables];
                    int k = 0;
                    for (auto &v: par->p_table.related_variables) {
                        loc_in_new[last][k++] = pt.GetVariableIndex(v);
                    }
                    table_index[last] = new int[pt.table_size];

                    tmp_pt.push_back(pt);

                    // malloc in pre-, not to parallelize
                    full_config[last] = new int[pt.table_size * pt.num_variables];
                    partial_config[last] = new int[pt.table_size * par->p_table.num_variables];

                    // update sum
                    cum_sum[sum_index++] = final_sum;
                    final_sum += pt.table_size;
                } else { // if both table1 and table2 should be extended
                    // record the index (that requires to do the extension)
                    vector_extension.push_back(j * 2 + 0);
                    vector_extension.push_back(j * 2 + 1);
                    nv_old[sum_index] = clique->p_table.num_variables;
                    cl_old.push_back(clique->p_table.cum_levels);
                    nv_old[sum_index + 1] = par->p_table.num_variables;
                    cl_old.push_back(par->p_table.cum_levels);

                    PotentialTable tmp_pta, tmp_ptb;

                    vector<int> dims; // to save dims of the new related variables
                    dims.reserve(all_related_variables.size());
                    // to find the location of each new related variable
                    for (auto &v: all_related_variables) {
                        int loc = clique->p_table.GetVariableIndex(v);
                        if (loc < clique->p_table.related_variables.size()) { // find it in table1
                            dims.push_back(clique->p_table.var_dims[loc]);
                        } else { // cannot find in table1, we need to find it in table2
                            loc = par->p_table.GetVariableIndex(v);
                            dims.push_back(par->p_table.var_dims[loc]);
                        }
                    }

                    tmp_pta.ExtensionPre(all_related_variables, dims);
                    tmp_ptb.ExtensionPre(all_related_variables, dims);

                    // get this table's location -- it is currently the last one
                    int last = vector_extension.size() - 1;
                    // generate an array showing the locations of the variables of the new table in the old table
                    loc_in_new[last - 1] = new int[clique->p_table.num_variables];
                    int k = 0;
                    for (auto &v: clique->p_table.related_variables) {
                        loc_in_new[last][k++] = tmp_pta.GetVariableIndex(v);
                    }
                    table_index[last] = new int[tmp_pta.table_size];

                    tmp_pt.push_back(tmp_pta);

                    // malloc in pre-, not to parallelize
                    full_config[last - 1] = new int[tmp_pta.table_size * tmp_pta.num_variables];
                    partial_config[last] = new int[tmp_pta.table_size * clique->p_table.num_variables];

                    // generate an array showing the locations of the variables of the new table in the old table
                    loc_in_new[last] = new int[par->p_table.num_variables];
                    k = 0;
                    for (auto &v: par->p_table.related_variables) {
                        loc_in_new[last][k++] = tmp_ptb.GetVariableIndex(v);
                    }
                    table_index[last] = new int[tmp_ptb.table_size];

                    tmp_pt.push_back(tmp_ptb);

                    // malloc in pre-, not to parallelize
                    full_config[last] = new int[tmp_ptb.table_size * tmp_ptb.num_variables];
                    partial_config[last] = new int[tmp_ptb.table_size * par->p_table.num_variables];

                    // update sum
                    cum_sum[sum_index++] = final_sum;
                    final_sum += tmp_pta.table_size;
                    cum_sum[sum_index++] = final_sum;
                    final_sum += tmp_ptb.table_size;
                }
            }
            timer->Stop("pre-down-clq");

            timer->Start("main-down-clq");
            int size_e = vector_extension.size(); // the number of variables to be extended

            timer->Start("parallel");
            // the main loop
            omp_set_num_threads(num_threads);
#pragma omp parallel for
            for (int s = 0; s < final_sum; ++s) {
                // compute j and k
                int j = -1;
                for (int m = size_e - 1; m >= 0; --m) {
                    if (s >= cum_sum[m]) {
                        j = m;
                        break;
                    }
                }
                int k = s - cum_sum[j];

                // 1. get the full config value of new table
                tmp_pt[j].GetConfigValueByTableIndex(k, full_config[j] + k * tmp_pt[j].num_variables);
                // 2. get the partial config value from the new table
                for (int l = 0; l < nv_old[j]; ++l) {
                    partial_config[j][k * nv_old[j] + l] = full_config[j][k * tmp_pt[j].num_variables + loc_in_new[j][l]];
                }
                // 3. obtain the potential index
                table_index[j][k] = tmp_pt[j].GetTableIndexByConfigValue(partial_config[j] + k * nv_old[j], nv_old[j], cl_old[j]);
            }
            timer->Stop("parallel");
            timer->Stop("main-down-clq");

            timer->Start("post-down-clq");
            // post-computing
            timer->Start("post-down-clq-mem");
            int *cum_sum2 = new int[size];
            int final_sum2 = 0;

            int l = 0;
            for (int j = 0; j < size; ++j) {
                auto clique = nodes_by_level[i][j];
                auto par = clique->ptr_upstream_clique;

                int m = j * 2 + 0;
                if (l < size_e && m == vector_extension[l]) { // index k have done the extension
                    for (int k = 0; k < tmp_pt[l].table_size; ++k) {
                        // 4. potential[table_index]
                        tmp_pt[l].potentials[k] = clique->p_table.potentials[table_index[l][k]];
                    }
                    clique->p_table = tmp_pt[l];
                    l++;
                }

                m = j * 2 + 1;
                if (l < size_e && m == vector_extension[l]) { // index j have done the extension
                    for (int k = 0; k < tmp_pt[l].table_size; ++k) {
                        // 4. potential[table_index]
                        tmp_pt[l].potentials[k] = par->p_table.potentials[table_index[l][k]];
                    }
                    multi_pt[j] = tmp_pt[l];
                    l++;
                }

                cum_sum2[j] = final_sum2;
                final_sum2 += multi_pt[j].table_size;
            }
            timer->Stop("post-down-clq-mem");

            timer->Start("post-down-clq-del");
            for (int l = 0; l < size_e; ++l) {
                delete[] loc_in_new[l];
                delete[] full_config[l];
                delete[] partial_config[l];
                delete[] table_index[l];
            }

            delete[] loc_in_new;
            delete[] full_config;
            delete[] partial_config;
            delete[] table_index;
            delete[] cum_sum;
            delete[] nv_old;
            timer->Stop("post-down-clq-del");

            timer->Start("post-down-clq-mul");
            timer->Start("parallel");
            omp_set_num_threads(num_threads);
#pragma omp parallel for
            for (int s = 0; s < final_sum2; ++s) {
                // compute j and k
                int j = -1;
                for (int m = size - 1; m >= 0; --m) {
                    if (s >= cum_sum2[m]) {
                        j = m;
                        break;
                    }
                }
                int k = s - cum_sum2[j];

                nodes_by_level[i][j]->p_table.potentials[k] *= multi_pt[j].potentials[k];
            }
            timer->Stop("parallel");

            delete[] cum_sum2;

//            for (int j = 0; j < size; ++j) {
//                for (int k = 0; k < multi_pt[j].table_size; ++k) {
//                    nodes_by_level[i][j]->p_table.potentials[k] *= multi_pt[j].potentials[k];
//                }
//            }
            timer->Stop("post-down-clq-mul");

            timer->Stop("post-down-clq");
        }
    }
}

/**
 * @brief: compute the marginal distribution for a query variable
 **/
PotentialTable JunctionTree::BeliefPropagationCalcuDiscreteVarMarginal2(int query_index) {

    // The input is a set of query_indexes of variables.
    // The output is a factor representing the joint marginal of these variables.
    // TODO: here only support one query variable

    int min_size = INT32_MAX;
    Clique *selected_clique = nullptr;

    // The case where the query variables are all appear in one clique.
    // Find the clique that contains this variable,
    // whose size of potentials table is the smallest,
    // which can reduce the number of sum operation.
    // TODO: find from separator
    for (auto &c : tree->vector_clique_ptr_container) {

        if (!c->pure_discrete) {
            continue;
        }
        if (c->p_table.related_variables.find(query_index) == c->p_table.related_variables.end()) { // cannot find the query variable
            continue;
        }
        if (c->p_table.related_variables.size() >= min_size) {
            continue;
        }
        min_size = c->p_table.related_variables.size();
        selected_clique = c;
    }

    if (selected_clique == nullptr) {
        fprintf(stderr, "Error in function [%s]\n"
                        "Variable [%d] does not appear in any clique!", __FUNCTION__, query_index);
        exit(1);
    }

    set<int> other_vars = selected_clique->p_table.related_variables;
    other_vars.erase(query_index);

    PotentialTable pt = selected_clique->p_table;

    pt.TableMarginalization(other_vars);

    pt.Normalize(); // todo: no need to do normalization

    return pt;
}

/**
 * @brief: predict the label for a given variable.
 */
int JunctionTree::InferenceUsingBeliefPropagation(int &query_index) {

    PotentialTable pt = BeliefPropagationCalcuDiscreteVarMarginal2(query_index);
    double max_prob = 0;
    int max_index;
    for (int i = 0; i < pt.table_size; ++i) { // traverse the potential table
        if (pt.potentials[i] > max_prob) {
            max_prob = pt.potentials[i];
            max_index = i;
        }
    }

    // "pt" has only one related variable, which is exactly the query variable,
    // so the "max_index" exactly means which value of the query variable gets the max probability
    auto dn = dynamic_cast<DiscreteNode*>(network->FindNodePtrByIndex(query_index));
    int label_predict = dn->vec_potential_vals[max_index];

  return label_predict;
}

/**
 * @brief: test the Junction Tree given a data set
 */
double JunctionTree::EvaluateAccuracy(Dataset *dts, int num_threads, int num_samp, string alg, bool is_dense) {

  cout << "==================================================" << '\n'
       << "Begin testing the trained network." << endl;

    Timer *timer = new Timer();
    // record time
    timer->Start("jt");

    int m = dts->num_instance;

    int class_var_index = dts->class_var_index;

    vector<DiscreteConfig> evidences;
    evidences.reserve(m);
    vector<int> ground_truths;
    ground_truths.reserve(m);

    for (int i = 0; i < m; ++i) {  // For each sample in test set
        vector<VarVal> vec_instance = dts->vector_dataset_all_vars.at(i);

        // construct an evidence by removing the class variable
        DiscreteConfig e;
        pair<int, int> p;
        for (int j = 0; j < vec_instance.size(); ++j) {
            if (j == class_var_index) { // skip the class variable
                continue;
            }
            p.first = vec_instance.at(j).first;
            p.second = vec_instance.at(j).second.GetInt();
            e.insert(p);
        }

        if (is_dense) {
            e = Sparse2Dense(e, network->num_nodes, class_var_index);
        }
        evidences.push_back(e);

        // construct the ground truth
        int g = vec_instance.at(class_var_index).second.GetInt();
        ground_truths.push_back(g);
    }

    // predict the labels of the test instances
    vector<int> predictions = PredictUseJTInfer(evidences, class_var_index, num_threads, timer);

    double accuracy = Accuracy(ground_truths, predictions);

    timer->Stop("jt");
    setlocale(LC_NUMERIC, "");

//    double accuracy = num_of_correct / (double)(num_of_correct+num_of_wrong);
    cout << '\n' << "Accuracy: " << accuracy << endl;
    cout << "==================================================" << endl;
    timer->Print("jt"); cout << "after removing reset time: " << timer->time["jt"] - timer->time["reset"] << " s"<< endl;
    double total = timer->time["jt"] - timer->time["reset"] - timer->time["norm"]; cout << "after removing norm time: " << total << " s"<< endl;
    timer->Print("load evidence"); cout << "(" << timer->time["load evidence"] / timer->time["jt"] * 100 << "%)" << endl;
    timer->Print("msg passing"); cout << "(" << timer->time["msg passing"] / timer->time["jt"] * 100 << "%)" << endl;
    timer->Print("upstream"); cout << endl;
    timer->Print("downstream"); cout << endl;
    timer->Print("predict"); cout << "(" << timer->time["predict"] / timer->time["jt"] * 100 << "%)" << endl;
    timer->Print("reset"); cout << "(" << timer->time["reset"] / timer->time["jt"] * 100 << "%)" << endl << endl;

    timer->Print("pre-evi"); timer->Print("main-evi"); timer->Print("post-evi"); cout << endl << endl;
    timer->Print("pre-down-sep"); timer->Print("main-down-sep"); timer->Print("post-down-sep"); cout << endl;
    timer->Print("pre-down-clq"); timer->Print("main-down-clq"); timer->Print("post-down-clq"); cout << endl;
    timer->Print("pre-up-sep"); timer->Print("main-up-sep"); timer->Print("post-up-sep"); cout << endl;
    timer->Print("pre-up-clq"); timer->Print("main-up-clq"); timer->Print("post-up-clq"); cout << endl << endl;

    timer->Print("post-down-clq-mem"); timer->Print("post-down-clq-del"); timer->Print("post-down-clq-mul"); cout << endl;
    timer->Print("post-down-sep-mem"); timer->Print("post-sep-del"); timer->Print("post-down-sep-div"); cout << endl << endl;

    timer->Print("norm"); cout << endl << endl;

    timer->Print("parallel");
    cout << "(" << timer->time["parallel"] / (timer->time["jt"] - timer->time["norm"])* 100 << "%)";
    cout << "(" << timer->time["parallel"] / total * 100 << "%)" << endl;

    delete timer;
    timer = nullptr;

    return accuracy;
}

/**
 * @brief: predict label given evidence E and target variable id Y_index
 * @return label of the target variable
 */
int JunctionTree::PredictUseJTInfer(const DiscreteConfig &E, int Y_index, int num_threads, Timer *timer) {
    timer->Start("load evidence");
    //update a clique using the evidence
    LoadDiscreteEvidence(E, num_threads, timer);
    timer->Stop("load evidence");
//    cout << "finish load evidence" << endl;

    timer->Start("msg passing");
    //update the whole Junction Tree
    MessagePassingUpdateJT(num_threads, timer);
    timer->Stop("msg passing");
//    cout << "finish msg passing" << endl;

    timer->Start("predict");
    int label_predict = InferenceUsingBeliefPropagation(Y_index);
    timer->Stop("predict");
//    cout << "finish predict " << endl;

    timer->Start("reset");
    ResetJunctionTree();
    timer->Stop("reset");
//    cout << "finish reset" << endl;
    return label_predict;
}

/**
 * @brief: predict the labels given different evidences
 * it just repeats the function above multiple times, and print the progress at the meantime
 */
vector<int> JunctionTree::PredictUseJTInfer(const vector<DiscreteConfig> &evidences, int target_node_idx,
                                            int num_threads, Timer *timer) {
    int size = evidences.size();

    cout << "Progress indicator: ";
    int every_1_of_20 = size / 20; // used to print, print 20 times in total
    int progress = 0;

    vector<int> results(size, 0);

    for (int i = 0; i < size; ++i) {
        ++progress;

        if (progress % every_1_of_20 == 0) {
            string progress_percentage = to_string((double)progress/size * 100) + "%...\n";
            fprintf(stdout, "%s\n", progress_percentage.c_str());
//            double acc_so_far = num_of_correct / (double)(num_of_correct+num_of_wrong);
//            fprintf(stdout, "Accuracy so far: %f\n", acc_so_far);
            fflush(stdout);
        }

        int label_predict = PredictUseJTInfer(evidences.at(i), target_node_idx, num_threads, timer);
        results.at(i) = label_predict;
    }
    return results;
}