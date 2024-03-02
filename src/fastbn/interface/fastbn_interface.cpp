//
// Created by jjt on 2/03/24.
//
#include "fastbn/interface/fastbn_interface.h"

using namespace std;

string GetDataPath() {
    string path = __FILE__;
    string project = "FastBN";
    string project_root = path.substr(0, path.find(project) + project.length() + 1);
    string dpath = project_root + "dataset/";
    return dpath;
}

/**
 * Job = 0, method = 0: BN structure learning, PC-Stable
 */
void BNSLPCStable(int t, int g, int v, const char* refbn, const char* trainset, double a, bool stru) {
    cout << "==================================================" << endl;
    cout << "Job: PC-stable for structure learning, #threads = " << t << endl;
    cout << "\tgroup size = " << g << endl;
    cout << "\treference BN: " << refbn << endl;
    cout << "\tsample set: " << trainset << endl;
    cout << "==================================================" << endl;

    string dpath = GetDataPath();
    string s_refbn = refbn;
    string s_trainset = trainset;

    Dataset *trainer = new Dataset(); // todo: decide whether the dataset is in libsvm format or in csv format
    trainer->LoadCSVTrainingData(dpath + s_trainset, true, true, 0);

    Network *network = new Network(true);
    StructureLearning *bnsl = new PCStable(network, a);
    bnsl->StructLearnCompData(trainer, g, t,false, false,
                              stru, dpath + s_trainset + "_struct", v);
    SAFE_DELETE(trainer);

    if (!s_refbn.empty()) {
        CustomNetwork *ref_net = new CustomNetwork();
        ref_net->LoadBIFFile(dpath + s_refbn);
        BNSLComparison comp(ref_net, network);
        int shd = comp.GetSHD();
        cout << "SHD = " << shd << endl;
        SAFE_DELETE(ref_net);
    } else {
        cout << "There is no reference BN (ground-truth) provided, so BN comparison (showing accuracy) is skipped." << endl;
    }

    SAFE_DELETE(network);
}
