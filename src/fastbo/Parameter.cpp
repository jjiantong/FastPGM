//
// Created by jjt on 20/01/23.
//
#include "fastbo/Parameter.h"

Parameter::Parameter() {
    /**
     * default settings
     */
    algorithm = ALGJT;
    num_threads = 1;

    num_samples = 10000;
    max_updating = 10; // default setting: stop after 10 times updates
    updating_interval = 2500; // default setting follows AIS-BN
    propagation_length = 2;
    enable_heuristic_uniform_distribution = false;
    enable_heuristic_theta_cutoff = false;

    group_size = 1;

    net_file = "alarm/alarm.xml"; // for inference
    ref_net_file = "alarm/alarm.bif"; // for structure learning
    train_set_file = "alarm/alarm_s5000.txt"; // for structure learning
    test_set_file = "alarm/testing_alarm_1k_p20"; // for inference
    pt_file = "alarm/alarm_1k_pt"; // for inference
}

void Parameter::ParseParameters(int argc, char *argv[]) {
    int i;
    for(i = 1; (i < argc) && ((argv[i])[0] == '-'); i++){
        switch((argv[i][1]))
        {
            case 'h': PrintHelpInfo(); exit(0);
            case 'a': i++; algorithm = atoi(argv[i]); break;
            case 't': i++; num_threads = atoi(argv[i]); break;
            case 'q': i++; num_samples = atoi(argv[i]); break;
            case 'm': i++; max_updating = atoi(argv[i]); break;
            case 'l': i++; updating_interval = atoi(argv[i]); break;
            case 'd': i++; propagation_length = atoi(argv[i]); break;
            case 'g': i++; group_size = atoi(argv[i]); break;
            case 'f':
                switch (argv[i][2]) {
                    case '0': i++; net_file = argv[i]; break;
                    case '1': i++; ref_net_file = argv[i]; break;
                    case '2': i++; train_set_file = argv[i]; break;
                    case '3': i++; test_set_file = argv[i]; break;
                    case '4': i++; pt_file = argv[i]; break;
                }; break;

            default: printf("\n Unrecognized option %s!\n", argv[i]);
                exit(0);
        }
    }

    if(algorithm < 0 || algorithm > 11){
        cout << "\tError! Please give the right value of -a to specify the functionality/algorithm" << endl;
        PrintFuncInfo();
        exit(0);
    }

    string file = "../dataset/";
    net_file = file + net_file;
    ref_net_file = file + ref_net_file;
    train_set_file = file + train_set_file;
    test_set_file = file + test_set_file;
    pt_file = file + pt_file;
}

void Parameter::PrintHelpInfo() {
    cout << endl << "Usage: ./BayesianNetwork [command-line options]" << endl << endl;

    cout << "Note: " << endl;
    cout << "Put all your input files into dataset/ and use command-line options to provide the relative path" << endl;
    cout << endl;

    cout << "Command-line options: " << endl;
    cout << "-h\tPrint this help message" << endl;
    cout << "-a\tSpecify the functionality/algorithm" << endl;
    PrintFuncInfo();
    cout << "-t\tSpecify number of threads, default 1" << endl;
    cout << "-q\tSpecify desired number of samples, default 10,000 [sampling-based approximate inference]" << endl;
    cout << "-m\tSpecify maximum updating times of importance function, default 10 [learning & importance sampling-based approximate inference]" << endl;
    cout << "-l\tSpecify updating interval, default 2,500 [learning & importance sampling-based approximate inference]" << endl;
    cout << "-d\tSpecify propagation length, default 2 [LBP and EPIS-BN]" << endl;
    cout << "-g\tSpecify group size, default 1 [PC-stable]" << endl;
    cout << "-f0\tProvide relative path of BN file, default alarm/alarm.xml [inference]" << endl;
    cout << "-f1\tProvide relative path of reference BN file, default alarm/alarm.bif [structure learning]" << endl;
    cout << "-f2\tProvide relative path of training set file, default alarm/alarm_s5000.txt [structure learning]" << endl;
    cout << "-f3\tProvide relative path of testing set file, default alarm/testing_alarm_1k_p20 [inference]" << endl;
    cout << "-f4\tProvide relative path of reference potential table file, default alarm/alarm_1k_pt [inference]" << endl;
}

void Parameter::PrintFuncInfo() {
    cout << "\t\t0:  PC-stable for structure learning" << endl;
    cout << "\t\t1:  Brute force for exact inference on full evidence" << endl;
    cout << "\t\t2:  Junction tree (JT) for exact inference" << endl;
    cout << "\t\t3:  Variable elimination (VE) for exact inference" << endl;
    cout << "\t\t4:  Probabilistic logic sampling (PLS) for approximate inference" << endl;
    cout << "\t\t5:  Likelihood weighting (LW) for approximate inference" << endl;
    cout << "\t\t6:  EPIS-BN for approximate inference" << endl;
    cout << "\t\t7:  Loopy belief propagation (LBP) for approximate inference" << endl;
    cout << "\t\t8:  Self-importance sampling (SIS) for approximate inference" << endl;
    cout << "\t\t9:  Self-importance sampling variant (SISv1) for approximate inference" << endl;
    cout << "\t\t10: AIS-BN for approximate inference" << endl;
    cout << "\t\t11: Generate set of samples according to input BN" << endl;
}