Installation
============

Prerequisites
-------------

Before building the project, ensure the following prerequisites are met:

* **Linux**:

  * ``gcc``/``g++``.

* **MacOS (Apple Silicon)**:

  * ``clang``/``clang++``; recommended to install the latest version via ``LLVM``, see detailed instructions below.
  * ``libomp``; see detailed installation steps below.

* **MacOS (Intel)**:

  * Either use ``clang``/``clang++`` & ``libomp`` as described above, or
  * Use ``gcc``/``g++``; see detailed instructions below.

* Ensure ``CMake`` (version 3.27 or later recommended) is installed on all systems.



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

Once you have downloaded the source code, you can build FastPGM. The recommended option for most users:

.. code-block:: bash

   # under the directory of FastPGM
   mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && make

After the build process completes, the executable ``./BayesianNetwork`` will be available in the ``build`` directory.


For MacOS users:
^^^^^^^^^^^^^^^^

We recommend to manually install ``clang``/``clang++`` via LLVM to get the latest version and additional tools. Besides,
Clang has built-in support for OpenMP directives, but Clang itself does not provide the runtime support required for
OpenMP to function. The runtime support is provided by ``libomp``, which you need to manually install. Install through
`Homebrew <https://brew.sh/>`__:

.. code-block:: bash

   brew install llvm libomp

After installing the LLVM toolchain and the ``libomp`` library, you'll need to configure your environment. To do this,
add the following lines to your ``~/.zshrc`` file:

.. code-block:: bash

   export CC=/usr/local/opt/llvm/bin/clang
   export CXX=/usr/local/opt/llvm/bin/clang++
   export PATH="/usr/local/opt/llvm/bin:$PATH"

After the above steps, you may build FastPGM and obtain the executable.


