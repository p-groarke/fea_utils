# string_algorithm

String functions that should be built in c++.

## Examples

```c++

```

## Build
`string_algorithm` is a single header with no dependencies other than the stl.

The unit tests depend on gtest. They are not built by default. Use conan to install the dependencies when running the test suite.

### Windows
```
mkdir build && cd build
..\conan.bat
cmake .. -A x64 -DBUILD_TESTING=On && cmake --build . --config debug
bin\string_algorithm_tests.exe

// Optionally
cmake --build . --target install
```

### Unixes
```
mkdir build && cd build
..\conan.sh
cmake .. -DBUILD_TESTING=On && cmake --build . --config debug
bin\string_algorithm_tests.exe

// Optionally
cmake --build . --target install
```
