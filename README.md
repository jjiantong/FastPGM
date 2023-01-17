# FastBN

## Overview

The aim of FastBN is to help users easily and efficiently apply Bayesian Network (BN) models to solve real-world problems. 
FastBN exploits multi-core CPUs to achieve high efficiency. 
FastBN contains:
- Fast-BNS: accelerated PC-stable algorithm for BN structure learning from data 
  (to learn DAGs that are well matched the observed data);
- Fast-BNI: accelerated junction tree algorithm for exact inference on BNs
  (given observed values of some variables in the BN, to calculate the conditional
  probability of the other variables)

<div align="center">
<img src="https://github.com/jjiantong/FastBN/raw/master/docs/figs/bnsl_tab.png" width="800" />
<img src="https://github.com/jjiantong/FastBN/raw/master/docs/figs/bnei_tab.png" width="800" />
</div>

<!-- ![bnsl_tab](https://github.com/jjiantong/FastBN/blob/master/docs/figs/bnei_tab.pdf)
![bnei_tab](https://github.com/jjiantong/FastBN/blob/master/docs/figs/bnei_tab.pdf) -->

## How to use

Clone the repo.
```
git clone https://github.com/jjiantong/FastBN.git
cd FastBN/lib
#under the directory of lib
git submodule init ARFF && git submodule init googletest && git submodule init tinyxml2 && git submodule init stats && git submodule init gcem && git submodule update
```

Build the project. 

Create a directory ```build``` under the root directory. Generate the make file and compile.
```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

On Mac OS, the default compiler (clang) needs to be changed to g++. (You can use ```ls /usr/local/bin | grep g++``` to check the compiler. Mine is ```g++-9```.)
```
mkdir build
cd build
cmake -DCMAKE_CXX_COMPILER=g++-9 -DCMAKE_BUILD_TYPE=Release ..
make
```

Quick start.
```
./BayesianNetwork
```
By default settings, FastBN will run exact inference task using the sequential version of junction tree algorithm 
on 1000 test cases of the ALARM network. Each test case contains 20% observed variables.
Please add the following commands when running the executable file:

- ```-a```: specify the algorithm
    ```
    0: PC-stable for structure learning; 
    1: brute force for exact inference on full evidence; 
    2: junction tree for exact inference; 
    3: variable elimination for exact inference.
    ```
- ```-t```: specify number of threads
- ```-g```: group size (used only in PC-stable)
- ```-f0```: network file path (in XMLBIF format, used only in inference algorithms)
- ```-f1```: testing set file path (in LIBSVN format, used only in inference algorithms)
- ```-f3```: training set file path (in CSV format, used only in structure learning algorithms)
- ```-f4```: reference network file path (used only in structure learning algorithms to compare)



## How to cite our papers

If you find FastBN useful, please cite our papers.

Fast-BNS in IPDPS 2022 ([pdf](https://github.com/jjiantong/FastBN/tree/master/docs/papers/bnsl_ipdps.pdf)):
```
@inproceedings{jiang2022fast,
  title={Fast Parallel Bayesian Network Structure Learning},
  author={Jiang, Jiantong and Wen, Zeyi and Mian, Ajmal},
  booktitle={2022 IEEE International Parallel and Distributed Processing Symposium (IPDPS)},
  pages={617--627},
  year={2022}
}
```

Fast-BNI (to appear) in PPoPP 2023 ([pdf](https://github.com/jjiantong/FastBN/tree/master/docs/papers/bnei_ppopp.pdf)):
```
@article{jiang2022fast2,
  title={Fast Parallel Exact Inference on Bayesian Networks: Poster},
  author={Jiang, Jiantong and Wen, Zeyi and Mansoor, Atif and Mian, Ajmal},
  journal={arXiv preprint arXiv:2212.04241},
  year={2022}
}
```


## Dependency
 * [GoogleTest](https://github.com/google/googletest)
 * [TinyXML2](https://github.com/leethomason/tinyxml2)
 * [ARFF](https://github.com/LinjianLi/ARFF)
 * [StasLib](https://github.com/jjiantong/stats)
 * [GCE-Math](https://github.com/kthohr/gcem)
