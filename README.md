# FastBN

Fast and parallel solutions for Bayesian Network models (IPDPS'22, PPoPP'23, TPDS'24)

## Overview

The aim of FastBN is to help practitioners easily and efficiently apply Bayesian Network (BN) models to solve real-world problems. 
FastBN exploits multi-core CPUs to achieve high efficiency. Key features of FastBN are as follows:
- Support structure learning (i.e., Fast-BNS, Fast-BNS-v2): to learn the BN structure (e.g., a DAG) that are well matched the observed data.
  - PC-stable.
- Support learning: to learn the BN structure (e.g., a DAG) and parameters (i.e., probabilities) from the observed data.
- Support exact inference (i.e., Fast-BNI): given observed values of some variables in the BN, to calculate the exact conditional probabilities of the other variables. 
  - Junction tree.
  - Variable elimination.
- Support approximate inference (i.e, Fast-PGM, coming soon): given observed values of some variables in the BN, to calculate the approximate conditional probabilities of the other variables. 
  - Loopy belief propagation.
  - Probabilistic logic sampling.
  - Likelihood weighting.
  - Self-importance sampling (and variants).
  - AIS-BN.
  - EPIS-BN.
- Support classification, through the building blocks of structure learning, parameter learning and inference.
- Support other related functionalities, including BN sample generator, dataset format convertor, etc.

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

Build the project: Create a directory ```build``` under the root directory. Generate the make file and compile.
```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DUSE_MPI=OFF ..
make
```

On Mac OS, the default compiler (clang) needs to be changed to g++. (You can use ```ls /usr/local/bin | grep g++``` to check the compiler. Mine is ```g++-9```.)
```
mkdir build
cd build
cmake -DCMAKE_CXX_COMPILER=g++-9 -DCMAKE_BUILD_TYPE=Release -DUSE_MPI=OFF ..
make
```

Quick start.
```
./BayesianNetwork
```
By default settings, FastBN will run structure learning job using the sequential version of PC-Stable algorithm 
on 5000 samples generating from the ALARM BN. 
Please use the following command line options when running the executable file:

- ```-j & -m```: specify the job & method
    ```
    -j 0 -m 0: structure learning using PC-Stable; 
    -j 1 -m 0: learning using PC-Stable;
    -j 2 -m 0: exact inference using junction tree (JT);
    -j 2 -m 1: exact inference using variable elimination (VE);
    -j 3 -m 0: approximate inference using loopy belief propagation (LBP);
    -j 3 -m 1: approximate inference using probabilistic logic sampling (PLS);
    -j 3 -m 2: approximate inference using likelihood weighting (LW);
    -j 3 -m 3: approximate inference using self-importance sampling (SIS);
    -j 3 -m 4: approximate inference using self-importance sampling variant (SISv1);
    -j 3 -m 5: approximate inference using AIS-BN;
    -j 3 -m 6: approximate inference using EPIS-BN;
    -j 4 -m 0: classification using PC-Stable + JT;
    -j 4 -m 1: classification using PC-Stable + VE;
    -j 4 -m 0: classification using PC-Stable + LBP;
    -j 4 -m 1: classification using PC-Stable + PLS;
    -j 4 -m 2: classification using PC-Stable + LW;
    -j 4 -m 3: classification using PC-Stable + SIS;
    -j 4 -m 4: classification using PC-Stable + SISv1;
    -j 4 -m 5: classification using PC-Stable + AIS-BN;
    -j 4 -m 6: classification using PC-Stable + EPIS-BN;
    -j 5 -m 0: BN sample generator;
    -j 5 -m 1: dataset format convertor (from CSV to LibSVM);
    -j 5 -m 2: dataset format convertor (from LibSVM to CSV).
    ```
- ```-t```: specify number of threads, default 1
- ```-v```: verbose, 0 for silence, 1 for key information and 2 for more information, default 1
- ```-g```: group size, default 1 [used in PC-Stable]
- ```-a```: significance level alpha, default 0.05 [used in PC-Stable]
- ```-ss```: save the learned BN structure (a graph), default 1 [used in structure learning]
- ```-sp```: save the learned BN parameter (probabilities), default 1 [used in parameter learning]
- ```-q```: provide desired number of samples, default 10,000 [used in sampling-based approximate inference]
- ```-m```: provide maximum updating times of importance function, default 10 [used in learning & importance sampling-based approximate inference]
- ```-l```: provide updating interval, default 2,500 [used in learning & importance sampling-based approximate inference]
- ```-d```: provide propagation length, default 2 [used in LBP and EPIS-BN]
- ```-f0```: provide relative path of BN file, default ```alarm/alarm.xml``` [used in inference]
- ```-f1```: provide relative path of reference BN file, default ```alarm/alarm.bif``` [used in structure learning]
- ```-f2```: provide relative path of training set file (in CSV format), default ```alarm/alarm_s5000``` [used in structure learning]
- ```-f3```: provide relative path of testing set file (in LIBSVM format), default ```alarm/testing_alarm_1k_p20``` [used in inference]
- ```-f4```: provide relative path of reference potential table file, default ```alarm/alarm_1k_pt``` [used in inference]



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

Fast-BNS-v2 in TPDS (2024):
```
@article{yang2023parallel,
  title={Parallel and Distributed Bayesian Network Structure Learning},
  author={Yang, Jian and Jiang, Jiantong and Wen, Zeyi and Mian, Ajmal},
  journal={IEEE Transactions on Parallel and Distributed Systems},
  year={2024},
  publisher={IEEE}
}
```

Fast-BNI in PPoPP 2023 ([pdf](https://github.com/jjiantong/FastBN/tree/master/docs/papers/bnei_ppopp.pdf)):
```
@inproceedings{jiang2023fast,
  title={Fast Parallel Exact Inference on Bayesian Networks},
  author={Jiang, Jiantong and Wen, Zeyi and Mansoor, Atif and Mian, Ajmal},
  booktitle={Proceedings of the 28th ACM SIGPLAN Annual Symposium on Principles and Practice of Parallel Programming},
  pages={425--426},
  year={2023}
}
```


## Dependency
 * [GoogleTest](https://github.com/google/googletest)
 * [TinyXML2](https://github.com/leethomason/tinyxml2)
 * [ARFF](https://github.com/LinjianLi/ARFF)
 * [StasLib](https://github.com/jjiantong/stats)
 * [GCE-Math](https://github.com/kthohr/gcem)
