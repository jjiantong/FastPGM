#include "JunctionTree.h"

JunctionTree::JunctionTree(Network *net, Dataset *dts, bool is_dense) : Inference(net, dts, is_dense) {
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
    SAFE_DELETE(timer);
}

JunctionTree::~JunctionTree() {
    SAFE_DELETE(tree);
    SAFE_DELETE_ARRAY(clique_backup);
    SAFE_DELETE_ARRAY(separator_backup);
}

/**
 * @brief: test the Junction Tree given a data set
 */
double JunctionTree::EvaluateAccuracy(int num_threads) {

    cout << "==================================================" << '\n'
         << "Begin testing the trained network." << endl;

    Timer *timer = new Timer();
    // record time
    timer->Start("jt");

    // predict the labels of the test instances
    vector<int> predictions = PredictUseJTInfer(num_threads, timer);

    double accuracy = Accuracy(predictions);

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

    SAFE_DELETE(timer);

    return accuracy;
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
    /**
     * version 1: use TableReduction method
     * including both nested and flattened version
     */
    for (auto &e: E) { // for each observation of variable
        timer->Start("pre-evi");
        // we need the index of the the evidence and the value index
        int index = e.first;
        int value = e.second;
        if (index >= network->num_nodes) {
            cout << "!!!!" << endl;
            // todo: this is because the testing set has more features than the training set
            //  for this case, we just ignore such evidence in the current implementation
            continue;
        }

//        /**
//         * 1. nested version, without flattening
//         */
//        for (auto &c: tree->vector_clique_ptr_container) {
//            if (c->p_table.related_variables.find(index) != c->p_table.related_variables.end()) {
//                c->p_table.TableReduction(index, value, num_threads);
//            }
//        }
//        for (auto &c: tree->vector_separator_ptr_container) {
//            if (c->p_table.related_variables.find(index) != c->p_table.related_variables.end()) {
//                c->p_table.TableReduction(index, value, num_threads);
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

        LoadEvidenceToNodes(vector_reduced_clique_and_separator_ptr, index, value, num_threads, timer);
    }

//    /**
//     * version 2: use TableReduction v2 method
//     * TODO: create the flattened version
//     */
//    for (auto &c: tree->vector_clique_ptr_container) {
//        c->p_table.TableReduction(E, num_threads);
//    }
//    for (auto &c: tree->vector_separator_ptr_container) {
//        c->p_table.TableReduction(E, num_threads);
//    }
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
        full_config[k] = new int[clique_ptr->p_table.table_size * clique_ptr->p_table.num_variables];
        v_index[k] = new int[clique_ptr->p_table.table_size];

        e_loc[k] = clique_ptr->p_table.TableReductionPre(index);
    }

    timer->Stop("pre-evi");

    timer->Start("main-evi");
    // the main loop
    omp_set_num_threads(num_threads);
#pragma omp parallel for
    for (int s = 0; s < final_sum; ++s) {
        // compute k and i
        int k, i;
        Compute2DIndex(k, i, s, red_size, cum_sum);

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
        clique_ptr->p_table.TableReductionPost(index, value_index, v_index[k], e_loc[k]);

        SAFE_DELETE_ARRAY(full_config[k]);
        SAFE_DELETE_ARRAY(v_index[k]);
    }
    timer->Stop("parallel");

    SAFE_DELETE_ARRAY(e_loc);
    SAFE_DELETE_ARRAY(full_config);
    SAFE_DELETE_ARRAY(v_index);
    SAFE_DELETE_ARRAY(cum_sum);
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
 *
 * the only difference between collection and distribution is:
 *      collection finds each separator and its child, marginalizes from child to it
 *      distribution finds each separator and its parent, marginalizes from parent to it
 * this difference affects three for loops
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

    timer->Start("pre-up-sep");
    for (int j = 0; j < size; ++j) {
        Separator *separator = separators_by_level[i/2][j];
        Clique *mar_clique;
        if (is_collect) {
            mar_clique = separator->ptr_downstream_cliques[0]; // there is only one child for each separator
        } else {
            mar_clique = separator->ptr_upstream_clique;
        }
        // update sum
        cum_sum[j] = final_sum;
        final_sum += mar_clique->p_table.table_size;
    }

    /**
     * pre computing
     */
    timer->Start("parallel");
    omp_set_num_threads(num_threads);
#pragma omp parallel for
    for (int j = 0; j < size; ++j) { // for each separator in this level
        auto separator = separators_by_level[i/2][j];
        Clique *mar_clique;
        if (is_collect) {
            mar_clique = separator->ptr_downstream_cliques[0]; // there is only one child for each separator
        } else {
            mar_clique = separator->ptr_upstream_clique;
        }

        // store the old table before marginalization, used for division
        separator->old_ptable = separator->p_table;

        // store the child's table, used for update the "its" table
        nv_old[j] = mar_clique->p_table.num_variables;
        cl_old[j] = mar_clique->p_table.cum_levels;

        // find the variables to be marginalized
        set<int> set_external_vars;
        set_difference(mar_clique->p_table.related_variables.begin(), mar_clique->p_table.related_variables.end(),
                       separator->clique_variables.begin(), separator->clique_variables.end(),
                       inserter(set_external_vars, set_external_vars.begin()));

        int num_vars = mar_clique->p_table.num_variables - set_external_vars.size();
        // generate an array showing the locations of the variables of the new table in the old table
        loc_in_old[j] = new int[num_vars];
        table_index[j] = new int[mar_clique->p_table.table_size];
        full_config[j] = new int[mar_clique->p_table.table_size * mar_clique->p_table.num_variables];
        partial_config[j] = new int[mar_clique->p_table.table_size * num_vars];

        mar_clique->p_table.TableMarginalizationPre(set_external_vars, tmp_pt[j]);
        int k = 0;
        for (auto &v: tmp_pt[j].related_variables) {
            // just to avoid using "GetVariableIndex"
            loc_in_old[j][k++] = mar_clique->p_table.TableReductionPre(v);
        }
    }
    timer->Stop("pre-up-sep");

    timer->Start("main-up-sep");
    // the main loop
    omp_set_num_threads(num_threads);
#pragma omp parallel for
    for (int s = 0; s < final_sum; ++s) {
        int j, k;
        Compute2DIndex(j, k, s, size, cum_sum); // compute j and k
        table_index[j][k] = tmp_pt[j].TableMarginalizationMain(k, full_config[j], partial_config[j],
                                                               nv_old[j], cl_old[j], loc_in_old[j]);
    }
    timer->Stop("main-up-sep");

    timer->Start("post-up-sep");
    // post-computing
    omp_set_num_threads(num_threads);
#pragma omp parallel for
    for (int j = 0; j < size; ++j) { // for each separator in this level
        SAFE_DELETE_ARRAY(loc_in_old[j]);
        SAFE_DELETE_ARRAY(full_config[j]);
        SAFE_DELETE_ARRAY(partial_config[j]);

        auto separator = separators_by_level[i/2][j];
        Clique *mar_clique;
        if (is_collect) {
            mar_clique = separator->ptr_downstream_cliques[0]; // there is only one child for each separator
        } else {
            mar_clique = separator->ptr_upstream_clique;
        }

        tmp_pt[j].TableMarginalizationPost(mar_clique->p_table, table_index[j]);
        SAFE_DELETE_ARRAY(table_index[j]);

        separator->p_table = tmp_pt[j];
        separator->p_table.TableDivision(separator->old_ptable);
    }
    timer->Stop("parallel");
    timer->Stop("post-up-sep");
    timer->Start("post-sep-del");
    SAFE_DELETE_ARRAY(loc_in_old);
    SAFE_DELETE_ARRAY(full_config);
    SAFE_DELETE_ARRAY(partial_config);
    SAFE_DELETE_ARRAY(table_index);
    SAFE_DELETE_ARRAY(cum_sum);
    SAFE_DELETE_ARRAY(nv_old);
    timer->Stop("post-sep-del");
}

/**
 * operations for cliques, including extension and multiplication
 * the multiplication is conducted on a clique and a separator,
 * so the separator may be extended, but the clique is never extended. (!!!)
 * the difference between collection and distribution:
 *      distribution finds each clique and its parent, extends the parent, then multiplies them
 *      collection finds each clique and its k-th child, extends the child, then multiplies them
 * @param is_collect true for upstream passing, false for downstream passing
 * @param i corresponds to level
 * @param size number of clique-separator pairs to be processed
 * @param has_kth_child an array containing all cliques in the current level that has the k-th child; only used for collection
 * @param k we need "k"-th child; only used for collection
 */
void JunctionTree::CliqueLevelOperation(bool is_collect, int i, int size,
                                        const vector<int> &has_kth_child, int k,
                                        int num_threads, Timer *timer) {
    timer->Start("pre-down-clq");

    /**
     * the purpose of this part is to avoid extension and multiplication when the separator has no related variable
     * i.e., the potential table of separator is "1 vector" (1, 1, ..., 1)
     * however, the implementation is wrong because we perform all the operations on "size" cliques together
     * so we cannot just "return" if we find one of the related separator has no related variable
     */
//    for (int j = 0; j < size; ++j) { // for each clique in this level
//        Clique *clique, *separator;
//        if (is_collect) {
//            clique = nodes_by_level[i][has_kth_child[j]];
//            separator = clique->ptr_downstream_cliques[k];
//        } else {
//            clique = nodes_by_level[i][j];
//            separator = clique->ptr_upstream_clique;
//        }
//
//        if (separator->p_table.related_variables.empty()) {
//            return;
//        }
//    }

    // used to store the (separator) potential tables that are needed to be extended
    vector<PotentialTable> tmp_pt;
    tmp_pt.reserve(size);

    // used to store the (separator) potential tables that are used to be multiplied
    // it is different from "tmp_pt" because not all the tables are needed to be extended
    vector<PotentialTable> multi_pt;
    multi_pt.resize(size);

    // store number_variables and cum_levels of the original table
    // rather than storing the whole potential table
    int *nv_old = new int[size];
    vector<vector<int>> cl_old;
    cl_old.reserve(size);

    int *cum_sum = new int[size];
    int final_sum = 0;
    int sum_index = 0;

    // not all tables need to do the extension
    // there are "size" tables in total
    // use a vector to show which tables need to do the extension
    vector<int> vector_extension;
    vector_extension.reserve(size);

    // set of arrays, showing the locations of the variables of the new table in the old table
    int **loc_in_new = new int*[size];
    int **full_config = new int*[size];
    int **partial_config = new int*[size];
    int **table_index = new int*[size];

    /**
     * pre computing
     */
    for (int j = 0; j < size; ++j) { // for each clique in this level
        Clique *clique, *separator;
        if (is_collect) {
            clique = nodes_by_level[i][has_kth_child[j]];
            separator = clique->ptr_downstream_cliques[k];
        } else {
            clique = nodes_by_level[i][j];
            separator = clique->ptr_upstream_clique;
        }

        multi_pt[j] = separator->p_table;

        // pre processing for extension
        bool to_be_extended = clique->p_table.TableMultiplicationPre(separator->p_table);

        /**
         * we only need to decide whether the separator needs to be extended
         */
        if (to_be_extended) { // if the separator table should be extended
            // record the index (that requires to do the extension)
            vector_extension.push_back(j);
            nv_old[sum_index] = separator->p_table.num_variables;
            cl_old.push_back(separator->p_table.cum_levels);

            PotentialTable pt;
            pt.TableExtensionPre(clique->p_table.related_variables, clique->p_table.var_dims);

            // get this table's location -- it is currently the last one
            int last = vector_extension.size() - 1;
            // generate an array showing the locations of the variables of the new table in the old table
            loc_in_new[last] = new int[separator->p_table.num_variables];
            int k = 0;
            for (auto &v: separator->p_table.related_variables) {
                // just to avoid using "GetVariableIndex"
                loc_in_new[last][k++] = pt.TableReductionPre(v);
            }
            table_index[last] = new int[pt.table_size];

            tmp_pt.push_back(pt);

            // malloc in pre-, not to parallelize
            full_config[last] = new int[pt.table_size * pt.num_variables];
            partial_config[last] = new int[pt.table_size * separator->p_table.num_variables];

            // update sum
            cum_sum[sum_index++] = final_sum;
            final_sum += pt.table_size;
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
        int j, k;
        Compute2DIndex(j, k, s, size_e, cum_sum); // compute j and k
        table_index[j][k] = tmp_pt[j].TableExtensionMain(k, full_config[j], partial_config[j],
                                                         nv_old[j], cl_old[j], loc_in_new[j]);
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
        Clique *clique, *separator;
        if (is_collect) {
            clique = nodes_by_level[i][has_kth_child[j]];
            separator = clique->ptr_downstream_cliques[k];
        } else {
            clique = nodes_by_level[i][j];
            separator = clique->ptr_upstream_clique;
        }

        if (l < size_e && j == vector_extension[l]) { // index j have done the extension
            tmp_pt[l].TableExtensionPost(separator->p_table, table_index[l]);
            multi_pt[j] = tmp_pt[l];
            l++;
        }

        cum_sum2[j] = final_sum2;
        final_sum2 += multi_pt[j].table_size;
    }
    timer->Stop("post-down-clq-mem");

    timer->Start("post-down-clq-del");
    for (int l = 0; l < size_e; ++l) {
        SAFE_DELETE_ARRAY(loc_in_new[l]);
        SAFE_DELETE_ARRAY(full_config[l]);
        SAFE_DELETE_ARRAY(partial_config[l]);
        SAFE_DELETE_ARRAY(table_index[l]);
    }
    SAFE_DELETE_ARRAY(loc_in_new);
    SAFE_DELETE_ARRAY(full_config);
    SAFE_DELETE_ARRAY(partial_config);
    SAFE_DELETE_ARRAY(table_index);
    SAFE_DELETE_ARRAY(cum_sum);
    SAFE_DELETE_ARRAY(nv_old);
    timer->Stop("post-down-clq-del");

    timer->Start("post-down-clq-mul");
    timer->Start("parallel");
    omp_set_num_threads(num_threads);
#pragma omp parallel for
    for (int s = 0; s < final_sum2; ++s) {
        int j, k;
        Compute2DIndex(j, k, s, size, cum_sum2); // compute j and k
        if (is_collect) {
            nodes_by_level[i][has_kth_child[j]]->p_table.potentials[k] *= multi_pt[j].potentials[k];
        } else {
            nodes_by_level[i][j]->p_table.potentials[k] *= multi_pt[j].potentials[k];
        }
    }
    timer->Stop("parallel");
    SAFE_DELETE_ARRAY(cum_sum2);
    timer->Stop("post-down-clq-mul");
    timer->Stop("post-down-clq");
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

            // get the maximum number of children for the cliques in the current level
            int max_num_children = 0;
            for (int j = 0; j < size; ++j) { // for each clique of this level
                auto clique = nodes_by_level[i][j];
                // first, find the max number of children for this level
                if (clique->ptr_downstream_cliques.size() > max_num_children) {
                    max_num_children = clique->ptr_downstream_cliques.size();
                }
            }
            timer->Stop("pre-up-clq");

            /**
             * there may be multiple children for a clique
             * first process the first child of each clique in this level,
             * then the second child of each clique (if has), ...
             * until all the children of all the cliques in this level have been processed
             *
             */
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
                CliqueLevelOperation(true, i, process_size, has_kth_child, k, num_threads, timer);
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
            int size = nodes_by_level[i].size();
            CliqueLevelOperation(false, i, size, vector<int>(), -1, num_threads, timer);
        }
    }
}

/**
 * @brief: compute the marginal distribution for a query variable
 * @param query_index the index of query variable TODO: here only support one query variable
 * @return a potential table (factor) representing the marginal of the query variable
 **/
PotentialTable JunctionTree::CalculateMarginalProbability(int query_index) {

    int min_size = INT32_MAX;
    Clique *selected_clique = nullptr;

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
int JunctionTree::InferenceUsingJT(int &query_index) {

    PotentialTable pt = CalculateMarginalProbability(query_index);
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
    return max_index;
}

/**
 * @brief: predict label given evidence E and target variable id Y_index
 * @return label of the target variable
 */
int JunctionTree::PredictUseJTInfer(const DiscreteConfig &E, int num_threads, Timer *timer) {
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
    int label_predict = InferenceUsingJT(query_index);
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
vector<int> JunctionTree::PredictUseJTInfer(int num_threads, Timer *timer) {
    int size = evidences.size();

    cout << "Progress indicator: ";
    int every_1_of_20 = size / 20; // used to print, print 20 times in total
    int progress = 0;

    vector<int> results(size, 0);

    for (int i = 0; i < size; ++i) {
        ++progress;

        if (progress % every_1_of_20 == 0) {
            string progress_percentage = to_string((double)progress/size * 100) + "%...";
            fprintf(stdout, "%s\n", progress_percentage.c_str());
            fflush(stdout);
        }

        int label_predict = PredictUseJTInfer(evidences.at(i), num_threads, timer);
        results.at(i) = label_predict;
    }
    return results;
}