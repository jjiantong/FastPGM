# class Dataset

## void LoadLIBSVMDataAutoDetectConfig(string data_file_path)
First, read the data file and store with the representation of `std::vector`.
Second, convert the `vector` representation into array (does not erase the `vector` representation).

Input: path to the LIBSVM data file (like a1a)

Output: none (but the information about data has been written to the data structure of the class instance)


## void ConvertLIBSVMVectorDatasetIntoArrayDataset()
First, initialize the array to be all zero. Second, change the value
to be 1 if it is 1 in the `vector` representation.

## void LoadCSVDataAutoDetectConfig(string data_file_path)
As the name suggests.

## void SamplesToLIBSVMFile(vector<DiscreteConfig> &samples, string file)
Store the drawn samples as LIBSVM file (like a1a).

Input: a `vector` of drawn samples

Output: a LIBSVM format data file

## SamplesToCSVFile
Store the drawn samples as CSV file. The first line of the file is like the table head.