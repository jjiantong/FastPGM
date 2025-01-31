import cmake_example as fastpgm
import os
import sys
import json

def main():
    # get the path of fastpgm
    current_dir = os.getcwd()
    parent_dir = os.path.dirname(current_dir)
    dpath = parent_dir + '/dataset/'

    # load and parse parameters
    with open('config.json', 'r') as f:
        config = json.load(f)

    job = config['job']
    method = config['method']
    num_threads = config['num_threads']
    verbose = config['verbose']
    num_samples = config['num_samples']
    max_updating = config['max_updating']
    updating_interval = config['updating_interval']
    propagation_length = config['propagation_length']
    enable_heuristic_uniform_distribution = config['enable_heuristic_uniform_distribution']
    enable_heuristic_theta_cutoff = config['enable_heuristic_theta_cutoff']
    group_size = config['group_size']
    alpha = config['alpha']
    save_struct = config['save_struct']
    save_param = config['save_param']
    net_file = config['net_file']
    ref_net_file = config['ref_net_file']
    train_set_file = config['train_set_file']
    test_set_file = config['test_set_file']
    pt_file = config['pt_file']


    if job == 0:
        """
        Job = structure learning
        Method = PC-Stable
        by default, we get a CPDAG and the graph may contain multiple independent sub-graphs. in order to get a DAG, or
        to get one connected graph, change the corresponding arguments in `StructLearnCompData`
        """
        if method != 0:
            print("\tError! We currently only support -a 0 for PC-Stable structure learning")
            sys.exit(1)

        print("==================================================")
        print("Job: PC-stable for structure learning, #threads = " + str(num_threads))
        print("\tgroup size = " + str(group_size))
        print("\treference BN: " + ref_net_file)
        print("\tsample set: " + train_set_file)
        print("==================================================")

        if ref_net_file:
            ref_net_file = dpath + ref_net_file

        fastpgm.BNSL_PCStable(verbose, num_threads, group_size, alpha,
                             ref_net_file, dpath + train_set_file, save_struct)

    elif job == 1:
        # print("Hello")
        """
        Job = learning (structure learning + parameter learning)
        Method = PC-Stable + maximum likelihood estimation
        by default, we get a CPDAG and the graph may contain multiple independent sub-graphs. in order to get a DAG, or
        to get one connected graph, change the corresponding arguments in `StructLearnCompData`
        """
        if method != 0:
            print("\tError! We currently only support -a 0 for PC-Stable structure learning + maximum likelihood"
                  "estimation parameter learning")
            sys.exit(1)

        print("==================================================")
        print("Job: PC-stable + maximum likelihood estimation for learning, #threads = " + str(num_threads))
        print("\tgroup size = " + str(group_size))
        print("\treference BN: " + ref_net_file)
        print("\tsample set: " + train_set_file)
        print("==================================================")

        if ref_net_file:
            ref_net_file = dpath + ref_net_file

        fastpgm.BNL_PCStable(verbose, num_threads, group_size, alpha,
                            ref_net_file, dpath + train_set_file, save_struct, save_param)

    elif job == 2:
        # print("Hello")
        """
        Job = exact inference
        """
        if method == 0:
            """
            Method = brute force
            brute force should only work on full evidence
            """
            print("==================================================")
            print("Job: brute force for exact inference, #threads = " + str(num_threads))
            print("\tBN: " + net_file)
            print("\ttesting set: " + test_set_file)
            print("\treference potential table: " + pt_file)
            print("==================================================")

            print("Brute force for exact inference is under development")
            sys.exit(1)

        elif method == 1:
            """
            Method = junction tree
            """
            print("==================================================")
            print("Job: junction tree for exact inference, #threads = " + str(num_threads))
            print("\tBN: " + net_file)
            print("\ttesting set: " + test_set_file)
            print("\treference potential table: " + pt_file)
            print("==================================================")

            if pt_file:
                pt_file = dpath + pt_file

            fastpgm.BNEI_JT(verbose, num_threads,
                           dpath + net_file, dpath + test_set_file, pt_file)

        elif method == 2:
            """
            Method = variable elimination
            """
            print("==================================================")
            print("Job: variable elimination for exact inference, #threads = " + str(num_threads))
            print("\tBN: " + net_file)
            print("\ttesting set: " + test_set_file)
            print("\treference potential table: " + pt_file)
            print("==================================================")

            print("Variable elimination for exact inference is under development")
            sys.exit(1)

        else:
            print("\tError! For exact inference, we currently support -m 0 brute force, -m 1 junction tree "
                  "and -m 2 variable elimination.")
            sys.exit(1)

    elif job == 3:
        # print("Hello")
        """
        Job =  approximate inference
        """
        if method == 0:
            """
            Method =  probabilistic logic sampling
            """
            print("==================================================")
            print("probabilistic logic sampling for approximate inference, #threads = " + str(num_threads))
            print("\t#samples: " + str(num_samples))
            print("\tBN: " + net_file)
            print("\ttesting set: " + test_set_file)
            print("\treference potential table: " + pt_file)
            print("==================================================")

            print("Probabilistic logic sampling for approximate inference is under development")
            sys.exit(1)

    elif job == 4:
        # print("Hello")
        """
        Job = classification
        """
        if method == 1:
            """
            Method = PC-Stable + maximum likelihood estimation + junction tree
            """
            print("==================================================")
            print("PC-stable + maximum likelihood estimation + junction tree for classification, #threads = "
                  + str(num_threads))
            print("\tgroup size = " + str(group_size))
            print("\treference BN: " + ref_net_file)
            print("\ttraining set: " + train_set_file)
            print("\ttesting set: " + test_set_file)
            print("==================================================")

            if ref_net_file:
                ref_net_file = dpath + ref_net_file

            fastpgm.C_PCStable_JT(verbose, num_threads, group_size, alpha,
                                 ref_net_file, dpath + train_set_file, dpath + test_set_file,
                                 save_struct, save_param)

    else:
        print("Hello")

if __name__ == "__main__":
    main()
