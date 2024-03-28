Global Parameters
=================

* ``-t``: number of threads (default 1)
* ``-v``: verbose, 0 for silence, 1 for key information and 2 for more information (default 1)
* ``-j``: job, see the table below (default 0 for structure learning)
* ``-m``: method, see the table below (default 0 for PC-Stable)

+-----------+-----------+---------------------------------------------------------------+
| ``-j``    | ``-m``    | Job & Method                                                  |
+===========+===========+===============================================================+
| ``-j 0``  | ``-m 0``  | structure learning using PC-Stable                            |
+-----------+-----------+---------------------------------------------------------------+
| ``-j 1``  | ``-m 0``  | learning using PC-Stable & MLE                                |
+-----------+-----------+---------------------------------------------------------------+
| ``-j 2``  | ``-m 0``  | exact inference using brute force                             |
+           +-----------+---------------------------------------------------------------+
|           | ``-m 1``  | exact inference using JT                                      |
+           +-----------+---------------------------------------------------------------+
|           | ``-m 2``  | exact inference using VE                                      |
+-----------+-----------+---------------------------------------------------------------+
| ``-j 3``  | ``-m 0``  | approximate inference using LBP                               |
+           +-----------+---------------------------------------------------------------+
|           | ``-m 1``  | approximate inference using PLS                               |
+           +-----------+---------------------------------------------------------------+
|           | ``-m 2``  | approximate inference using LW                                |
+           +-----------+---------------------------------------------------------------+
|           | ``-m 3``  | approximate inference using SIS                               |
+           +-----------+---------------------------------------------------------------+
|           | ``-m 4``  | approximate inference using SIS-v1                            |
+           +-----------+---------------------------------------------------------------+
|           | ``-m 5``  | approximate inference using AIS-BN                            |
+           +-----------+---------------------------------------------------------------+
|           | ``-m 6``  | approximate inference using EPIS-BN                           |
+-----------+-----------+---------------------------------------------------------------+
| ``-j 4``  | ``-m 0``  | classification using PC-Stable & MLE & brute force            |
+           +-----------+---------------------------------------------------------------+
|           | ``-m 1``  | classification using PC-Stable & MLE & JT                     |
+           +-----------+---------------------------------------------------------------+
|           | ``-m 2``  | classification using PC-Stable & MLE & VE                     |
+           +-----------+---------------------------------------------------------------+
|           | ``-m 0``  | classification using PC-Stable & MLE & LBP                    |
+           +-----------+---------------------------------------------------------------+
|           | ``-m 1``  | classification using PC-Stable & MLE & PLS                    |
+           +-----------+---------------------------------------------------------------+
|           | ``-m 2``  | classification using PC-Stable & MLE & LW                     |
+           +-----------+---------------------------------------------------------------+
|           | ``-m 3``  | classification using PC-Stable & MLE & SIS                    |
+           +-----------+---------------------------------------------------------------+
|           | ``-m 4``  | classification using PC-Stable & MLE & SISv1                  |
+           +-----------+---------------------------------------------------------------+
|           | ``-m 5``  | classification using PC-Stable & MLE & AIS-BN                 |
+           +-----------+---------------------------------------------------------------+
|           | ``-m 6``  | classification using PC-Stable & MLE & EPIS-BN                |
+-----------+-----------+---------------------------------------------------------------+
| ``-j 5``  | ``-m 0``  | BN sample generator                                           |
+           +-----------+---------------------------------------------------------------+
|           | ``-m 1``  | dataset format convertor from CSV to LibSVM                   |
+           +-----------+---------------------------------------------------------------+
|           | ``-m 2``  | dataset format convertor from LibSVM to CSV                   |
+-----------+-----------+---------------------------------------------------------------+

Note:

MLE = maximum likelihood estimation;
JT = juntion tree;
VE = variable elimination;
LBP = loopy belief propagation;
PLS = probabilistic logic sampling;
LW = likelihood weighting;
SIS = self-importance sampling;


Parameters Related to Structure Learning
========================================

For the tasks that include structure learning, the following parameters can be specified through command line options.

* ``-g``: group size, used in the implementation of PC-Stable (default 1)
* ``-a``: significance level alpha, used in constraint-based methods like PC-Stable (default 0.05)
* ``-ss``: whether to save the learned BN structure, 0 for no, 1 for yes (default 1)


Parameters Related to Parameter Learning
========================================

For the tasks that include structure learning, you can use ``-sp`` to specify whether to save the learned BN parameters
or not (default 1).


Parameters Related to Approximate Inference
===========================================

For the tasks that include structure learning, the following parameters can be specified through command line options.

* ``-q``: desired number of samples, used in sampling-based methods including PLS, LW, SIS, SISv1, AIS-BN, EPIS-BN (default 10,000)
* ``-u``: maximum updating times of importance function, used in learning based importance sampling methods including SIS, SISv1, AIS-BN (default 10)
* ``-l``: updating interval, used in learning based importance sampling methods (default 2,500)
* ``-d``: propagation length, used in LBP and EPIS-BN (default 2)


Parameters for Files
====================

The files of datasets and networks can be provided via command line options. Please put all the files in
``FastBN/dataset`` and provide the relative path.

* ``-f0``: the BN that inference operates on (default ``alarm/alarm.xml``)
* ``-f1``: the reference BN that serves as the ground truth of structure learning (default ``alarm/alarm.bif``)
* ``-f2``: training set, used in structure learning, learning, or classification (default ``alarm/alarm_s5000``)
* ``-f3``: testing set, used in inference or classification (default ``alarm/testing_alarm_1k_p20``)
* ``-f4``: the reference potential table file that serves as the ground truth of inference (default ``alarm/alarm_1k_pt``)





