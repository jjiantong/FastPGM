# FastBN: Fast Parallel Bayesian Network Learning and Inference

**Documentation** | **Parameters** | **Knowledge base** | **Python interface**

**FastBN** is an open-source C++ library that aims to help practitioners easily and efficiently 
apply Bayesian Network (BN) models to solve real-world problems. FastBN exploits multi-core CPUs 
to achieve high efficiency. Key features of FastBN are as follows:
- Wide coverage of different tasks and algorithms related to BNs, including structure learning 
(PC-stable), parameter learning (maximum likelihood estimation), exact inference (junction tree, 
variable elimination, brute force), and approximate inference (loopy belief propagation, 
probabilistic logic sampling, likelihood weighting, self-importance sampling, AIS-BN, EPIS-BN).
- Support classification, through the building blocks of structure learning, parameter learning 
and inference.
- Support Python interfaces.
- Support BN sample generation, dataset and network format convertor, etc.



## Installation

Just clone this repository and build. Note the ```--recursive``` option which is needed for all the 
submodules.
```
git clone --recursive https://github.com/jjiantong/FastBN.git
cd FastBN
# under the directory of FastBN
mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release -DUSE_MPI=OFF .. && make
```

[//]: # (On Mac OS, the default compiler &#40;clang&#41; needs to be changed to g++. &#40;You can use ```ls /usr/local/bin | grep g++``` to check the compiler. Mine is ```g++-9```.&#41;)

[//]: # (```)

[//]: # (mkdir build)

[//]: # (cd build)

[//]: # (cmake -DCMAKE_CXX_COMPILER=g++-9 -DCMAKE_BUILD_TYPE=Release -DUSE_MPI=OFF ..)

[//]: # (make)

[//]: # (```)

## Quick Start
```
# under the directory of FastBN/build
./BayesianNetwork
```
By default settings, FastBN will run the structure learning job using the PC-Stable method with one
thread (the sequential version) on 5000 samples generating from the ALARM BN. Command line options
can be used to specify the job, method and corresponding parameters, see **Parameters**.
 
[//]: # (Please use the following command line options when running the executable file:)

[//]: # ()
[//]: # (- ```-j & -m```: specify the job & method)

[//]: # (    ```)

[//]: # (    -j 0 -m 0: structure learning using PC-Stable; )

[//]: # (    -j 1 -m 0: learning using PC-Stable;)

[//]: # (    -j 2 -m 0: exact inference using brute force;)

[//]: # (    -j 2 -m 1: exact inference using junction tree &#40;JT&#41;;)

[//]: # (    -j 2 -m 2: exact inference using variable elimination &#40;VE&#41;;)

[//]: # (    -j 3 -m 0: approximate inference using loopy belief propagation &#40;LBP&#41;;)

[//]: # (    -j 3 -m 1: approximate inference using probabilistic logic sampling &#40;PLS&#41;;)

[//]: # (    -j 3 -m 2: approximate inference using likelihood weighting &#40;LW&#41;;)

[//]: # (    -j 3 -m 3: approximate inference using self-importance sampling &#40;SIS&#41;;)

[//]: # (    -j 3 -m 4: approximate inference using self-importance sampling variant &#40;SISv1&#41;;)

[//]: # (    -j 3 -m 5: approximate inference using AIS-BN;)

[//]: # (    -j 3 -m 6: approximate inference using EPIS-BN;)

[//]: # (    -j 4 -m 0: classification using PC-Stable + JT;)

[//]: # (    -j 4 -m 1: classification using PC-Stable + VE;)

[//]: # (    -j 4 -m 0: classification using PC-Stable + LBP;)

[//]: # (    -j 4 -m 1: classification using PC-Stable + PLS;)

[//]: # (    -j 4 -m 2: classification using PC-Stable + LW;)

[//]: # (    -j 4 -m 3: classification using PC-Stable + SIS;)

[//]: # (    -j 4 -m 4: classification using PC-Stable + SISv1;)

[//]: # (    -j 4 -m 5: classification using PC-Stable + AIS-BN;)

[//]: # (    -j 4 -m 6: classification using PC-Stable + EPIS-BN;)

[//]: # (    -j 5 -m 0: BN sample generator;)

[//]: # (    -j 5 -m 1: dataset format convertor &#40;from CSV to LibSVM&#41;;)

[//]: # (    -j 5 -m 2: dataset format convertor &#40;from LibSVM to CSV&#41;.)

[//]: # (    ```)

[//]: # (- ```-t```: specify number of threads, default 1)

[//]: # (- ```-v```: verbose, 0 for silence, 1 for key information and 2 for more information, default 1)

[//]: # (- ```-g```: group size, default 1 [used in PC-Stable])

[//]: # (- ```-a```: significance level alpha, default 0.05 [used in PC-Stable])

[//]: # (- ```-ss```: save the learned BN structure &#40;a graph&#41;, default 1 [used in structure learning])

[//]: # (- ```-sp```: save the learned BN parameter &#40;probabilities&#41;, default 1 [used in parameter learning])

[//]: # (- ```-q```: provide desired number of samples, default 10,000 [used in sampling-based approximate inference])

[//]: # (- ```-m```: provide maximum updating times of importance function, default 10 [used in learning & importance sampling-based approximate inference])

[//]: # (- ```-l```: provide updating interval, default 2,500 [used in learning & importance sampling-based approximate inference])

[//]: # (- ```-d```: provide propagation length, default 2 [used in LBP and EPIS-BN])

[//]: # (- ```-f0```: provide relative path of BN file, default ```alarm/alarm.xml``` [used in inference])

[//]: # (- ```-f1```: provide relative path of reference BN file, default ```alarm/alarm.bif``` [used in structure learning])

[//]: # (- ```-f2```: provide relative path of training set file &#40;in CSV format&#41;, default ```alarm/alarm_s5000``` [used in structure learning])

[//]: # (- ```-f3```: provide relative path of testing set file &#40;in LIBSVM format&#41;, default ```alarm/testing_alarm_1k_p20``` [used in inference])

[//]: # (- ```-f4```: provide relative path of reference potential table file, default ```alarm/alarm_1k_pt``` [used in inference])



## Citation

If you find FastBN useful, please consider citing our papers.

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
 * [pybind11](https://github.com/pybind/pybind11)
