import numpy as np


if __name__ == '__main__':
    phantom = np.fromfile('img/phantom.raw', dtype=np.uint8).reshape(976, 976)

    ### Python output
    # 0. 'findmaxima2d' re-implemented in cython: https://github.com/dwaithe/MaximaFinder.git
    from findmaxima2d import find_maxima, find_local_maxima
    local_max = find_local_maxima(img_data=phantom)
    y, x, regs = find_maxima(phantom, local_max, 10)

    ### C++ output
    # 1. Use dll to call in `main.py` and save as List
    import ctypes
    import os
    os.system('g++ -o cfindmaxima.so -shared -fPIC cfindmaxima.cpp')
    cpplib = ctypes.cdll.LoadLibrary('./cfindmaxima.so')
    rows, cols = phantom.shape

    dataptr = phantom.astype(np.uint32).ctypes.data_as(ctypes.c_char_p)
    cpplib.find_maxima(dataptr, rows, cols, ctypes.c_float(10), b"points.csv")

    # 2. Use pybind11 to import in `main.py`
    from pybind11_call.build import cfindmaxima
    x, y = cfindmaxima.find_maxima(phantom, 10)