# Table of Contents
- [Table of Contents](#table-of-contents)
- [Neyson](#neyson)
- [Build & Install](#build--install)
- [CMake Submodule](#cmake-submodule)
- [Usage](#usage)
- [Tests](#tests)
- [Contributing](#contributing)
- [License](#license)

# Neyson
Neyson is a lightweight C++11 json library which is compliant with [RFC 8259 specification](https://tools.ietf.org/html/rfc8259) and very easy to use.

# Build & Install
If you want to install the library you can clone the project by:

``` shell
git clone https://github.com/Neyn/neyson.git
```

Then go into the project directory and create build directory:

``` shell
cd neyson
mkdir build
cd build
```

In order to build the library:

``` shell
cmake -DCMAKE_BUILD_TYPE=Release -DNEYSON_BUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX=/usr/local/ ..
cmake --build .
```

you can change the install directory by changing the value of ```CMAKE_INSTALL_PREFIX```. You can choose between building shared or static library by setting the option ```BUILD_SHARED_LIBS``` to ```ON``` or ```OFF```.

In order to install the library:

``` shell
sudo cmake --install .
```

```sudo``` might be needed or not depending on the install destination. Alternatively you can use the project as a CMake subdirectory which is explained in the section below.

# CMake Submodule
If you have installed the library you can add it to your CMake project like this:

``` cmake
find_package(neyson REQUIRED)
add_executable(myexec main.cpp)
target_link_libraries(myexec neyson::neyson)
```

Or if you prefer to use the library as a subdirectory you can add it to your CMake project like this:

``` cmake
add_subdirectory(neyson)
add_executable(myexec main.cpp)
target_link_libraries(myexec ${NEYSON_LIBRARIES})
target_include_directories(myexec PUBLIC ${NEYSON_INCLUDE_DIRS})
```

Please note that these are examples and you might want to change some stuff like the path given to ```add_subdirectory```.

# Usage
Please read the [documentation](USAGE.md) to learn how to use this library.

# Tests
You don't need to run the tests in order to use the library but if you want to you can do this in project's directory:

``` shell
mkdir build && cd build
cmake -DNEYSON_BUILD_TESTS=ON ..
cmake --build .
./Tests
```

# Contributing
You can report bugs, ask questions and request features on [issues page](../../issues). Pull requests are not accepted right now.

# License
This library is licensed under BSD 3-Clause permissive license. You can read it [here](LICENSE).
