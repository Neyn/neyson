# Table of Contents
- [Table of Contents](#table-of-contents)
- [Neyson](#neyson)
- [Build & Install](#build--install)
- [Adding to CMake Project](#adding-to-cmake-project)
- [Usage](#usage)
  - [Data Types](#data-types)
  - [Reading](#reading)
  - [Writing](#writing)
  - [Value Class](#value-class)
- [Tests](#tests)
- [Issues](#issues)
- [License](#license)

# Neyson
Neyson is a lightweight C++11 json library which is compliant with [RFC 8259](https://tools.ietf.org/html/rfc8259) and very easy to use. 

# Build & Install
If you want to install the library you can clone the project by:

``` shell
git clone --recursive https://github.com/Neyn/neyson.git
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

# Adding to CMake Project
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
target_include_directories(myexec PUBLIC ${NEYSON_INCLUDE_DIR})
```

Please note that these are examples and you might want to change some stuff like the path given to ```add_subdirectory```.

# Usage
The API of this library is in namespace ```Neyson``` and you can access them by including ```#include <neyson/neyson.h>``` in your code. Please note that this library only handles UTF-8 strings.

## Data Types
The data types that this library provides are:

+ ```Neyson::Value``` which is the class that holds any json value type.
+ ```Neyson::Array``` which is the container that holds an array of json values (which is std::vector<Neyson::Value>).
+ ```Neyson::Object``` which is the container that holds a json object (which is std::unordered_map<Neyson::String, Neyson::Value>).
+ ```Neyson::String``` which is the container that holds a string (which is std::string).
+ ```Neyson::Integer``` which is the integer type that holds an integer json number (which is int64_t).
+ ```Neyson::Real``` which is the floating-point type that holds an floating-point json number (which is double).

Some of the types above are typedefs of C++ containers and types but they might change in future so in order for the code to be compatible use these types instead.

## Reading
In order to parse a json document you can use ```Neyson::IO::read``` function which takes a ```Neyson::Value``` and a ```std::string``` or a ```const char *``` and reads the string into the given value and returns ```Neyson::Result``` class which contains the error code and size of bytes read from the string. Note that if you give ```std::string``` as input to this function and the parser doesn't reach the end of the string the result will have ```Neyson::Error::WrongEnd``` error code. Here is an example of reading a json document:

``` c++
using namespace Neyson;
const char *data = "{\"id\":1, \"name\":\"Alex\", \"value\":0.2}";

Value document;
auto result = IO::read(document, data);
assert(result);

Object object = document.object();
cout << object["id"].integer() << endl;
cout << object["name"].string() << endl;
cout << object["value"].real() << endl;
```

Output:

```
1
Alex
0.2
```

## Writing
You can serialize a json value into a string or a file or a custom stream. This can be done using ```Neyson::IO::write``` function which takes ```Neyson::Value``` as the first argument. If you don't pass the second argument the function returns the serialized data as a ```std::string```. If you pass a ```std::string``` as the second argument holding a path to a file the function returns ```bool``` indicating success or failure. If you pass a ```std::ostream *``` as the second argument then the data will be written to the stream. There is also another argument you can pass which is ```Neyson::Mode``` which currently consists of compact mode (no spaces and new lines) and human readable mode. Here is an example of writing a json document.

``` c++
using namespace Neyson;

Object object;
object["id"] = 1;
object["name"] = "Alex";
object["value"] = 0.2;

cout << IO::write(object, Mode::Readable) << endl;
```

Output:

```
{
    "value": 0.2,
    "name": "Alex",
    "id": 1
}
```

## Value Class
The method for setting and getting types to values are similar so here is an example for integer type:

``` c++
Value value; // is null here
value = 10; // set to 10
value.integer(10); // another way to set to 10
Integer integer = value.integer(); // get value
bool isinteger = value.isinteger(); // is the value integer?
Real real = value.toreal(); // get value of other type
```

You can set a value to null by:

``` c++
Value value = 10;
value.reset(); // set to null
value = Value(); // another way to set to null
```

# Tests
You don't need to run the tests in order to use the library but if you want to you can do this in project's directory:

``` shell
cmake -DNEYSON_BUILD_TESTS=ON ..
cmake --build .
./Tests
```

# Contributing
You can report bugs, ask questions and request features on [issues page](../../issues) but please don't send pull requests (they will be closed).

# License
This library is licensed under BSD 3-Clause permissive license. You can read it [here](LICENSE).
