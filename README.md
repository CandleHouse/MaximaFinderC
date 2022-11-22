# MaximaFinderC: Maxima finding algorithm in ImageJ/Fiji re-implemented in C++

This is a re-implementation of the java plugin written by Michael Schmid and Wayne Rasband for ImageJ. The original java code source can be found in: https://imagej.nih.gov/ij/developer/source/ij/plugin/filter/MaximumFinder.java.html.

**This script is simply a re-implementation of the ImageJ find maxima algorithm writing with pure C++ code.** Therefore cython compiler is dropped for protential integration which written in C++.

The outcome is exactly consistent with the ImageJ/Fiji, and the calculation speed is in the same level of [
MaximaFinder](https://github.com/dwaithe/MaximaFinder), which use cython compiler to realize the same function.



## How To Call

The re-implementation of the maxima finding function can either be called through C++ and Python for any possible usage.

### For C++:

- Directly run `./cfindmaxima.cpp` under the root directory.

### For Python:

- Run `./main.py` under the root directory.

1. **Use dll to call**

   The interface exposed to *'ctypes.cdll.LoadLibrary'* is called *'find_maxima'*, therefore, use command:

   ```
   g++ -o cfindmaxima.so -shared -fPIC cfindmaxima.cpp
   ```

   under the root directory to compile `./cfindmaxima.cpp`, which is the same file for c++ calls.

   **⚠️ **This method is unable to back pass array of any type, as a result, maxima points founded can only be persisted in `./points.csv` files, which is in the same form of **ImageJ/Fiji output type: List**.



2. **(Recommend) Use [pybind11](https://github.com/pybind/pybind11.git) to call**

   pybind11 offer the possibility to pass *np.array* type to c++ function, and back pass the *np.array* type outcome for postprocess. 

   To compile it, use:

   ```
   cd ./pybind11_call/build/
   cmake ../../  # or use CMake to compile
   make
   ```

   the `cfindmaxima.cp38-win_amd64.pyd` will be generated under `./pybind11_call/build/`. **Then use the same python environment** in `./main.py` and call as:

   ```python
   from pybind11_call.build import cfindmaxima
   x, y = cfindmaxima.find_maxima(phantom, 10)
   ```

   x, y is in the *np.array* type.

   

The main function used in python is the same as *'analyzeAndMarkMaxima'* in `./cfindmaxima.cpp`, different interface lead to different calls.