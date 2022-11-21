# MaximaFinderC: Maxima finding algorithm in ImageJ/Fiji re-implemented in C++

This is a re-implementation of the java plugin written by Michael Schmid and Wayne Rasband for ImageJ. The original java code source can be found in: https://imagej.nih.gov/ij/developer/source/ij/plugin/filter/MaximumFinder.java.html.

This script is simply a re-implementation of the ImageJ find maxima algorithm writing with pure C++ code. Therefore cython compiler is dropped for protential integration which written in C++.

The outcome is exactly consistent with the ImageJ/Fiji, and the calculation speed is in the same level of [
MaximaFinder](https://github.com/dwaithe/MaximaFinder), which use cython compiler to realize the same function.



### How To Call

The re-implementation of the maxima finding can either be called through c++ and python.

1. **C++**: Directly run `cfindmaxima.cpp`
2. **Python**: Use ctypes to call in `main.py`
3. **(TODO)Python**: Use pybind11 to import in `main.py`