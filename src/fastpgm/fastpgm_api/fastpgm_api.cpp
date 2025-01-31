//
// Created by jjt on 21/03/24.
//
#include "fastpgm/fastpgm_api/fastpgm_api.h"

string GetLastPath(const string& path) {
    size_t last_slash_pos = path.find_last_of('/');

    // if path doesn't have slash, return empty string
    if (last_slash_pos == string::npos) {
        return "";
    }

    // remove the content after slash
    return path.substr(0, last_slash_pos);
}

void BNSL_PCStable(int verbose, int n_threads, int group_size,
                   double alpha, string ref_net, string train_set,
                   bool save_struct) {

    Dataset *trainer = new Dataset(); // todo: decide whether the dataset is in libsvm format or in csv format
    trainer->LoadCSVTrainingData(train_set, true, 0);

    Network *network = new Network(true);
    StructureLearning *bnsl = new PCStable(network, alpha);
    // for structure learning, we don't change to dag or add ROOT
    bnsl->StructLearnCompData(trainer, group_size, n_threads,false, false,
                              save_struct, train_set + "_struct", verbose);
    SAFE_DELETE(trainer);

    if (!ref_net.empty()) {
        CustomNetwork *r_net = new CustomNetwork();
        r_net->LoadBIFFile(ref_net);
        BNSLComparison comp(r_net, network);
        int shd = comp.GetSHD();
        cout << "SHD = " << shd << endl;
        SAFE_DELETE(r_net);
    } else {
        cout << "There is no reference BN (ground-truth) provided, comparison (showing accuracy) is skipped." << endl;
    }

    SAFE_DELETE(network);
}

void BNL_PCStable(int verbose, int n_threads, int group_size,
                  double alpha, string ref_net, string train_set,
                  bool save_struct, bool save_param) {

    Dataset *trainer = new Dataset(); // todo: decide whether the dataset is in libsvm format or in csv format
    trainer->LoadCSVTrainingData(train_set, true, 0);

    Network *network = new Network(true);
    StructureLearning *bnsl = new PCStable(network, alpha);
    // for learning, we don't change to dag or add ROOT
    bnsl->StructLearnCompData(trainer, group_size, n_threads,false, false,
                              save_struct, train_set + "_struct", verbose);

    if (!ref_net.empty()) {
        CustomNetwork *r_net = new CustomNetwork();
        r_net->LoadBIFFile(ref_net);
        BNSLComparison comp(r_net, network);
        int shd = comp.GetSHD();
        cout << "SHD = " << shd << endl;
        SAFE_DELETE(r_net);
    } else {
        cout << "There is no reference BN (ground-truth) provided, comparison (showing accuracy) is skipped." << endl;
    }

    ParameterLearning *bnpl = new ParameterLearning(network);
    bnpl->LearnParamsKnowStructCompData(trainer, 1, save_param, train_set + "_param", verbose);

    SAFE_DELETE(trainer);
    SAFE_DELETE(network);
}

void BNEI_JT(int verbose, int n_threads,
             string net, string test_set, string pt) {

    CustomNetwork *network = new CustomNetwork(true);
    // todo: network file format
    network->LoadXMLBIFFile(net, 1);

    Dataset *tester = new Dataset(network);
    // todo: dataset file format
    tester->LoadCSVTestingData(test_set, true, 0);

    Inference *inference = new JunctionTree(0, network, tester);
    SAFE_DELETE(tester);

    double accuracy = inference->EvaluateAccuracy(pt, n_threads);
    SAFE_DELETE(inference);
    SAFE_DELETE(network);
}

void C_PCStable_JT(int verbose, int n_threads, int group_size, double alpha,
                   string ref_net, string train_set, string test_set,
                   bool save_struct, bool save_param) {

    Dataset *trainer = new Dataset(); // todo: decide whether the dataset is in libsvm format or in csv format
    trainer->LoadCSVTrainingData(train_set, true, 0);

    Dataset *tester = new Dataset(trainer);
    tester->LoadCSVTestingData(test_set, true, 0);

    Network *network = new Network(true);
    StructureLearning *bnsl = new PCStable(network, alpha);
    // for classification, we change to dag and add ROOT
    bnsl->StructLearnCompData(trainer, group_size, n_threads,true, true,
                              save_struct, train_set + "_struct", verbose);

    if (!ref_net.empty()) {
        CustomNetwork *r_net = new CustomNetwork();
        r_net->LoadBIFFile(ref_net);
        BNSLComparison comp(r_net, network);
        int shd = comp.GetSHD();
        cout << "SHD = " << shd << endl;
        SAFE_DELETE(r_net);
    } else {
        cout << "There is no reference BN (ground-truth) provided, comparison (showing accuracy) is skipped." << endl;
    }

    ParameterLearning *bnpl = new ParameterLearning(network);
    bnpl->LearnParamsKnowStructCompData(trainer, 1, save_param,train_set + "_param", verbose);
    SAFE_DELETE(trainer);

    Inference *inference = new JunctionTree(1, network, tester);
    SAFE_DELETE(tester);

    double accuracy = inference->EvaluateAccuracy("", n_threads);
    cout << "accuracy = " << accuracy << endl;
    SAFE_DELETE(inference);
    SAFE_DELETE(bnsl);
    SAFE_DELETE(bnpl);
}

void Sample_Generator(int verbose, int n_threads, string net, bool libsvm, int num_samples,
                      int class_variable) {

    CustomNetwork *network = new CustomNetwork(true);
    // todo: network file format
    network->LoadXMLBIFFile(net, 1);

    SampleSetGenerator *sample_set = new SampleSetGenerator(network, num_samples,
                                                            class_variable);
    sample_set->GenerateSamplesBasedOnCPTs();
    string libsvm_path = GetLastPath(net) + "/sample_" + to_string(num_samples) + "_libsvm";
    sample_set->OutputLIBSVM(libsvm_path);
    SAFE_DELETE(sample_set);

    if (libsvm) {
        SAFE_DELETE(network);
    } else {
        // for the case of generating sample set in CSV format, we use the convertor to convert
        // LibSVM to CSV.
        Dataset *dts = new Dataset(network);
        dts->LoadLIBSVMTestingData(libsvm_path, class_variable);

        dts->vec_var_names.resize(dts->num_vars);
        for (int i = 0; i < dts->num_vars; ++i) {
            Node *node = network->FindNodePtrByIndex(i);
            dts->vec_var_names[i] = node->node_name;
        }
        SAFE_DELETE(network);

        string csv_path = GetLastPath(net) + "/sample_" + to_string(num_samples) + "_csv";
        dts->StoreCSVData(csv_path, true);
        SAFE_DELETE(dts);
    }
}