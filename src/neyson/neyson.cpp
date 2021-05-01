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

#include "neyson.h"

#include <cmath>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>

#define Skip(ret)                                \
    parser.ptr += strspn(parser.ptr, " \t\r\n"); \
    if (parser.ptr[0] == '\0') return ret;

#define See(str)                            \
    parser.ptr += strcspn(parser.ptr, str); \
    if (parser.ptr[0] == '\0') return r;

#define Read(chr, ret) \
    if ((parser.ptr++)[0] != chr) return ret;

#define Assert(expr, msg) \
    if (!static_cast<bool>(expr)) throw std::runtime_error(std::string("") + msg);

#define Construct(C, T, N, A, F, P)     \
    Value::Value(C &&val)               \
    {                                   \
        _type = Type::T;                \
        _value.N = A C(std::move(val)); \
    }                                   \
    Value::Value(const C &val)          \
    {                                   \
        _type = Type::T;                \
        _value.N = A C(val);            \
    }

#define Assign(C, T, N, A, F, P)          \
    Value &Value::operator=(C &&val)      \
    {                                     \
        reset();                          \
        _type = Type::T;                  \
        _value.N = A C(std::move(val));   \
        return *this;                     \
    }                                     \
    Value &Value::operator=(const C &val) \
    {                                     \
        reset();                          \
        _type = Type::T;                  \
        _value.N = A C(val);              \
        return *this;                     \
    }

#define Return(C, T, N, A, F, P)                                                                               \
    C &Value::F(C &&val)                                                                                       \
    {                                                                                                          \
        reset();                                                                                               \
        *this = std::move(val);                                                                                \
        return F();                                                                                            \
    }                                                                                                          \
    C &Value::F(const C &val)                                                                                  \
    {                                                                                                          \
        reset();                                                                                               \
        *this = val;                                                                                           \
        return F();                                                                                            \
    }                                                                                                          \
                                                                                                               \
    C &Value::F()                                                                                              \
    {                                                                                                          \
        Assert(_type == Type::T, "Value has type " + TypeName[int(_type)] + " but you requested " + #T + "!"); \
        return P _value.N;                                                                                     \
    }                                                                                                          \
    const C &Value::F() const                                                                                  \
    {                                                                                                          \
        Assert(_type == Type::T, "Value has type " + TypeName[int(_type)] + " but you requested " + #T + "!"); \
        return P _value.N;                                                                                     \
    }

#define Function(F)                      \
    F(bool, Bool, b, , boolean, )        \
    F(Integer, Integer, i, , integer, )  \
    F(Real, Real, r, , real, )           \
    F(String, String, s, new, string, *) \
    F(Array, Array, a, new, array, *)    \
    F(Object, Object, o, new, object, *)

namespace Neyson
{
const char *TypeName[] = {

    "Null", "Bool", "Integer", "Real", "String", "Array", "Object",
};

struct Parser
{
    const char *ptr;
};

Value::~Value() { reset(); }

Value::Value() : _type(Type::Null) {}

Value::Value(const char *val)
{
    _type = Type::String;
    _value.s = new String(val);
}

Value::Value(const Value &val) : _type(val._type), _value(val._value)
{
    if (_type == Type::Object) _value.o = new Object(*val._value.o);
    if (_type == Type::Array) _value.a = new Array(*val._value.a);
    if (_type == Type::String) _value.s = new String(*val._value.s);
}

Value::Value(Value &&val) : _type(val._type), _value(val._value) { val._type = Type::Null; }

Value &Value::operator=(const char *val)
{
    reset();
    _type = Type::String;
    _value.s = new String(val);
    return *this;
}

Value &Value::operator=(Value &&val)
{
    _type = val._type;
    _value = val._value;
    val._type = Type::Null;
    return *this;
}

Value &Value::operator=(const Value &val)
{
    reset();
    _type = val._type;
    _value = val._value;
    if (_type == Type::Object) _value.o = new Object(*val._value.o);
    if (_type == Type::Array) _value.a = new Array(*val._value.a);
    if (_type == Type::String) _value.s = new String(*val._value.s);
    return *this;
}

void Value::reset()
{
    if (_type == Type::Object) delete _value.o;
    if (_type == Type::Array) delete _value.a;
    if (_type == Type::String) delete _value.s;
    _type = Type::Null;
}

Function(Construct) Function(Assign) Function(Return);

const Value &Value::operator[](const std::string &name) const
{
    const auto &object = this->object();
    auto it = object.find(name);
    Assert(it != object.end(), "Object doesn't have key \"" + name + "\"!");
    return it->second;
}

Neyson::Value::operator bool() const
{
    if (_type == Type::Null) return false;
    if (_type == Type::Bool) return boolean();
    if (_type == Type::Integer) return bool(integer());
    if (_type == Type::Real) return std::abs(real()) < std::numeric_limits<Real>::epsilon();
    if (_type == Type::String) return !string().empty();
    if (_type == Type::Array) return !array().empty();
    if (_type == Type::Object) return !object().empty();
    throw std::runtime_error("Value is not convertable to boolean!");
}

Neyson::Value::operator Integer() const
{
    if (_type == Type::Null) return 0;
    if (_type == Type::Bool) return Integer(boolean());
    if (_type == Type::Integer) return integer();
    if (_type == Type::Real) return Integer(real());
    if (_type == Type::String) return std::stoll(string());
    throw std::runtime_error("Value is not convertable to integer!");
}

Neyson::Value::operator Real() const
{
    if (_type == Type::Null) return 0.0;
    if (_type == Type::Bool) return Real(boolean());
    if (_type == Type::Integer) return Real(integer());
    if (_type == Type::Real) return real();
    if (_type == Type::String) return std::stod(string());
    throw std::runtime_error("Value is not convertable to real!");
}

Neyson::Value::operator String() const
{
    if (_type == Type::Null) return "";
    if (_type == Type::Bool) return std::to_string(boolean());
    if (_type == Type::Integer) return std::to_string(integer());
    if (_type == Type::Real) return std::to_string(real());
    if (_type == Type::String) return string();
    throw std::runtime_error("Value is not convertable to string!");
}

Error readValue(Value &value, Parser &parser);

bool fixString(String &string, const char *ptr, size_t len)
{
    size_t j = 0;
    string.resize(len);
    bool escape = false;

    for (size_t i = 0; i < len; ++i)
        if (!escape)
        {
            if (ptr[i] == '\\')
                escape = true;
            else
                string[j++] = ptr[i];
        }
        else
        {
            escape = false;
            if (ptr[i] == '\"')
                string[j++] = '\"';
            else if (ptr[i] == '\\')
                string[j++] = '\\';
            else if (ptr[i] == '/')
                string[j++] = '/';
            else if (ptr[i] == 'b')
                string[j++] = '\b';
            else if (ptr[i] == 'f')
                string[j++] = '\f';
            else if (ptr[i] == 'n')
                string[j++] = '\n';
            else if (ptr[i] == 'r')
                string[j++] = '\r';
            else if (ptr[i] == 't')
                string[j++] = '\t';
            else if (ptr[i] == 'u')
            {
                uint32_t code = 0;
                if (++i + 4 > len) return false;
                for (int j = 0; j < 4; ++i, ++j)
                {
                    code <<= 4;
                    if (ptr[i] >= '0' && ptr[i] <= '9')
                        code += ptr[i] - '0';
                    else if (ptr[i] >= 'a' && ptr[i] <= 'f')
                        code += ptr[i] - 'a' + 10;
                    else if (ptr[i] >= 'A' && ptr[i] <= 'F')
                        code += ptr[i] - 'A' + 10;
                    else
                        return false;
                }
                --i;

                if (code <= 0x7F)
                    string[j++] = char(code);
                else if (code <= 0x07FF)
                {
                    string[j++] = char(((code >> 6) & 0x1F) | 0xC0);
                    string[j++] = char((code & 0x3F) | 0x80);
                }
                else if (code <= 0xFFFF)
                {
                    string[j++] = char(((code >> 12) & 0x0F) | 0xE0);
                    string[j++] = char(((code >> 6) & 0x3F) | 0x80);
                    string[j++] = char((code & 0x3F) | 0x80);
                }
                else if (code <= 0x10FFFF)
                {
                    string[j++] = char(((code >> 18) & 0x07) | 0xF0);
                    string[j++] = char(((code >> 12) & 0x3F) | 0x80);
                    string[j++] = char(((code >> 6) & 0x3F) | 0x80);
                    string[j++] = char((code & 0x3F) | 0x80);
                }
                else
                    return false;
            }
            else
                return false;
        }

    string.erase(j);
    string.shrink_to_fit();
    return 0;
}

Error readString(String &string, Parser &parser)
{
    Read('\"', Error::ExpectedQuoteOpen);
    auto ptr = parser.ptr;
    for (bool state = true; parser.ptr[0] != '\0'; ++parser.ptr)
    {
        if (parser.ptr[0] == '\"' && state) break;
        state = (parser.ptr[0] == '\\') ? !state : true;
    }

    if (parser.ptr[0] == '\0') return Error::ExpectedQuoteClose;
    fixString(string, ptr, parser.ptr - ptr);
    ++parser.ptr;
    return Error::None;
}

Error readObject(Object &object, Parser &parser)
{
    Read('{', Error::ExpectedBraceOpen);
    auto it = object.begin();
    while (true)
    {
        Skip(Error::ExpectedBraceClose);
        if (parser.ptr[0] == '}')
        {
            ++parser.ptr;
            return Error::None;
        }

        String string;
        auto error = readString(string, parser);
        if (error != Error::None) return error;
        Skip(Error::ExpectedColon) Read(':', Error::ExpectedColon);

        Value value;
        error = readValue(value, parser);
        if (error != Error::None) return error;
        it = object.emplace_hint(it, std::move(string), std::move(value));
        Skip(Error::ExpectedCommaOrBraceClose) if (parser.ptr[0] != '}') Read(',', Error::ExpectedComma);
    }
}

Error readArray(Array &array, Parser &parser)
{
    Read('[', Error::ExpectedBracketOpen);
    while (true)
    {
        Skip(Error::ExpectedBracketClose);
        if (parser.ptr[0] == ']')
        {
            ++parser.ptr;
            return Error::None;
        }

        Value value;
        auto error = readValue(value, parser);
        if (error != Error::None) return error;
        array.push_back(std::move(value));
        Skip(Error::ExpectedCommaOrBracketClose) if (parser.ptr[0] != ']') Read(',', Error::ExpectedComma);
    }
}

Error readNumber(Value &number, Parser &parser)
{
    std::string string(parser.ptr, strspn(parser.ptr, "-+.eE0123456789"));
    // clang-format off
    if (string.find_first_of(".eE") == std::string::npos)
        try{ number = std::stoll(string); } catch (...) { return Error::InvalidNumber; }
    else try{ number = std::stod(string); } catch (...) { return Error::InvalidNumber; }
    // clang-format on
    parser.ptr += string.size();
    return Error::None;
}

Error readValue(Value &value, Parser &parser)
{
    Skip(Error::ExpectedStart);
    if (parser.ptr[0] == '{') return readObject(value.object({}), parser);
    if (parser.ptr[0] == '[') return readArray(value.array({}), parser);
    if (parser.ptr[0] == '\"') return readString(value.string({}), parser);
    if (strchr("-+.0123456789", parser.ptr[0]) != NULL) return readNumber(value, parser);

    if (strncmp(parser.ptr, "true", 4) == 0)
    {
        parser.ptr += 4;
        value = true;
        return Error::None;
    }
    if (strncmp(parser.ptr, "false", 5) == 0)
    {
        parser.ptr += 5;
        value = false;
        return Error::None;
    }
    if (strncmp(parser.ptr, "null", 4) == 0)
    {
        parser.ptr += 4;
        value = Value();
        return Error::None;
    }
    return Error::UnexpectedValueStart;
}

Error unfixString(String &string)
{
    String output;
    output.reserve(string.size());
    for (size_t i = 0; i < string.size(); ++i)
    {
        if ((string[i] & 0xF8) == 0xF0)
        {
            if (i + 4 > string.size()) return Error::InvalidString;
            output.append(string.data() + i, 4);
            i += 3;
        }
        else if ((string[i] & 0xF0) == 0xE0)
        {
            if (i + 3 > string.size()) return Error::InvalidString;
            output.append(string.data() + i, 3);
            i += 2;
        }
        else if ((string[i] & 0xE0) == 0xC0)
        {
            if (i + 2 > string.size()) return Error::InvalidString;
            output.append(string.data() + i, 2);
            i += 1;
        }

        else if (string[i] == '\"')
            output.append("\\\"");
        else if (string[i] == '\\')
            output.append("\\\\");
        else if (string[i] == '/')
            output.append("\\/");
        else if (string[i] == '\b')
            output.append("\\b");
        else if (string[i] == '\f')
            output.append("\\f");
        else if (string[i] == '\n')
            output.append("\\n");
        else if (string[i] == '\r')
            output.append("\\r");
        else if (string[i] == '\t')
            output.append("\\t");

        else if (string[i] < 10)
            output.append("\\u000").push_back('0' + string[i]);
        else if (string[i] < 16)
            output.append("\\u000").push_back('A' + string[i] - 10);
        else if (string[i] < 26)
            output.append("\\u001").push_back('0' + string[i] - 16);
        else if (string[i] < 32)
            output.append("\\u001").push_back('A' + string[i] - 26);

        else
            output.push_back(string[i]);
    }

    string = output;
    return Error::None;
}

Error writeString(String string, std::ostream *stream)
{
    auto error = unfixString(string);
    if (error != Error::None) return error;
    *stream << '\"' << string << '\"';
    return Error::None;
}

Error writeReal(double number, std::ostream *stream)
{
    if (std::isnan(number) || std::isinf(number)) return Error::InvalidNumber;
    *stream << std::setprecision(16) << number;
    return Error::None;
}

Error writeValue(const Value &value, std::ostream *stream);

Error writeObject(const Object &object, std::ostream *stream)
{
    size_t i = 0;
    *stream << '{';
    for (const auto &pair : object)
    {
        if (i++ != 0) *stream << ',';
        auto error = writeString(pair.first, stream);
        if (error != Error::None) return error;

        *stream << ':';
        error = writeValue(pair.second, stream);
        if (error != Error::None) return error;
    }

    *stream << '}';
    return Error::None;
}

Error writeArray(const Array &array, std::ostream *stream)
{
    *stream << '[';
    for (size_t i = 0; i < array.size(); ++i)
    {
        if (i != 0) *stream << ',';
        auto error = writeValue(array[i], stream);
        if (error != Error::None) return error;
    }

    *stream << ']';
    return Error::None;
}

Error writeValue(const Value &value, std::ostream *stream)
{
    if (value.type() == Type::Object) return writeObject(value.object(), stream);
    if (value.type() == Type::Array) return writeArray(value.array(), stream);
    if (value.type() == Type::String) return writeString(value.string(), stream);
    if (value.type() == Type::Real) return writeReal(value.real(), stream);

    if (value.type() == Type::Integer)
        *stream << value.integer();
    else if (value.type() == Type::Bool)
        *stream << (value.boolean() ? "true" : "false");
    else if (value.type() == Type::Null)
        *stream << "null";
    else
        return Error::InvalidValueType;
    return Error::None;
}

Error writeValue(const Value &value, std::ostream *stream, size_t indent);

Error writeObject(const Object &object, std::ostream *stream, size_t indent)
{
    if (object.empty())
    {
        *stream << "{}";
        return Error::None;
    }

    size_t i = 0;
    *stream << "{\n";
    for (const auto &pair : object)
    {
        if (i++ != 0) *stream << ",\n";
        *stream << std::string((indent + 1) * 4, ' ');
        auto error = writeString(pair.first, stream);
        if (error != Error::None) return error;

        *stream << ": ";
        error = writeValue(pair.second, stream, indent + 1);
        if (error != Error::None) return error;
    }

    *stream << '\n' << std::string(indent * 4, ' ') << '}';
    return Error::None;
}

Error writeArray(const Array &array, std::ostream *stream, size_t indent)
{
    if (array.empty())
    {
        *stream << "[]";
        return Error::None;
    }

    *stream << "[\n";
    for (size_t i = 0; i < array.size(); ++i)
    {
        if (i != 0) *stream << ",\n";
        *stream << std::string((indent + 1) * 4, ' ');
        auto error = writeValue(array[i], stream, indent + 1);
        if (error != Error::None) return error;
    }

    *stream << '\n' << std::string(indent * 4, ' ') << ']';
    return Error::None;
}

Error writeValue(const Value &value, std::ostream *stream, size_t indent)
{
    if (value.type() == Type::Object) return writeObject(value.object(), stream, indent);
    if (value.type() == Type::Array) return writeArray(value.array(), stream, indent);
    if (value.type() == Type::String) return writeString(value.string(), stream);
    if (value.type() == Type::Real) return writeReal(value.real(), stream);

    if (value.type() == Type::Integer)
        *stream << value.integer();
    else if (value.type() == Type::Bool)
        *stream << (value.boolean() ? "true" : "false");
    else if (value.type() == Type::Null)
        *stream << "null";
    else
        return Error::InvalidValueType;
    return Error::None;
}

namespace IO
{
Result read(Value &value, const char *str)
{
    value.reset();
    Parser parser{str};
    auto error = readValue(value, parser);
    parser.ptr += strspn(parser.ptr, " \t\r\n");
    return Result{error, size_t(parser.ptr - str)};
}

Result read(Value &value, const std::string &str)
{
    auto result = read(value, str.c_str());
    if (result.error == Error::None && result.index != str.size()) result.error = Error::FailedToReachEnd;
    return result;
}

Result fread(Value &value, const std::string &path)
{
    auto file = fopen(path.c_str(), "r");
    if (file == NULL) return Result{Error::FileIOError, 0};

    fseek(file, 0, SEEK_END);
    std::string data(ftell(file), 0);
    rewind(file);
    fread(&data[0], 1, data.size(), file);

    fclose(file);
    return read(value, data);
}

Result write(const Value &value, std::ostream *stream, Mode mode)
{
    if (mode == Mode::Readable) return Result{writeValue(value, stream, 0), 0};
    return Result{writeValue(value, stream), 0};
}

Result write(const Value &value, std::string &data, Mode mode)
{
    std::ostringstream stream;
    auto result = write(value, &stream, mode);
    data = stream.str();
    return result;
}

Result fwrite(const Value &value, const std::string &path, Mode mode)
{
    std::ofstream stream(path);
    if (!stream.is_open()) return Result{Error::FileIOError, 0};
    return write(value, &stream, mode);
}
}  // namespace IO
}  // namespace Neyson
