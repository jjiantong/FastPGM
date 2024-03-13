//
// Created by jjt on 20/01/23.
//
#include "fastbn/Parameter.h"

Parameter::Parameter() {
    /**
     * default settings
     */
    job = 0; // default: structure learning
    method = 0; // default: the first method for each job
    num_threads = 1; // default: sequential running
    verbose = 1; // default: key information

    num_samples = 10000; // default: generating 10000 samples
    max_updating = 10; // default: stop after 10 times updates
    updating_interval = 2500; // default setting follows AIS-BN
    propagation_length = 2;
    enable_heuristic_uniform_distribution = false;
    enable_heuristic_theta_cutoff = false;

    group_size = 1;
    alpha = 0.05;

    save_struct = true;
    save_param = true;

    net_file = "alarm/alarm.xml"; // for inference
//    ref_net_file = "alarm/alarm.bif"; // for structure learning
    ref_net_file = ""; // for structure learning
    train_set_file = "alarm/alarm_s5000"; // for structure learning
    test_set_file = "alarm/testing_alarm_1k_p20_csv"; // for inference
    pt_file = "alarm/alarm_1k_pt"; // for inference
//    pt_file = ""; // for inference
}

void Parameter::ParseParameters(int argc, char *argv[]) {
    int i;
    for(i = 1; (i < argc) && ((argv[i])[0] == '-'); i++){
        switch((argv[i][1]))
        {
            case 'h': PrintHelpInfo(); exit(0);
            case 'j': i++; job = atoi(argv[i]); break;
            case 'm': i++; method = atoi(argv[i]); break;
            case 't': i++; num_threads = atoi(argv[i]); break;
//            case 'q': i++; num_samples = atoi(argv[i]); break;
//            case 'u': i++; max_updating = atoi(argv[i]); break;
//            case 'l': i++; updating_interval = atoi(argv[i]); break;
//            case 'd': i++; propagation_length = atoi(argv[i]); break;
            case 'g': i++; group_size = atoi(argv[i]); break;
            case 'a': i++; alpha = stod(argv[i]); break;
            case 's':
                switch (argv[i][2]) {
                    case 's': i++;
                        if (atoi(argv[i]) == 0) {save_struct = false;}
                        break;
                    case 'p': i++;
                        if (atoi(argv[i]) == 0) {save_param = false;}
                        break;
                }
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

    if(job < 0 || job > 5){
        cout << "\tError! Please give the right value of -j to specify the job" << endl;
        PrintJobInfo();
        exit(0);
    }
}

void Parameter::PrintHelpInfo() {
    cout << endl << "Usage: ./BayesianNetwork [command-line options]" << endl << endl;

    cout << "Note: " << endl;
    cout << "Put all your input files into dataset/ and use command-line options to provide the relative path" << endl;
    cout << endl;

    cout << "Command-line options: " << endl;
    cout << "-h\tPrint this help message" << endl;
    cout << "-t\tSpecify number of threads, default 1" << endl;
    cout << "-v\tSpecify verbose, 0 for silence, 1 for key information and 2 for more information, default 1" << endl;
    cout << "-j & -m\tSpecify the job & method" << endl;
    PrintJobAndAlgInfo();
    cout << "-q\tSpecify desired number of samples, default 10,000 [sampling-based approximate inference]" << endl;
    cout << "-u\tSpecify maximum updating times of importance function, default 10 [learning & importance sampling-based approximate inference]" << endl;
    cout << "-l\tSpecify updating interval, default 2,500 [learning & importance sampling-based approximate inference]" << endl;
    cout << "-d\tSpecify propagation length, default 2 [LBP and EPIS-BN]" << endl;
    cout << "-g\tSpecify group size, default 1 [PC-stable]" << endl;
    cout << "-a\tSpecify significance level alpha, default 0.05 [PC-stable]" << endl;
    cout << "-ss\tWhether to output the learned structure, 0 for no, 1 for yes, default 1 [for structure learning]" << endl;
    cout << "-sp\tWhether to output the learned parameter, 0 for no, 1 for yes, default 1 [for parameter learning]" << endl;
    cout << "-f0\tProvide relative path of BN file, default alarm/alarm.xml [inference]" << endl;
    cout << "-f1\tProvide relative path of reference BN file, default alarm/alarm.bif [structure learning]" << endl;
    cout << "-f2\tProvide relative path of training set file, default alarm/alarm_s5000.txt [structure learning]" << endl;
    cout << "-f3\tProvide relative path of testing set file, default alarm/testing_alarm_1k_p20 [inference]" << endl;
    cout << "-f4\tProvide relative path of reference potential table file, default alarm/alarm_1k_pt [inference]" << endl;
}

void Parameter::PrintJobAndAlgInfo() {
    cout << "\t-j 0: Structure learning" << endl;
    cout << "\t\t-m 0: PC-Stable" << endl;

    cout << "\t-j 1: Learning (structure and parameter)" << endl;
    cout << "\t\t-m 0: PC-Stable + maximum likelihood estimation (MLE)" << endl;

    cout << "\t-j 2: Exact inference" << endl;
    cout << "\t\t-m 0: Brute force (on full evidence)" << endl;
    cout << "\t\t-m 1: Junction tree (JT)" << endl;
    cout << "\t\t-m 2: Variable elimination (VE)" << endl;

    cout << "\t-j 3: Approximate inference" << endl;
    cout << "\t\t-m 0: Probabilistic logic sampling (PLS)" << endl;
    cout << "\t\t-m 1: Likelihood weighting (LW)" << endl;
    cout << "\t\t-m 2: EPIS-BN" << endl;
    cout << "\t\t-m 3: Loopy belief propagation (LBP)" << endl;
    cout << "\t\t-m 4: Self-importance sampling (SIS)" << endl;
    cout << "\t\t-m 5: Self-importance sampling variant (SISv1)" << endl;
    cout << "\t\t-m 6: AIS-BN" << endl;

    cout << "\t-j 4: Classification" << endl;
    cout << "\t\t-m 0: PC-Stable + MLE + Brute force" << endl;
    cout << "\t\t-m 1: PC-Stable + MLE + JT" << endl;
    cout << "\t\t-m 2: PC-Stable + MLE + VE" << endl;
    cout << "\t\t-m 3: PC-Stable + MLE + PLS" << endl;
    cout << "\t\t-m 4: PC-Stable + MLE + LW" << endl;
    cout << "\t\t-m 5: PC-Stable + MLE + EPIS-BN" << endl;
    cout << "\t\t-m 6: PC-Stable + MLE + LBP" << endl;
    cout << "\t\t-m 7: PC-Stable + MLE + SIS" << endl;
    cout << "\t\t-m 8: PC-Stable + MLE + SISv1" << endl;
    cout << "\t\t-m 9: PC-Stable + AIS-BN" << endl;

    cout << "\t-j 5: Generator or convertor" << endl;
    cout << "\t\t-m 0: Generate set of samples according to input BN" << endl;
}

void Parameter::PrintJobInfo() {
    cout << "\t-j 0: Structure learning" << endl;
    cout << "\t-j 1: Learning (structure and parameter)" << endl;
    cout << "\t-j 2: Exact inference" << endl;
    cout << "\t-j 3: Approximate inference" << endl;
    cout << "\t-j 4: Classification" << endl;
    cout << "\t-j 5: Generator or convertor" << endl;
}

int add(int i, int j) {
    return i + j;
}