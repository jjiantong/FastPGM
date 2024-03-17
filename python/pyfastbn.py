import cmake_example as fastbn
import os

# get the path of fastbn
current_dir = os.getcwd()
parent_dir = os.path.dirname(current_dir)
dpath = parent_dir + '/dataset'
print(dpath)

test1 = fastbn.Test1(3,4)
out1 = test1.add1(1,0)
print(out1)
out2 = test1.add2(-5)
print(out2)
test2 = fastbn.Test2()
out3 = test2.sub1(7,4)
print(out3)
out4 = test2.sub2(0)
print(out4)


# trainer = fastbn.Dataset()
# trainer.load_csv_training_data()
#
# trainer->LoadCSVTrainingData(dpath + param.train_set_file, true, true, 0);