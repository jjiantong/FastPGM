Installation
============

Clone the FastBN repository and install via ``pip```. Note the ``--recursive`` option which is needed for all the
submodules including ``pybind11``.

.. code-block:: bash

   git clone --recursive https://github.com/jjiantong/FastBN.git
   cd FastBN/python
   # under the directory of FastBN/python
   pip install .

Running FastBN
==============

Script ``pyfastbn.py`` is provided in ``FastBN/python``.

.. note::
   Before running the script, you must set the variable ``FASTBN_DIR`` in as the absolute path of ``FastBN``. This
   setting is in ``FastBN/python/CMakeLists.txt`` (Line 8).

Run the script.

.. code-block:: bash

   # under the directory of FastBN/python
   python3 pyfastbn.py

Parameters
==========

The default FastBN parameters are in ``FastBN/python/config.json``. You can easily change it to specify the job, method
and corresponding parameters before running FastBN. See FastBN Parameters for more information.