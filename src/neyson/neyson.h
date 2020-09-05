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

#define Function(TYPE, FUNC)           \
    Value(TYPE &&val);                 \
    Value(const TYPE &val);            \
    Value &operator=(TYPE &&val);      \
    Value &operator=(const TYPE &val); \
    TYPE &FUNC(TYPE &&val);            \
    TYPE &FUNC(const TYPE &val);       \
    TYPE &FUNC();                      \
    const TYPE &FUNC() const;

/// The main namespace of the library.
namespace Neyson
{
/// Error code that is in the Result class and is returned by IO::read() function.
enum class Error
{
    None,
    WrongEnd,
    WrongNumber,
    WrongString,
    WrongStart,
    WrongValue,
    FailedFileIO,
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
};

/// Type of the Value returned by Value::type() function.
enum class Type
{
    Object,
    Array,
    String,
    Real,
    Integer,
    Bool,
    Null
};

/// Result of parsing a json document returned by IO::read() function.
struct Result
{
    Error error;
    size_t index;
    inline operator bool() const { return error == Error::None; }
};

class Value;

/// Floating-point json number that is used in this library.
using Real = double;

/// Integer json number that is used in this library.
using Integer = int64_t;

/// Json string that is used in this library.
using String = std::string;

/// Json array that is used in this library.
using Array = std::vector<Value>;

/// Json object that is used in this library.
using Object = std::unordered_map<String, Value>;

/// Value class that can hold any of the json types.
class Value
{
    union Variant
    {
        Object *o;
        Array *a;
        String *s;
        Real r;
        Integer i;
        bool b;
    };

    Type _type;
    Variant _value;

public:
    Value();
    ~Value();

    Value(Value &&val);
    Value(const Value &val);
    Value(const char *val);

    Value &operator=(Value &&val);
    Value &operator=(const Value &val);
    Value &operator=(const char *val);

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

    inline bool isnull() const { return _type == Type::Null; }
    inline bool isboolean() const { return _type == Type::Bool; }
    inline bool isinteger() const { return _type == Type::Integer; }
    inline bool isreal() const { return _type == Type::Real; }
    inline bool isstring() const { return _type == Type::String; }
    inline bool isarrray() const { return _type == Type::Array; }
    inline bool isobject() const { return _type == Type::Object; }

    bool tobool() const;
    Integer tointeger() const;
    Real toreal() const;
    String tostring() const;

    void reset();
    inline Type type() const { return _type; }
    Function(Object, object) Function(Array, array) Function(String, string);
    Function(Real, real) Function(Integer, integer) Function(bool, boolean);
};

/// Writing mode of the IO::write() function.
enum class Mode
{
    Compact,
    Readable,
};

/// Namespace that contains IO operation which are reading and writing.
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
}  // namespace Neyson

#undef Function
