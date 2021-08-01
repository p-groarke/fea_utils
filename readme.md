<br/>

# This repository is inactive and kept for posterity's sake.
# Any further development will happen in [fea_libs](https://github.com/p-groarke/fea_libs). Cheers.
<br/>
<br/>

# fea_utils
[![Build status](https://ci.appveyor.com/api/projects/status/08nrt1tbdau7o3jq/branch/master?svg=true)](https://ci.appveyor.com/project/p-groarke/fea-utils/branch/master)
[![Build Status](https://travis-ci.org/p-groarke/fea_utils.svg?branch=master)](https://travis-ci.org/p-groarke/fea_utils)

Useful utility functions to do basic things in c++.

Includes helpers for file reading and writing (per line, binary, string), string functions (split, starts_with, ends_with, contains, utf conversions, etc), reader/writer mutex wrappers, very basic parallel_for and parallel_task and more.

## Examples

```c++

```

## Build
`fea_utils` is a single header with no dependencies other than the stl.

The unit tests depend on gtest. They are not built by default. Use conan to install the dependencies when running the test suite.

### Windows
```
mkdir build && cd build
..\conan.bat
cmake .. -A x64 -DBUILD_TESTING=On && cmake --build . --config debug
bin\fea_utils_tests.exe

// Optionally
cmake --build . --target install
```

### Unixes
```
mkdir build && cd build
..\conan.sh
cmake .. -DBUILD_TESTING=On && cmake --build . --config debug
bin\fea_utils_tests.exe

// Optionally
cmake --build . --target install
```
