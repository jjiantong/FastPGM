Installation
============

Prerequisites
-------------

Before building the project, ensure the following prerequisites are met:

* ``CMake`` (version 3.22 or later recommended).
* ``gcc``/``g++`` on Linux and MacOS (Intel).
* ``clang``/``clang++`` on MacOS (Apple Silicon); recommended to install the latest version via ``LLVM``.

Please see detailed installation steps below.


Download
--------

Clone the FastPGM repository. Note the ``--recursive`` option which is needed for all the submodules.

.. code-block:: bash

   git clone --recursive https://github.com/jjiantong/FastPGM.git

This will create a local copy of the project on your machine. Navigate to the project directory:

.. code-block:: bash

   cd FastPGM


Build
-----

Once you have downloaded the source code, you can build FastPGM.

For Linux Users:
^^^^^^^^^^^^^^^^

.. code-block:: bash

   # under the directory of FastPGM
   mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && make

After the build process completes, the executable ``./BayesianNetwork`` will be available in the ``build`` directory.


For MacOS (Apple Silicon) Users:
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

We recommend to manually install ``clang``/``clang++`` via LLVM to get the latest version and additional tools. Besides,
Clang has built-in support for OpenMP directives, but Clang itself does not provide the runtime support required for
OpenMP to function. The runtime support is provided by ``libomp``, which you need to manually install.

Install ``llvm`` and ``libomp`` through `Homebrew <https://brew.sh/>`__:

.. code-block:: bash

   brew install llvm libomp

After installing the LLVM toolchain and the ``libomp`` library, you'll need to configure your environment. To do this,
add the following lines to your ``~/.zshrc`` file:

.. code-block:: bash

   export CC=/opt/homebrew/opt/llvm/bin/clang
   export CXX=/opt/homebrew/opt/llvm/bin/clang++
   export PATH="/opt/homebrew/opt/llvm/bin:$PATH"

After the above steps, you may proceed to build FastPGM as described above in **For Linux Users**.


For MacOS (Intel) Users:
^^^^^^^^^^^^^^^^^^^^^^^^

Install ``gcc``/``g++`` and check the version:

.. code-block:: bash

   brew install gcc

Check if ``g++`` is installed and find the version:

.. code-block:: bash

   ls /usr/local/bin | grep g++

If you see an output like ``g++-14``, note the version and use it in your build command, e.g.,
``-DCMAKE_CXX_COMPILER=g++-14``, to change the default compiler on MacOS. More specifically:

.. code-block:: bash

   # under the directory of FastPGM
   mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-14 .. && make

.. note::
   We suppose that following the same steps as for MacOS (Apple Silicon) should work on MacOS (Intel), but this method
   hasn't been tested by us.

