Introduction
============
**Neyson** is a lightweight C++11 library for handling **JSON**, **XML**, and (optionally) **SQLite**. Originally focused on JSON parsing (RFC 8259-compliant), Neyson has grown into a versatile toolkit that simplifies manipulating multiple data formats. Its intuitive API makes it easy to parse, write, and work with structured data in your C++ projects.

.. contents::
   :depth: 2
   :local:

Features
--------
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

Build & Install
---------------
Neyson can be built as a standalone library or included directly in your project. Below are detailed steps to build and install from source.

1. Clone the Repository
~~~~~~~~~~~~~~~~~~~~~~~
.. code-block:: bash

   git clone https://github.com/Neyn/neyson.git
   cd neyson

2. Create a Build Directory
~~~~~~~~~~~~~~~~~~~~~~~~~~~
.. code-block:: bash

   mkdir build
   cd build

3. Configure and Build
~~~~~~~~~~~~~~~~~~~~~~
Use CMake to configure the build. You can optionally control:

- ``NEYSON_ENABLE_SQLITE=ON|OFF`` to enable or disable the SQLite module.
- ``NEYSON_BUILD_TESTS=ON|OFF`` to enable or disable tests.
- ``BUILD_SHARED_LIBS=ON|OFF`` to build shared or static libraries.
- ``CMAKE_INSTALL_PREFIX`` to set the install path (defaults vary by platform).

For example, to build a **Release** version without tests:

.. code-block:: bash

   cmake -DCMAKE_BUILD_TYPE=Release -DNEYSON_BUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX=/usr/local ..
   cmake --build .

4. Install
~~~~~~~~~~
.. code-block:: bash

   sudo cmake --install .

The ``sudo`` may or may not be necessary, depending on the system and the installation path. This step installs the Neyson headers and library files into your chosen prefix.

CMake Submodule
---------------
You can integrate Neyson in your own CMake-based project in two main ways:

1. Installed Package
~~~~~~~~~~~~~~~~~~~~
If you **installed** Neyson (see above), simply do:

.. code-block:: cmake

   find_package(neyson REQUIRED)

   add_executable(myexec main.cpp)
   target_link_libraries(myexec neyson::neyson)

CMake will locate the library and headers from your install prefix via ``find_package``.

2. Add as a Subdirectory
~~~~~~~~~~~~~~~~~~~~~~~~
If you prefer **not** to install system-wide or want to version-control Neyson within your project:

.. code-block:: cmake

   # Assuming the 'neyson' folder is inside your project tree
   add_subdirectory(neyson)

   add_executable(myexec main.cpp)
   target_link_libraries(myexec neyson::neyson)

Adjust paths as needed. This approach keeps dependencies self-contained.

Usage
-----
Neyson’s APIs are documented comprehensively in the code (with optional Doxygen integration) and in usage tutorials. Below is an overview of each module:

1. Value Class
~~~~~~~~~~~~~~
This library revolves around or make use of the core **Value** class.
It can represent none, booleans, integers, floating-point numbers, strings, arrays, and objects.

Key functionalities:
- Type checking (``isbool()``, ``isint()``, ``isreal()``, etc.)
- Getter/setter methods (``boolean()`, `integer()`, `string()`, `array()`, etc.)
- Operator indexing for arrays and objects (e.g., ``value[0]``, ``value["key"]``)
- Conversions to fundamental C++ types

Refer to the **Value Usage Tutorial** for a deep dive into constructing and manipulating **Value** objects.

2. JSON
~~~~~~~
The JSON module lives in the ``Neyson::Json`` namespace and provides:

- **read / sread / fread** to parse JSON from strings, C-strings, or files
- **write / fwrite** to serialize a **Value** to strings, files, or streams
- Detailed error reporting with the **Json::Result** structure

See the **JSON Usage Tutorial** for examples on:
- Reading JSON from various sources
- Writing JSON in compact or pretty-printed mode
- Handling parse/write errors

3. XML
~~~~~~
The XML module is under ``Neyson::XML`` and offers:

- **read / sread / fread** to parse XML (with optional trimming modes)
- **write / fwrite** to produce compact or readable XML
- A **Node** class to represent elements, attributes, comments, CDATA, etc.

See the **XML Usage Tutorial** for:
- Constructing and traversing XML nodes
- Attribute management
- Handling advanced node types like declarations and comments

4. SQLite (Optional)
~~~~~~~~~~~~~~~~~~~~
If compiled with SQLite support (``NEYSON_ENABLE_SQLITE=ON``), you gain:

- A **Database** class to open, query, and manage a connection
- **Statement** objects for prepared queries, parameter binding, row iteration
- **Transaction** to handle atomic commits and rollbacks
- **Backup** to copy data between databases

Check out the **SQLite Usage Tutorial** for detailed information on:
- Opening a database
- Executing statements (queries, inserts, etc.)
- Transactions and backups

Tests
-----
If you want to run the included test suite, enable tests when configuring:

.. code-block:: bash

   cd neyson
   mkdir build && cd build
   cmake -DNEYSON_BUILD_TESTS=ON ..
   cmake --build .
   ./Tests

This runs unit tests covering JSON, XML, and SQLite functionality (if SQLite is enabled).
