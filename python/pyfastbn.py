import scikit_build_example as fastbn
import os

# get the path of fastbn
current_dir = os.getcwd()
parent_dir = os.path.dirname(current_dir)
dpath = parent_dir + '/dataset'
print(dpath)

test_add = fastbn.add(1,2)
print(test_add)

# trainer = fastbn.Dataset()
# trainer.load_csv_training_data()
#
# trainer->LoadCSVTrainingData(dpath + param.train_set_file, true, true, 0);
