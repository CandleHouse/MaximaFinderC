import numpy as np
import ctypes
import os


if __name__ == '__main__':
    # python output
    phantom = np.fromfile('./phantom.raw', dtype=np.uint8).reshape(976, 976)
    print(np.max(phantom), np.min(phantom))

    # c++ output
    os.system('g++ -o cfindmaxima.so -shared -fPIC cfindmaxima.cpp')
    cpplib = ctypes.cdll.LoadLibrary('./cfindmaxima.so')
    rows, cols = phantom.shape

    dataptr = phantom.astype(np.uint32).ctypes.data_as(ctypes.c_char_p)
    # cpplib.getMax.restype = ctypes.c_char_p
    a = cpplib.getMax(dataptr, rows, cols, b"phantom2.raw")
    # print(ctypes.byref(a))
