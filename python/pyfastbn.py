import cmake_example as fastbn
import os
import sys
import json

def main():
    # get the path of fastbn
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

        fastbn.BNSL_PCStable(verbose, num_threads, group_size, alpha,
                             ref_net_file, dpath + train_set_file, save_param)

    elif job == 1:
        print("Hello job 1")
    elif job == 2:
        print("Hello job 2")
    elif job == 3:
        print("Hello job 3")
    elif job == 4:
        print("Hello job 4")
    else:
        print("Hello")

if __name__ == "__main__":
    main()
