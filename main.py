import numpy as np
import ctypes
import os
from findmaxima2d import find_maxima, find_local_maxima
import time


if __name__ == '__main__':
    # python output
    phantom = np.fromfile('./phantom.raw', dtype=np.uint8).reshape(976, 976)
    local_max = find_local_maxima(img_data=phantom)
    v, u, regs = find_maxima(phantom, local_max, 10)

    # c++ output
    os.system('g++ -o cfindmaxima.so -shared -fPIC cfindmaxima.cpp')
    cpplib = ctypes.cdll.LoadLibrary('./cfindmaxima.so')
    rows, cols = phantom.shape

    dataptr = phantom.astype(np.uint32).ctypes.data_as(ctypes.c_char_p)
    cpplib.find_maxima(dataptr, rows, cols, ctypes.c_float(10), b"points.csv")