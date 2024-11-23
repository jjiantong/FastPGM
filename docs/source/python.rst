Prerequisites
============

Before building the project, ensure the following prerequisites are met:

* Follow the `prerequisites <https://fastpgm.readthedocs.io/en/latest/installation.html#prerequisites>`__ of the FastPGM installation guide to set up required tools like CMake and compilers.
* ``Python`` (version 3.8 or later recommended).


Installation
============

Clone the FastPGM repository and install via ``pip```. Note the ``--recursive`` option which is needed for all the
submodules including ``pybind11``.

.. code-block:: bash

   git clone --recursive https://github.com/jjiantong/FastPGM.git
   cd FastPGM/python
   # under the directory of FastPGM/python
   pip install .


Running FastPGM
===============

Script ``pyfastpgm.py`` is provided in ``FastPGM/python``.

.. note::
   Before running the script, you must set the variable ``FASTPGM_DIR`` in as the absolute path of ``FastPGM``. This
   setting is in ``FastPGM/python/CMakeLists.txt`` (Line 8).

Run the script.

.. code-block:: bash

   # under the directory of FastPGM/python
   python3 pyfastpgm.py

Parameters
==========

The default FastPGM parameters are in ``FastPGM/python/config.json``. You can easily change it to specify the job, method
and corresponding parameters before running FastPGM. See FastPGM
`parameters <https://fastpgm.readthedocs.io/en/latest/param_toc.html>`__ for more information.