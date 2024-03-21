import cmake_example as fastbn
import os

# get the path of fastbn
current_dir = os.getcwd()
parent_dir = os.path.dirname(current_dir)
dpath = parent_dir + '/dataset/'

fastbn.BNSL_PCStable(1, 1, 1, 0.05, "", dpath + 'alarm/alarm_s5000', True)
