# FastBN


# How to use

Clone the repo.
```
git clone https://github.com/jjiantong/FastBN.git
cd Bayesian-network/lib
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

Run the test.
```
cd test
./test_exe
```




# Dependency
 * [GoogleTest](https://github.com/google/googletest)
 * [TinyXML2](https://github.com/leethomason/tinyxml2)
 * [ARFF](https://github.com/LinjianLi/ARFF)
 * [StasLib](https://github.com/jjiantong/stats)
 * [GCE-Math](https://github.com/kthohr/gcem)