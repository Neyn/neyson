# Table of Contents
- [Table of Contents](#table-of-contents)
- [Introduction](#introduction)
- [Data Types](#data-types)
- [Reading](#reading)
- [Validation](#validation)
- [Writing](#writing)
- [Value](#value)

# Introduction
The API of this library is in namespace ```Neyson``` and you can access them by including ```#include <neyson/neyson.h>``` in your code. Please note that this library only handles UTF-8 strings so strings given to the library must be convert to UTF-8 if they are not(perhaps with ```std::codecvt```).

# Data Types
The data types that this library provides are:

+ ```Neyson::Value``` which is the class that holds any json value type.
+ ```Neyson::Array``` which is the container that holds an array of json values (which is ```std::vector<Neyson::Value>```).
+ ```Neyson::Object``` which is the container that holds a json object (which is ```std::unordered_map<Neyson::String, Neyson::Value>```).
+ ```Neyson::String``` which is the container that holds a string (which is ```std::string```).
+ ```Neyson::Integer``` which is the integer type that holds an integer json number (which is ```int64_t```).
+ ```Neyson::Real``` which is the floating-point type that holds an floating-point json number (which is ```double```).

Some of the types above are aliases of C++ containers and types but they might change in future so in order for the code to be compatible use these types instead.

# Reading
You can parse json documents using ```Neyson::IO::read``` and ```Neyson::IO::fread```. Note that if the parser doesn't reach the end of the string the result will have ```Neyson::Error::WrongEnd``` error code (except when passing ```const char *```). Here is an example of reading a json document:

Here is an example of parsing a json string from char sequence:

``` c++
using namespace Neyson;
const char *data = "{\"id\":1, \"name\":\"Alex\", \"value\":0.2}";
Value document;
Result result = IO::read(document, data);

if (document != Type::Object)
{
    cerr << "Document root is not object!" << endl;
    exit(1);
}

Object object = document.object();
cout << object["id"].integer() << endl;
cout << object["name"].string() << endl;
cout << object["value"].real() << endl;
```

Here is an example of parsing a json string from ```std::string```:

``` c++
using namespace Neyson;
std::string data = "{\"id\":1, \"name\":\"Alex\", \"value\":0.2}";
Value document;
Result result = IO::read(document, data);
```

Here is an example of parsing a json string from a file:

``` c++
using namespace Neyson;
std::string path = "document.json";
Value document;
Result result = IO::fread(path, data);
```

# Validation
You can check if the result of reading or writing is successful by analyzing ```Neyson::Result```.

Here is an example:

``` c++
using namespace Neyson;
Result result; // fill it with return value of read or write.

if (!result)
{
    if (result.error == Error::FailedFileIO)
        std::cerr << "Failed to read file!" << std::endl;
    else
        ; // or check other error codes.
}
cout << "Read " << result.index << " bytes!" << std::endl;
```

# Writing
You can write json documents using ```Neyson::IO::write``` and ```Neyson::IO::fwrite```. The last argument you can pass is ```Neyson::Mode``` which currently consists of compact mode (no spaces and new lines) and human readable mode.

Here is an example of writing a json document to a string:

``` c++
using namespace Neyson;

Object object;
object["id"] = 1;
object["name"] = "Alex";
object["value"] = 0.2;

std::string data;
Result result = IO::write(object, data, Mode::Readable);
cout << data << endl;
```

Here is an example of writing a json document to a file:

``` c++
using namespace Neyson;
Value document; // fill it with object and arrays and etc (like above).
std::string path = "document.json";
Result result = IO::fwrite(value, path, Mode::Readable);
cout << data << endl;
```

Here is an example of writing a json document to a stream:

``` c++
using namespace Neyson;
Value document; // fill it with object and arrays and etc (like above).
std::ostringstream stream;
Result result = IO::fwrite(value, &stream, Mode::Readable);
cout << stream.str() << endl;
```

# Value
The method for setting and getting types to values are similar so here is an example for integer type:

``` c++
Value value; // is null here
value = 10; // set to 10
value.integer(10); // another way to set to 10
Integer integer = value.integer(); // get value
bool isinteger = value == Type::Integer; // is the value integer?
Real real = static_cast<Real>(value); // get value of other type
```

You can set a value to null by:

``` c++
Value value = 10;
value.reset(); // set to null
value = Value(); // another way to set to null
```

There is also ```operator[]``` to make accessing object and array elements easier:

``` c++
Value value = {10, 10, 10};
cout << value[0] << endl;
cout << value[1] << endl;
cout << value[2] << endl;

value = {{"A", 10}, {"B", 20}};
cout << value["A"] << endl;
cout << value["B"] << endl;
```
