# Neyson

![image](docs/_static/logo-128.png)

**Neyson** is a lightweight C++11 library for handling **JSON**, **XML**, and (optionally) **SQLite**. Originally focused on JSON parsing (RFC 8259-compliant), Neyson has grown into a versatile toolkit that simplifies manipulating multiple data formats. Its intuitive API makes it easy to parse, write, and work with structured data in your C++ projects.

- [Neyson](#neyson)
  - [Features](#features)
  - [Build \& Install](#build--install)
    - [1. Clone the Repository](#1-clone-the-repository)
    - [2. Create a Build Directory](#2-create-a-build-directory)
    - [3. Configure and Build](#3-configure-and-build)
    - [4. Install](#4-install)
  - [CMake Submodule](#cmake-submodule)
    - [1. Installed Package](#1-installed-package)
    - [2. Add as a Subdirectory](#2-add-as-a-subdirectory)
  - [Usage and Documentation](#usage-and-documentation)
  - [Tests](#tests)
  - [Contributing](#contributing)
  - [License](#license)

## Features

- **JSON**:
  - RFC-8259-compliant parsing and writing
  - Full support for objects, arrays, strings, numbers, booleans, and null values

- **XML**:
  - Flexible parsing with multiple modes (trimmed or untrimmed whitespace, etc.)
  - Handles elements, attributes, comments, CDATA sections, declarations, DOCTYPE, and more

- **SQLite** (optional):
  - A simplified C++ wrapper around SQLite for database interactions
  - Easy parameter binding, row-by-row stepping, transactions, and backups

- **Easy Integration**:
  - Simple, modern C++ API
  - Single include header for all modules (or selective use of each module)
  - CMake-friendly build system

## Build & Install

Neyson can be built as a standalone library or included directly in your project. Below are detailed steps to build and install from source.

### 1. Clone the Repository

```bash
git clone https://github.com/Neyn/neyson.git
cd neyson
```

### 2. Create a Build Directory

```bash
mkdir build
cd build
```

### 3. Configure and Build

Use CMake to configure the build. You can optionally control:

- `NEYSON_ENABLE_SQLITE=ON|OFF` to enable or disable the SQLite module.
- `NEYSON_BUILD_TESTS=ON|OFF` to enable or disable tests.
- `BUILD_SHARED_LIBS=ON|OFF` to build shared or static libraries.
- `CMAKE_INSTALL_PREFIX` to set the install path (defaults vary by platform).

For example, to build a **Release** version without tests:

```bash
cmake -DCMAKE_BUILD_TYPE=Release -DNEYSON_BUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX=/usr/local ..
cmake --build .
```

### 4. Install

```bash
sudo cmake --install .
```

The `sudo` may or may not be necessary, depending on the system and the installation path. This step installs the Neyson headers and library files into your chosen prefix.

## CMake Submodule

You can integrate Neyson in your own CMake-based project in two main ways:

### 1. Installed Package

If you **installed** Neyson (see above), simply do:

```cmake
find_package(neyson REQUIRED)

add_executable(myexec main.cpp)
target_link_libraries(myexec neyson::neyson)
```

CMake will locate the library and headers from your install prefix via `find_package`.

### 2. Add as a Subdirectory

If you prefer **not** to install system-wide or want to version-control Neyson within your project:

```cmake
# Assuming the 'neyson' folder is inside your project tree
add_subdirectory(neyson)

add_executable(myexec main.cpp)
target_link_libraries(myexec neyson::neyson)
```

Adjust paths as needed. This approach keeps dependencies self-contained.

## Usage and Documentation

The documentation is available [here](https://neyson.readthedocs.io/en/latest/).

## Tests

If you want to run the included test suite, enable tests when configuring:

```bash
cd neyson
mkdir build && cd build
cmake -DNEYSON_BUILD_TESTS=ON ..
cmake --build .
./Tests
```

This runs unit tests covering JSON, XML, and SQLite functionality (if SQLite is enabled).

## Contributing

Issues, bug reports, feature requests, and pull requests are welcome! You can submit them on the project’s [issues page](../../issues). Please start a discussion about pull requests before you send them on the issues page.

## License

Neyson is released under the **BSD 3-Clause** permissive license, allowing you to use, modify, and distribute it freely for both open-source and commercial projects. You can find the full text in the [LICENSE](LICENSE) file.  
