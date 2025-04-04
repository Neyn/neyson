// BSD 3-Clause License
//
// Copyright (c) 2020-2025, Shahriar Rezghi <shahriar.rezghi.sh@gmail.com>
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "value.h"

#include <limits>

namespace Neyson
{
std::string _search(const std::string &pointer, size_t &start)
{
    NEYSON_ASSERT(pointer[start] == '/', "Invalid pointer syntax");
    size_t end = pointer.find('/', start);
    auto ref = pointer.substr(start, end);
    start = end;

    size_t index = 0;
    while (true)
    {
        index = ref.find("~1", index);
        if (index == std::string::npos) break;
        ref.replace(index, 2, "/");
    }

    index = 0;
    while (true)
    {
        index = ref.find("~0", index);
        if (index == std::string::npos) break;
        ref.replace(index, 2, "~");
    }

    return ref;
}

Value &search(Value &value, const std::string &pointer, size_t start)
{
    if (pointer.size() <= start) return value;
    auto ref = _search(pointer, start);

    if (value.isobj())
        return search(value[ref], pointer, start);
    else if (value.isarr())
    {
        size_t index;
        // clang-format off
        try { index = std::stoul(ref); }
        catch(...) { NEYSON_ASSERT(false, "Invalid pointer syntax"); }
        // clang-format on
        Array &array = value.array();
        if (array.size() <= index) array.resize(index + 1);
        return search(array[index], pointer, start);
    }
    else
        NEYSON_ASSERT(false, "The pointer references a nonexistent value");
}

const Value &search(const Value &value, const std::string &pointer, size_t start)
{
    if (pointer.size() <= start) return value;
    auto ref = _search(pointer, start);

    if (value.isobj())
    {
        const Object &object = value.object();
        auto it = object.find(ref);
        NEYSON_ASSERT(it != object.end(), "The pointer references a nonexistent value");
        return search(it->second, pointer, start);
    }
    else if (value.isarr())
    {
        size_t index;
        // clang-format off
        try { index = std::stoul(ref); }
        catch(...) { NEYSON_ASSERT(false, "Invalid pointer syntax"); }
        // clang-format on
        const Array &array = value.array();
        NEYSON_ASSERT(index < array.size(), "The pointer references a nonexistent value");
        return search(array[index], pointer, start);
    }
    else
        NEYSON_ASSERT(false, "The pointer references a nonexistent value");
}

Value::Value(Type type) : _type(type)
{
    if (type == Type::Boolean)
        _value.b = false;
    else if (type == Type::Integer)
        _value.i = 0;
    else if (type == Type::Real)
        _value.r = 0;
    else if (type == Type::String)
        _value.s = new String();
    else if (type == Type::Array)
        _value.a = new Array();
    else if (type == Type::Object)
        _value.o = new Object();
}

Value::Value(const Value &other) : _type(other._type), _value(other._value)
{
    if (_type == Type::String) _value.s = new String(*_value.s);
    if (_type == Type::Array) _value.a = new Array(*_value.a);
    if (_type == Type::Object) _value.o = new Object(*_value.o);
}

Value::Value(Value &&other) : _type(other._type), _value(other._value) { other._type = Type::Null; }

void Value::reset()
{
    if (_type == Type::String) delete _value.s;
    if (_type == Type::Array) delete _value.a;
    if (_type == Type::Object) delete _value.o;
    _type = Type::Null;
}

Value &Value::operator=(Value &&other)
{
    reset();
    _type = other._type;
    _value = other._value;
    other._type = Type::Null;
    return *this;
}

Value &Value::operator=(const Value &other)
{
    reset();
    _type = other._type;
    _value = other._value;
    if (_type == Type::String) _value.s = new String(*_value.s);
    if (_type == Type::Array) _value.a = new Array(*_value.a);
    if (_type == Type::Object) _value.o = new Object(*_value.o);
    return *this;
}

Value &Value::operator()(const std::string &pointer) { return search(*this, pointer, 0); }

const Value &Value::operator()(const std::string &pointer) const { return search(*this, pointer, 0); }

Value::operator Boolean() const
{
    if (_type == Type::Null) return false;
    if (_type == Type::Boolean) return boolean();
    if (_type == Type::Integer) return Boolean(integer());
    if (_type == Type::Real) return std::abs(real()) >= std::numeric_limits<Real>::epsilon();
    if (_type == Type::String) return !string().empty();
    if (_type == Type::Array) return !array().empty();
    if (_type == Type::Object) return !object().empty();
    NEYSON_ASSERT(false, "Value is not convertable to Boolean!");
}

Value::operator Integer() const
{
    if (_type == Type::Null) return 0;
    if (_type == Type::Boolean) return Integer(boolean());
    if (_type == Type::Integer) return integer();
    if (_type == Type::Real) return Integer(real());
    if (_type == Type::String) return std::stoll(string());
    NEYSON_ASSERT(false, "Value is not convertable to Integer!");
}

Value::operator Real() const
{
    if (_type == Type::Null) return 0.0;
    if (_type == Type::Boolean) return Real(boolean());
    if (_type == Type::Integer) return Real(integer());
    if (_type == Type::Real) return real();
    if (_type == Type::String) return std::stod(string());
    NEYSON_ASSERT(false, "Value is not convertable to Real!");
}

Value::operator String() const
{
    if (_type == Type::Null) return "";
    if (_type == Type::Boolean) return boolean() ? "true" : "false";
    if (_type == Type::Integer) return std::to_string(integer());
    if (_type == Type::Real) return std::to_string(real());
    if (_type == Type::String) return string();
    NEYSON_ASSERT(false, "Value is not convertable to String!");
}

bool Value::operator<(const Value &other) const
{
    if (_type != other._type) return _type < other._type;
    if (_type == Type::Null) return false;
    if (_type == Type::Boolean) return _value.b < other._value.b;
    if (_type == Type::Integer) return _value.i < other._value.i;
    if (_type == Type::Real) return _value.r < other._value.r;
    if (_type == Type::String) return *_value.s < *other._value.s;
    if (_type == Type::Array) return *_value.a < *other._value.a;
    if (_type == Type::Object) return _value.o->size() < other._value.o->size();
    return false;
}

bool Value::contains(const String &key) const
{
    const Object &object = this->object();
    return object.find(key) != object.end();
}

size_t Value::size() const
{
    if (_type == Type::Array) return _value.a->size();
    if (_type == Type::Object) return _value.o->size();
    NEYSON_ASSERT(false, "Value must be object or array!");
}

bool Value::empty() const
{
    if (_type == Type::Array) return _value.a->empty();
    if (_type == Type::Object) return _value.o->empty();
    NEYSON_ASSERT(false, "Value must be object or array!");
}

void Value::clear()
{
    if (_type == Type::Array) return _value.a->clear();
    if (_type == Type::Object) return _value.o->clear();
    NEYSON_ASSERT(false, "Value must be object or array!");
}

void Value::remove(size_t index)
{
    Array &value = array();
    NEYSON_ASSERT(index < value.size(), "Index to be removed is out of range!");
    return (value.erase(value.begin() + index), (void)0);
}

bool Value::remove(const String &key) { return object().erase(key); }

void swap(Value &first, Value &second)
{
    std::swap(first._type, second._type);
    std::swap(first._value, second._value);
}

const Value &Value::operator[](const std::string &key) const
{
    const Object &object = this->object();
    auto it = object.find(key);
    NEYSON_ASSERT(it != object.end(), "Object doesn't have key \"" + key + "\"!");
    return it->second;
}

bool Value::operator==(const Value &other) const
{
    const auto eps = std::numeric_limits<Real>::epsilon();
    if (isint() && other.isreal()) return std::abs(_value.i - other._value.r) <= eps;
    if (isreal() && other.isint()) return std::abs(_value.r - other._value.i) <= eps;

    if (_type != other._type) return false;
    if (_type == Type::Null) return true;
    if (_type == Type::Boolean) return _value.b == other._value.b;
    if (_type == Type::Integer) return _value.i == other._value.i;
    if (_type == Type::Real) return std::abs(_value.r - other._value.r) <= eps;
    if (_type == Type::String) return *_value.s == *other._value.s;
    if (_type == Type::Array) return *_value.a == *other._value.a;
    if (_type == Type::Object) return *_value.o == *other._value.o;
    return false;
}

std::ostream &operator<<(std::ostream &os, Type type)
{
    if (type == Type::Null) return os << "Type::Null";
    if (type == Type::Boolean) return os << "Type::Boolean";
    if (type == Type::Integer) return os << "Type::Integer";
    if (type == Type::Real) return os << "Type::Real";
    if (type == Type::String) return os << "Type::String";
    if (type == Type::Array) return os << "Type::Array";
    if (type == Type::Object) return os << "Type::Object";
    return os << "Type::Unknown";
}

std::ostream &operator<<(std::ostream &os, const Value &value)
{
    if (value == Type::Null) return os << "Null";
    if (value == Type::Boolean) return os << (value.boolean() ? "True" : "False");
    if (value == Type::Integer) return os << value.integer();
    if (value == Type::Real) return os << value.real();
    if (value == Type::String) return os << value.string();
    if (value == Type::Array) return os << value.array();
    if (value == Type::Object) return os << value.object();
    return os << "Unknown";
}

std::ostream &operator<<(std::ostream &os, const Array &array)
{
    os << "[";
    for (auto it = array.begin(); it != array.end(); it = std::next(it))
        os << *it << (std::next(it) == array.end() ? "" : ", ");
    return os << "]";
}

std::ostream &operator<<(std::ostream &os, const Object &object)
{
    os << "{";
    for (auto it = object.begin(); it != object.end(); it = std::next(it))
        os << it->first << ":" << it->second << (std::next(it) == object.end() ? "" : ", ");
    return os << "}";
}
}  // namespace Neyson

namespace std
{
inline size_t combine(size_t A, size_t B)  //
{
    return A ^ B + 0x9e3779b9 + (A << 6) + (A >> 2);
}

size_t hash<Neyson::Value>::operator()(const Neyson::Value &value) const
{
    uint64_t type = hash<int>()(int(value.type())), data = 0;
    if (value.isbool()) data = hash<Neyson::Boolean>()(value.boolean());
    if (value.isint()) data = hash<Neyson::Integer>()(value.integer());
    if (value.isreal()) data = hash<Neyson::Real>()(value.real());
    if (value.isstr()) data = hash<Neyson::String>()(value.string());
    if (value.isarr()) data = hash<Neyson::Array>()(value.array());
    if (value.isobj()) data = hash<Neyson::Object>()(value.object());
    return combine(type, data);
}

size_t hash<Neyson::Array>::operator()(const Neyson::Array &value) const
{
    size_t data = 0;
    for (const auto &item : value)  //
        data = combine(data, hash<Neyson::Value>()(item));
    return data;
}

size_t hash<Neyson::Object>::operator()(const Neyson::Object &value) const
{
    size_t data = 0;
    for (const auto &item : value)
    {
        auto A = hash<Neyson::String>()(item.first);
        auto B = hash<Neyson::Value>()(item.second);
        data = combine(data, combine(A, B));
    }
    return data;
}
}  // namespace std
