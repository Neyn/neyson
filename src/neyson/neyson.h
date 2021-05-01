/*
  BSD 3-Clause License

  Copyright (c) 2020, Shahriar Rezghi <shahriar25.ss@gmail.com>
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.

  3. Neither the name of the copyright holder nor the names of its
     contributors may be used to endorse or promote products derived from
     this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include <neyson/config.h>

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

/// The main namespace of the library.
namespace Neyson
{
/// Error code that indicates results of parsing and writing JSON.
enum class Error
{
    None,
    FileIOError,
    InvalidNumber,
    InvalidString,
    InvalidValueType,
    ExpectedColon,
    ExpectedComma,
    ExpectedStart,
    ExpectedQuoteOpen,
    ExpectedQuoteClose,
    ExpectedBraceOpen,
    ExpectedBraceClose,
    ExpectedBracketOpen,
    ExpectedBracketClose,
    ExpectedCommaOrBraceClose,
    ExpectedCommaOrBracketClose,
    FailedToReachEnd,
    UnexpectedValueStart,
};

/// Type of the value that Value class holds.
enum class Type
{
    Null,
    Bool,
    Integer,
    Real,
    String,
    Array,
    Object,
};

/// Result of parsing and writing a json document returned by functions in IO namespace.
struct Result
{
    /// Error code that indicates succes or failure.
    Error error;

    /// Number of bytes read from input.
    size_t index;

    /// Bool operator that indicates succes or failure.
    inline operator bool() const { return error == Error::None; }
};

class Value;

/// Floating-point JSON number that is used in this library.
using Real = double;

/// Integer JSON number that is used in this library.
using Integer = int64_t;

/// JSON string type that is used in this library.
using String = std::string;

/// JSON array type that is used in this library.
using Array = std::vector<Value>;

/// JSON object type that is used in this library.
using Object = std::unordered_map<String, Value>;

/// Mode of writing JSON values.
enum class Mode
{
    /// Compact mode that doesn't have extra spaces and new lines to make the output readable.
    Compact,
    /// Readable mode that has extra spaces and new lines to make the output readable.
    Readable,
};

/// Namespace that contains IO operations which are for reading and writing JSON values.
namespace IO
{
/// Reader function that reads the string into value.
Result read(Value &value, const char *str);

/// Reader function that reads the string into value.
Result read(Value &value, const std::string &str);

/// Reader function that reads the file into value.
Result fread(Value &value, const std::string &path);

/// Writer function that writes value to a string and returns it.
Result write(const Value &value, std::string &data, Mode mode = Mode::Compact);

/// Writer function that writes value to the given stream.
Result write(const Value &value, std::ostream *stream, Mode mode = Mode::Compact);

/// Writer function that writes value to the given file path and returns success or failure.
Result fwrite(const Value &value, const std::string &path, Mode mode = Mode::Compact);
}  // namespace IO

/// Value class that can hold any of the JSON types.
class Value
{
    union Variant
    {
        bool b;
        Integer i;
        Real r;
        String *s;
        Array *a;
        Object *o;
    };

    Type _type;
    Variant _value;

public:
    /// Default constructor.
    Value();

    /// Destructor function.
    ~Value();

    /// Move constructor.
    Value(Value &&val);

    /// Copy constructor.
    Value(const Value &val);

    /// Constructor that takes char sequence type and sets the value to it by copying.
    Value(const char *val);

    /// Move assignment operator
    Value &operator=(Value &&val);

    /// Copy assignment operator
    Value &operator=(const Value &val);

    /// Assignment operator that takes char sequence type and sets the value to it by copying.
    Value &operator=(const char *val);

    /// Sets the value to null type.
    void reset();

    /// Getter function for type of value.
    inline Type type() const { return _type; }

    /// Conversion operator that converts the holding type to bool.
    operator bool() const;

    /// Conversion operator that converts the holding type to Integer.
    operator Integer() const;

    /// Conversion operator that converts the holding type to Real.
    operator Real() const;

    /// Conversion operator that converts the holding type to String.
    operator String() const;

    /// Comparison operator that returns true if the type is the same as type arguement otherwise false.
    inline bool operator==(Type type) const { return _type == type; }

    /// Comparison operator that returns false if the type is the same as type arguement otherwise true.
    inline bool operator!=(Type type) const { return _type != type; }

    /// Access function that returns reference to item at index arguement in the array.
    /// If the type of the value is not array an runtime exception is thrown.
    inline Value &operator[](int index) { return array()[index]; }

    /// Access function that returns reference to item at index arguement in the array.
    /// If the type of the value is not array an runtime exception is thrown.
    inline Value &operator[](size_t index) { return array()[index]; }

    /// Access function that returns reference to the value of object where key is name arguement.
    /// If the type of the value is not object an runtime exception is thrown.
    inline Value &operator[](const char *name) { return object()[name]; }

    /// Access function that returns reference to the value of object where key is name arguement.
    /// If the type of the value is not object an runtime exception is thrown.
    inline Value &operator[](const std::string &name) { return object()[name]; }

    /// Access function that returns constant reference to item at index arguement in the array.
    /// If the type of the value is not array an runtime exception is thrown.
    inline const Value &operator[](int index) const { return array()[index]; }

    /// Access function that returns constant reference to item at index arguement in the array.
    /// If the type of the value is not array an runtime exception is thrown.
    inline const Value &operator[](size_t index) const { return array()[index]; }

    /// Access function that returns reference to the value of object where key is name arguement.
    /// If the type of the value is not object an runtime exception is thrown.
    inline const Value &operator[](const char *name) const { return (*this)[std::string(name)]; }

    /// Access function that returns reference to the value of object where key is name arguement.
    /// If the type of the value is not object an runtime exception is thrown.
    const Value &operator[](const std::string &name) const;

    /// Constructor that takes value of arithmetic type and sets the value to it by copying.
    template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
    Value(const T &val)
    {
        if (std::is_integral<T>::value)
        {
            _type = Type::Integer;
            _value.i = Integer(val);
        }
        else
        {
            _type = Type::Real;
            _value.r = Real(val);
        }
    }

    /// Assignment operator that takes value of arithmetic type and sets the value to it by copying.
    template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
    Value &operator=(const T &val)
    {
        reset();
        if (std::is_integral<T>::value)
        {
            _type = Type::Integer;
            _value.i = Integer(val);
        }
        else
        {
            _type = Type::Real;
            _value.r = Real(val);
        }
        return *this;
    }

    /// Constructor that takes bool and sets the value to it by moving.
    Value(bool &&val);

    /// Constructor that takes bool type and sets the value to it by copying.
    Value(const bool &val);

    /// Assignment operator that takes bool type and sets the value to it by moving.
    Value &operator=(bool &&val);

    /// Assignment operator that takes bool type and sets the value to it by copying.
    Value &operator=(const bool &val);

    /// Setter function that takes bool type and sets the value to it by moving.
    bool &boolean(bool &&val);

    /// Setter function that takes bool type and sets the value to it by copying.
    bool &boolean(const bool &val);

    /// Getter function that returns a reference to the bool type that this class is holding.
    /// If the type that this class holds is not bool the function throws a runtime exception.
    bool &boolean();

    /// Getter function that returns a constant reference to the bool type that this class is holding.
    /// If the type that this class holds is not bool the function throws a runtime exception.
    const bool &boolean() const;

    /// Constructor that takes Integer and sets the value to it by moving.
    Value(Integer &&val);

    /// Constructor that takes Integer type and sets the value to it by copying.
    Value(const Integer &val);

    /// Assignment operator that takes Integer type and sets the value to it by moving.
    Value &operator=(Integer &&val);

    /// Assignment operator that takes Integer type and sets the value to it by copying.
    Value &operator=(const Integer &val);

    /// Setter function that takes Integer type and sets the value to it by moving.
    Integer &integer(Integer &&val);

    /// Setter function that takes Integer type and sets the value to it by copying.
    Integer &integer(const Integer &val);

    /// Getter function that returns a reference to the Integer type that this class is holding.
    /// If the type that this class holds is not Integer the function throws a runtime exception.
    Integer &integer();

    /// Getter function that returns a constant reference to the Integer type that this class is holding.
    /// If the type that this class holds is not Integer the function throws a runtime exception.
    const Integer &integer() const;

    /// Constructor that takes Real and sets the value to it by moving.
    Value(Real &&val);

    /// Constructor that takes Real type and sets the value to it by copying.
    Value(const Real &val);

    /// Assignment operator that takes Real type and sets the value to it by moving.
    Value &operator=(Real &&val);

    /// Assignment operator that takes Real type and sets the value to it by copying.
    Value &operator=(const Real &val);

    /// Setter function that takes Real type and sets the value to it by moving.
    Real &real(Real &&val);

    /// Setter function that takes Real type and sets the value to it by copying.
    Real &real(const Real &val);

    /// Getter function that returns a reference to the Real type that this class is holding.
    /// If the type that this class holds is not Real the function throws a runtime exception.
    Real &real();

    /// Getter function that returns a constant reference to the Real type that this class is holding.
    /// If the type that this class holds is not Real the function throws a runtime exception.
    const Real &real() const;

    /// Constructor that takes String and sets the value to it by moving.
    Value(String &&val);

    /// Constructor that takes String type and sets the value to it by copying.
    Value(const String &val);

    /// Assignment operator that takes String type and sets the value to it by moving.
    Value &operator=(String &&val);

    /// Assignment operator that takes String type and sets the value to it by copying.
    Value &operator=(const String &val);

    /// Setter function that takes String type and sets the value to it by moving.
    String &string(String &&val);

    /// Setter function that takes String type and sets the value to it by copying.
    String &string(const String &val);

    /// Getter function that returns a reference to the String type that this class is holding.
    /// If the type that this class holds is not String the function throws a runtime exception.
    String &string();

    /// Getter function that returns a constant reference to the String type that this class is holding.
    /// If the type that this class holds is not String the function throws a runtime exception.
    const String &string() const;

    /// Constructor that takes Array and sets the value to it by moving.
    Value(Array &&val);

    /// Constructor that takes Array type and sets the value to it by copying.
    Value(const Array &val);

    /// Assignment operator that takes Array type and sets the value to it by moving.
    Value &operator=(Array &&val);

    /// Assignment operator that takes Array type and sets the value to it by copying.
    Value &operator=(const Array &val);

    /// Setter function that takes Array type and sets the value to it by moving.
    Array &array(Array &&val);

    /// Setter function that takes Array type and sets the value to it by copying.
    Array &array(const Array &val);

    /// Getter function that returns a reference to the Array type that this class is holding.
    /// If the type that this class holds is not Array the function throws a runtime exception.
    Array &array();

    /// Getter function that returns a constant reference to the Array type that this class is holding.
    /// If the type that this class holds is not Array the function throws a runtime exception.
    const Array &array() const;

    /// Constructor that takes Object and sets the value to it by moving.
    Value(Object &&val);

    /// Constructor that takes Object type and sets the value to it by copying.
    Value(const Object &val);

    /// Assignment operator that takes Object type and sets the value to it by moving.
    Value &operator=(Object &&val);

    /// Assignment operator that takes Object type and sets the value to it by copying.
    Value &operator=(const Object &val);

    /// Setter function that takes Object type and sets the value to it by moving.
    Object &object(Object &&val);

    /// Setter function that takes Object type and sets the value to it by copying.
    Object &object(const Object &val);

    /// Getter function that returns a reference to the Object type that this class is holding.
    /// If the type that this class holds is not Object the function throws a runtime exception.
    Object &object();

    /// Getter function that returns a constant reference to the Object type that this class is holding.
    /// If the type that this class holds is not Object the function throws a runtime exception.
    const Object &object() const;
};

/// Operator for printing Error to standard stream
std::ostream &operator<<(std::ostream &os, Error error);

/// Operator for printing Type to standard stream
std::ostream &operator<<(std::ostream &os, Type type);

/// Operator for printing Result to standard stream
std::ostream &operator<<(std::ostream &os, const Result &result);

/// Operator for printing Mode to standard stream
std::ostream &operator<<(std::ostream &os, Mode mode);

/// Operator for printing Value to standard stream
std::ostream &operator<<(std::ostream &os, const Value &value);

std::ostream &operator<<(std::ostream &os, const Array &array);

std::ostream &operator<<(std::ostream &os, const Object &object);
}  // namespace Neyson

#undef Function
