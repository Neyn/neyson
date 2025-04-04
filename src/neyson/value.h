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

#pragma once

#include <cstdint>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#define NEYSON_ASSERT(expr, msg) \
    if (!static_cast<bool>(expr)) throw Neyson::Exception((std::stringstream() << msg).str())

/// @brief Main namespace of the Neyson library.
/// @details Contains all fundamental components for handling JSON, XML, and SQLite
namespace Neyson
{
/// @brief Exception class used throughout the Neyson library.
/// @details Inherits from std::exception and stores a custom error message.
struct Exception : std::exception
{
    /// @brief The stored error message.
    std::string message;

    /// @brief Constructs an Exception with the specified message.
    /// @param message The error message associated with this exception.
    inline explicit Exception(const std::string &message) : message(message) {}

    /// @brief Returns the error message as a C-style string.
    /// @return The error message.
    inline virtual const char *what() const noexcept { return message.c_str(); }

    /// @brief Virtual destructor.
    virtual ~Exception() noexcept = default;
};

/// @brief The type enumeration that a Value can hold.
/// @details This enumeration defines the possible data types
/// that can be stored in a Value object.
enum class Type
{
    Null,     ///< Represents a null JSON value.
    Boolean,  ///< Represents a boolean JSON value.
    Integer,  ///< Represents an integer JSON value.
    Real,     ///< Represents a real (floating-point) JSON value.
    String,   ///< Represents a string JSON value.
    Array,    ///< Represents an array JSON value.
    Object,   ///< Represents an object JSON value.
};

/// @brief Overloads the insertion operator to print the Type enumeration.
/// @param os The output stream.
/// @param type The Type to be printed.
/// @return A reference to the output stream.
std::ostream &operator<<(std::ostream &os, Type type);

class Value;

/// @brief Boolean data type that a Value can hold.
using Boolean = bool;

/// @brief Integer data type that a Value can hold.
/// @details Uses a 64-bit signed integer type for storage.
using Integer = int64_t;

/// @brief Floating-point data type that a Value can hold.
/// @details Uses a double-precision floating-point type for storage.
using Real = double;

/// @brief String data type that a Value can hold.
using String = std::string;

/// @brief Array data type that a Value can hold.
/// @details Internally represented as a std::vector of Value objects.
using Array = std::vector<Value>;

/// @brief Object data type that a Value can hold.
/// @details Internally represented as a std::unordered_map of String-Value pairs.
using Object = std::unordered_map<String, Value>;

/// @brief A versatile class that can hold different JSON-compatible data types.
/// @details Encapsulates types such as null, boolean, integer, real, string,
/// array, or object. Manages memory and supports deep copying and moving of its contents.
class Value
{
    /// @brief Internal union holding the actual value.
    /// @details Depending on the current Type, the union will store one of the supported types.
    union Variant
    {
        Boolean b;
        Integer i;
        Real r;
        String *s;
        Array *a;
        Object *o;
    };

    /// @brief The current type of the stored value.
    Type _type;
    /// @brief The union holding the stored data.
    Variant _value;

public:
    /// @brief Constructs a Value with a specific Type and its default value.
    /// @param type The initial type for this Value.
    explicit Value(Type type);

    /// @brief Destructor for Value.
    /// @details Cleans up dynamically allocated resources depending on the current Type.
    inline ~Value() { reset(); }

    /// @brief Copy constructor.
    /// @details Creates a new Value by deep-copying the contents from another Value.
    /// @param other The Value to copy from.
    Value(const Value &other);

    /// @brief Move constructor.
    /// @details Transfers ownership of the resources from another Value to this one, leaving the other Value in a Null
    /// state.
    /// @param other The Value to move from.
    Value(Value &&other);

    /// @brief Copy assignment operator.
    /// @details Replaces the contents of the current Value by deep-copying from another Value.
    /// @param other The Value to copy from.
    /// @return A reference to this Value.
    Value &operator=(const Value &other);

    /// @brief Move assignment operator.
    /// @details Replaces the contents of the current Value by transferring ownership of the resources from another
    /// Value.
    /// @param other The Value to move from.
    /// @return A reference to this Value.
    Value &operator=(Value &&other);

    /// @brief Resets this Value to a Null type.
    /// @details Frees any resources associated with the current type, if applicable.
    void reset();

    ////////////////////////////////////////

    /// @brief Checks if this Value holds a Null type.
    /// @return True if the Value is Null, false otherwise.
    inline bool isnull() const { return _type == Type::Null; }

    /// @brief Checks if this Value holds a Boolean type.
    /// @return True if the Value is a boolean, false otherwise.
    inline bool isbool() const { return _type == Type::Boolean; }

    /// @brief Checks if this Value holds an Integer type.
    /// @return True if the Value is an integer, false otherwise.
    inline bool isint() const { return _type == Type::Integer; }

    /// @brief Checks if this Value holds a Real (floating-point) type.
    /// @return True if the Value is a floating-point number, false otherwise.
    inline bool isreal() const { return _type == Type::Real; }

    /// @brief Checks if this Value holds a String type.
    /// @return True if the Value is a string, false otherwise.
    inline bool isstr() const { return _type == Type::String; }

    /// @brief Checks if this Value holds an Array type.
    /// @return True if the Value is an array, false otherwise.
    inline bool isarr() const { return _type == Type::Array; }

    /// @brief Checks if this Value holds an Object type.
    /// @return True if the Value is an object, false otherwise.
    inline bool isobj() const { return _type == Type::Object; }

    /// @brief Checks if this Value holds a numeric type (Integer or Real).
    /// @return True if the Value is numeric, false otherwise.
    inline bool isnum() const { return _type == Type::Integer || _type == Type::Real; }

    ////////////////////////////////////////

    /// @brief Default constructor.
    /// @details Initializes the Value as Null.
    inline Value() : _type(Type::Null) {}

    /// @brief Constructs a Value from a boolean.
    /// @param value The boolean value to store.
    inline Value(Boolean value) : _type(Type::Boolean), _value{.b = value} {}

    /// @brief Constructs a Value from an integral type.
    /// @tparam T Must be an integral type.
    /// @param value The integral value to store.
    template <typename T, std::__enable_if_t<std::is_integral<T>::value, bool> = true>
    inline Value(T value) : _type(Type::Integer), _value{.i = Integer(value)}
    {
    }

    /// @brief Constructs a Value from a floating-point type.
    /// @tparam T Must be a floating-point type.
    /// @param value The floating-point value to store.
    template <typename T, std::__enable_if_t<std::is_floating_point<T>::value, bool> = true>
    inline Value(T value) : _type(Type::Real), _value{.r = Real(value)}
    {
    }

    /// @brief Constructs a Value from a C-style string.
    /// @param value The C-style string to store.
    inline Value(const char *value) : _type(Type::String), _value{.s = new String(value)} {}

    /// @brief Constructs a Value from a constant std::string reference.
    /// @param value The string to copy into this Value.
    inline Value(const String &value) : _type(Type::String), _value{.s = new String(value)} {}

    /// @brief Constructs a Value by moving an std::string.
    /// @param value The string to move into this Value.
    inline Value(String &&value) : _type(Type::String), _value{.s = new String(std::move(value))} {}

    /// @brief Constructs a Value from a constant Array reference.
    /// @param value The array to copy into this Value.
    inline Value(const Array &value) : _type(Type::Array), _value{.a = new Array(value)} {}

    /// @brief Constructs a Value by moving an Array.
    /// @param value The array to move into this Value.
    inline Value(Array &&value) : _type(Type::Array), _value{.a = new Array(std::move(value))} {}

    /// @brief Constructs a Value from a constant Object reference.
    /// @param value The object to copy into this Value.
    inline Value(const Object &value) : _type(Type::Object), _value{.o = new Object(value)} {}

    /// @brief Constructs a Value by moving an Object.
    /// @param value The object to move into this Value.
    inline Value(Object &&value) : _type(Type::Object), _value{.o = new Object(std::move(value))} {}

    /// @brief Constructs a Value from an initializer list of Value objects (creates an Array).
    /// @param list The initializer list to store as an array.
    inline Value(const std::initializer_list<Value> &list) : _type(Type::Array), _value{.a = new Array(list)} {}

    ////////////////////////////////////////

    /// @brief Assigns a boolean value to this Value.
    /// @details Resets the current Value, sets the type to Boolean, and stores the new boolean value.
    /// @param value The boolean value to be assigned.
    /// @return A reference to this Value.
    inline Value &operator=(Boolean value) { return (reset(), _type = Type::Boolean, _value.b = value, *this); }

    /// @brief Assigns an integral value to this Value.
    /// @tparam T An integral type (e.g., int, long, int64_t, etc.).
    /// @param value The integral value to be assigned.
    /// @return A reference to this Value.
    template <typename T, std::__enable_if_t<std::is_integral<T>::value, bool> = true>
    inline Value &operator=(T value)
    {
        return (reset(), _type = Type::Integer, _value.i = value, *this);
    }

    /// @brief Assigns a floating-point value to this Value.
    /// @tparam T A floating-point type (e.g., float, double, long double).
    /// @param value The floating-point value to be assigned.
    /// @return A reference to this Value.
    template <typename T, std::__enable_if_t<std::is_floating_point<T>::value, bool> = true>
    inline Value &operator=(T value)
    {
        return (reset(), _type = Type::Real, _value.r = value, *this);
    }

    /// @brief Assigns a C-style string to this Value.
    /// @details Resets the current Value, sets the type to String, and stores a new string.
    /// @param value The C-style string to be assigned.
    /// @return A reference to this Value.
    inline Value &operator=(const char *value)
    {
        return (reset(), _type = Type::String, _value.s = new String(value), *this);
    }

    /// @brief Assigns a std::string to this Value by copy.
    /// @details Resets the current Value, sets the type to String, and copies the string data.
    /// @param value The string to be assigned.
    /// @return A reference to this Value.
    inline Value &operator=(const String &value)
    {
        return (reset(), _type = Type::String, _value.s = new String(value), *this);
    }

    /// @brief Assigns a std::string to this Value by move.
    /// @details Resets the current Value, sets the type to String, and moves the string data.
    /// @param value The string to be moved.
    /// @return A reference to this Value.
    inline Value &operator=(String &&value)
    {
        return (reset(), _type = Type::String, _value.s = new String(std::move(value)), *this);
    }

    /// @brief Assigns an Array to this Value by copy.
    /// @details Resets the current Value, sets the type to Array, and copies the array data.
    /// @param value The array to be assigned.
    /// @return A reference to this Value.
    inline Value &operator=(const Array &value)
    {
        return (reset(), _type = Type::Array, _value.a = new Array(value), *this);
    }

    /// @brief Assigns an Array to this Value by move.
    /// @details Resets the current Value, sets the type to Array, and moves the array data.
    /// @param value The array to be moved.
    /// @return A reference to this Value.
    inline Value &operator=(Array &&value)
    {
        return (reset(), _type = Type::Array, _value.a = new Array(std::move(value)), *this);
    }

    /// @brief Assigns an Object to this Value by copy.
    /// @details Resets the current Value, sets the type to Object, and copies the object data.
    /// @param value The object to be assigned.
    /// @return A reference to this Value.
    inline Value &operator=(const Object &value)
    {
        return (reset(), _type = Type::Object, _value.o = new Object(value), *this);
    }

    /// @brief Assigns an Object to this Value by move.
    /// @details Resets the current Value, sets the type to Object, and moves the object data.
    /// @param value The object to be moved.
    /// @return A reference to this Value.
    inline Value &operator=(Object &&value)
    {
        return (reset(), _type = Type::Object, _value.o = new Object(std::move(value)), *this);
    }

    /// @brief Assigns an initializer list of Value objects to this Value as an Array.
    /// @details Resets the current Value, sets the type to Array, and copies the list data into a new Array.
    /// @param list The initializer list to be assigned.
    /// @return A reference to this Value.
    inline Value &operator=(const std::initializer_list<Value> &list)
    {
        return (reset(), _type = Type::Array, _value.a = new Array(list), *this);
    }

    ////////////////////////////////////////

    /// @brief Retrieves the boolean stored in this Value.
    /// @details Throws an exception if the current type is not Boolean.
    /// @return The stored boolean value.
    inline Boolean boolean() const
    {
        NEYSON_ASSERT(_type == Type::Boolean, "Value has type " << _type << " but you requested Boolean!");
        return _value.b;
    }

    /// @brief Retrieves a reference to the boolean stored in this Value.
    /// @details Throws an exception if the current type is not Boolean.
    /// @return A reference to the stored boolean value.
    inline Boolean &boolean()
    {
        NEYSON_ASSERT(_type == Type::Boolean, "Value has type " << _type << " but you requested Boolean!");
        return _value.b;
    }

    /// @brief Sets the Value to a boolean and returns a reference to the stored boolean.
    /// @param value The boolean value to set.
    /// @return A reference to the stored boolean value.
    inline Boolean &boolean(Boolean value) { return (*this = value, _value.b); }

    /// @brief Retrieves the integer stored in this Value.
    /// @details Throws an exception if the current type is not Integer.
    /// @return The stored integer value.
    inline Integer integer() const
    {
        NEYSON_ASSERT(_type == Type::Integer, "Value has type " << _type << " but you requested Integer!");
        return _value.i;
    }

    /// @brief Retrieves a reference to the integer stored in this Value.
    /// @details Throws an exception if the current type is not Integer.
    /// @return A reference to the stored integer value.
    inline Integer &integer()
    {
        NEYSON_ASSERT(_type == Type::Integer, "Value has type " << _type << " but you requested Integer!");
        return _value.i;
    }

    /// @brief Sets the Value to an integer and returns a reference to the stored integer.
    /// @param value The integer value to set.
    /// @return A reference to the stored integer value.
    inline Integer &integer(Integer value) { return (*this = value, _value.i); }

    /// @brief Retrieves the floating-point number stored in this Value.
    /// @details Throws an exception if the current type is not Real.
    /// @return The stored floating-point value.
    inline Real real() const
    {
        NEYSON_ASSERT(_type == Type::Real, "Value has type " << _type << " but you requested Real!");
        return _value.r;
    }

    /// @brief Retrieves a reference to the floating-point number stored in this Value.
    /// @details Throws an exception if the current type is not Real.
    /// @return A reference to the stored floating-point value.
    inline Real &real()
    {
        NEYSON_ASSERT(_type == Type::Real, "Value has type " << _type << " but you requested Real!");
        return _value.r;
    }

    /// @brief Sets the Value to a floating-point number and returns a reference to the stored value.
    /// @param value The floating-point value to set.
    /// @return A reference to the stored floating-point value.
    inline Real &real(Real value) { return (*this = value, _value.r); }

    /// @brief Retrieves the string stored in this Value (const version).
    /// @details Throws an exception if the current type is not String.
    /// @return A const reference to the stored string.
    inline const String &string() const
    {
        NEYSON_ASSERT(_type == Type::String, "Value has type " << _type << " but you requested String!");
        return *_value.s;
    }

    /// @brief Retrieves the string stored in this Value (non-const version).
    /// @details Throws an exception if the current type is not String.
    /// @return A reference to the stored string.
    inline String &string()
    {
        NEYSON_ASSERT(_type == Type::String, "Value has type " << _type << " but you requested String!");
        return *_value.s;
    }

    /// @brief Sets the Value to a string by copy and returns a reference to the stored string.
    /// @param value The string to set.
    /// @return A reference to the stored string.
    inline String &string(const String &value) { return (*this = value, *_value.s); }

    /// @brief Sets the Value to a string by move and returns a reference to the stored string.
    /// @param value The string to move.
    /// @return A reference to the stored string.
    inline String &string(String &&value) { return (*this = std::move(value), *_value.s); }

    /// @brief Retrieves the array stored in this Value (const version).
    /// @details Throws an exception if the current type is not Array.
    /// @return A const reference to the stored array.
    inline const Array &array() const
    {
        NEYSON_ASSERT(_type == Type::Array, "Value has type " << _type << " but you requested Array!");
        return *_value.a;
    }

    /// @brief Retrieves the array stored in this Value (non-const version).
    /// @details Throws an exception if the current type is not Array.
    /// @return A reference to the stored array.
    inline Array &array()
    {
        NEYSON_ASSERT(_type == Type::Array, "Value has type " << _type << " but you requested Array!");
        return *_value.a;
    }

    /// @brief Sets the Value to an array by copy and returns a reference to the stored array.
    /// @param value The array to copy.
    /// @return A reference to the stored array.
    inline Array &array(const Array &value) { return (*this = value, *_value.a); }

    /// @brief Sets the Value to an array by move and returns a reference to the stored array.
    /// @param value The array to move.
    /// @return A reference to the stored array.
    inline Array &array(Array &&value) { return (*this = std::move(value), *_value.a); }

    /// @brief Retrieves the object stored in this Value (const version).
    /// @details Throws an exception if the current type is not Object.
    /// @return A const reference to the stored object.
    inline const Object &object() const
    {
        NEYSON_ASSERT(_type == Type::Object, "Value has type " << _type << " but you requested Object!");
        return *_value.o;
    }

    /// @brief Retrieves the object stored in this Value (non-const version).
    /// @details Throws an exception if the current type is not Object.
    /// @return A reference to the stored object.
    inline Object &object()
    {
        NEYSON_ASSERT(_type == Type::Object, "Value has type " << _type << " but you requested Object!");
        return *_value.o;
    }

    /// @brief Sets the Value to an object by copy and returns a reference to the stored object.
    /// @param value The object to copy.
    /// @return A reference to the stored object.
    inline Object &object(const Object &value) { return (*this = value, *_value.o); }

    /// @brief Sets the Value to an object by move and returns a reference to the stored object.
    /// @param value The object to move.
    /// @return A reference to the stored object.
    inline Object &object(Object &&value) { return (*this = std::move(value), *_value.o); }

    ////////////////////////////////////////

    /// @brief Retrieves a copy of the stored value interpreted as a boolean.
    /// @details Calls the bool conversion operator under the hood.
    /// @return The boolean interpretation of this Value.
    inline Boolean tobool() const { return Boolean(*this); }

    /// @brief Retrieves a copy of the stored value interpreted as an integer.
    /// @details Calls the integer conversion operator under the hood.
    /// @return The integer interpretation of this Value.
    inline Integer toint() const { return Integer(*this); }

    /// @brief Retrieves a copy of the stored value interpreted as a real (floating-point) number.
    /// @details Calls the real conversion operator under the hood.
    /// @return The real (floating-point) interpretation of this Value.
    inline Real toreal() const { return Real(*this); }

    /// @brief Retrieves a copy of the stored value interpreted as a string.
    /// @details Calls the string conversion operator under the hood.
    /// @return The string interpretation of this Value.
    inline String tostr() const { return String(*this); }

    /// @brief Converts this Value to a boolean.
    /// @details If the Value cannot be converted to a boolean, a runtime exception is thrown.
    /// @return The boolean representation of this Value.
    explicit operator Boolean() const;

    /// @brief Converts this Value to an integer.
    /// @details If the Value cannot be converted to an integer, a runtime exception is thrown.
    /// @return The integer representation of this Value.
    explicit operator Integer() const;

    /// @brief Converts this Value to a real (floating-point) number.
    /// @details If the Value cannot be converted to a real, a runtime exception is thrown.
    /// @return The real (floating-point) representation of this Value.
    explicit operator Real() const;

    /// @brief Converts this Value to a string.
    /// @details If the Value cannot be converted to a string, a runtime exception is thrown.
    /// @return The string representation of this Value.
    explicit operator String() const;

    /// @brief Retrieves the current Type of this Value.
    /// @return The Type enumeration value representing this Value's type.
    inline Type type() const { return _type; }

    /// @brief Retrieves or navigates to a nested Value by a JSON Pointer-like string.
    /// @details The specifics of how 'pointer' is interpreted may be determined by the implementation.
    /// @param pointer A string that denotes the path to a nested Value.
    /// @return A reference to the nested Value.
    inline Value &operator()(const std::string &pointer);

    /// @brief Retrieves or navigates to a nested Value by a JSON Pointer-like string (const overload).
    /// @details The specifics of how 'pointer' is interpreted may be determined by the implementation.
    /// @param pointer A string that denotes the path to a nested Value.
    /// @return A const reference to the nested Value.
    inline const Value &operator()(const std::string &pointer) const;

    /// @brief Compares the current Value's type to the specified Type.
    /// @param type The Type to compare against.
    /// @return True if this Value has the same Type, false otherwise.
    inline bool operator==(Type type) const { return _type == type; }

    /// @brief Compares the current Value's type to the specified Type for inequality.
    /// @param type The Type to compare against.
    /// @return True if this Value does not have the same Type, false otherwise.
    inline bool operator!=(Type type) const { return _type != type; }

    /// @brief Checks whether this Value and another Value are equal.
    /// @details The criteria for equality may depend on the type of each Value and the contents.
    /// @param other The Value to compare against.
    /// @return True if the two Values are considered equal, false otherwise.
    bool operator==(const Value &other) const;

    /// @brief Compares this Value with another Value to determine an ordering.
    /// @param other The Value to compare against.
    /// @return True if this Value is considered less than the other, false otherwise.
    bool operator<(const Value &other) const;

    /// @brief Swaps the contents of two Value objects.
    /// @details This operation exchanges both the Value types and the underlying data.
    /// @param first The first Value object.
    /// @param second The second Value object.
    friend void swap(Value &first, Value &second);

    ////////////////////////////////////////

    /// @brief Provides array-style access to the stored data by index.
    /// @details If this Value is not an array, a runtime exception is thrown.
    /// @param index The array index to access.
    /// @return A reference to the Value at the specified index.
    inline Value &operator[](size_t index) { return array().at(index); }

    /// @brief Provides array-style access to the stored data by index (const version).
    /// @details If this Value is not an array, a runtime exception is thrown.
    /// @param index The array index to access.
    /// @return A const reference to the Value at the specified index.
    inline const Value &operator[](size_t index) const { return array().at(index); }

    /// @brief Provides object-style access to the stored data by key.
    /// @details If this Value is not an object, a runtime exception is thrown.
    /// @param key The key to access in the object.
    /// @return A reference to the Value associated with the specified key.
    inline Value &operator[](const std::string &key) { return object()[key]; }

    /// @brief Provides object-style access to the stored data by key (const version).
    /// @details If this Value is not an object, or if the key is not found, a runtime exception is thrown.
    /// @param key The key to access in the object.
    /// @return A const reference to the Value associated with the specified key.
    const Value &operator[](const std::string &key) const;

    /// @brief Returns an iterator to the beginning of the array.
    /// @details If the Value is not an array, a runtime exception is thrown.
    /// @return A const_iterator to the beginning of the array.
    inline Array::const_iterator begin() const { return array().begin(); }

    /// @brief Returns an iterator to the beginning of the array (non-const version).
    /// @details If the Value is not an array, a runtime exception is thrown.
    /// @return An iterator to the beginning of the array.
    inline Array::iterator begin() { return array().begin(); }

    /// @brief Returns an iterator to the end of the array.
    /// @details If the Value is not an array, a runtime exception is thrown.
    /// @return A const_iterator to the end of the array.
    inline Array::const_iterator end() const { return array().end(); }

    /// @brief Returns an iterator to the end of the array (non-const version).
    /// @details If the Value is not an array, a runtime exception is thrown.
    /// @return An iterator to the end of the array.
    inline Array::iterator end() { return array().end(); }

    /// @brief Adds a new element to the end of the array by copy.
    /// @details If this Value is not an array, a runtime exception is thrown.
    /// @param value The Value to be added to the array.
    /// @return A reference to this Value (for chaining).
    inline Value &add(const Value &value) { return (array().push_back(value), *this); }

    /// @brief Adds a new element to the end of the array by move.
    /// @details If this Value is not an array, a runtime exception is thrown.
    /// @param value The Value to be added (moved) into the array.
    /// @return A reference to this Value (for chaining).
    inline Value &add(Value &&value) { return (array().push_back(std::move(value)), *this); }

    /// @brief Inserts a key-value pair into the object by copy.
    /// @details If this Value is not an object, a runtime exception is thrown.
    /// @param key The key under which the Value will be stored.
    /// @param value The Value to store under the provided key.
    /// @return A reference to this Value (for chaining).
    inline Value &add(const String &key, const Value &value) { return (object().emplace(key, value), *this); }

    /// @brief Inserts a key-value pair into the object by move.
    /// @details If this Value is not an object, a runtime exception is thrown.
    /// @param key The key under which the Value will be stored.
    /// @param value The Value to store under the provided key (moved).
    /// @return A reference to this Value (for chaining).
    inline Value &add(const String &key, Value &&value) { return (object().emplace(key, std::move(value)), *this); }

    /// @brief Checks if the object contains a given key.
    /// @details If this Value is not an object, a runtime exception is thrown.
    /// @param key The key to look for in the object.
    /// @return True if the key exists in the object, false otherwise.
    bool contains(const String &key) const;

    /// @brief Returns the size of the array or object.
    /// @details If this Value is neither an array nor an object, a runtime exception is thrown.
    /// @return The size of the array or object (number of elements).
    size_t size() const;

    /// @brief Checks if the array or object is empty.
    /// @details If this Value is neither an array nor an object, a runtime exception is thrown.
    /// @return True if the array or object has no elements, false otherwise.
    bool empty() const;

    /// @brief Clears the contents of the array or object.
    /// @details If this Value is neither an array nor an object, a runtime exception is thrown.
    void clear();

    /// @brief Removes an element from the array at the specified index.
    /// @details If this Value is not an array, a runtime exception is thrown.
    /// @param index The index of the element to remove.
    void remove(size_t index);

    /// @brief Removes an element from the object by key.
    /// @details If this Value is not an object, a runtime exception is thrown.
    /// @param key The key of the element to remove.
    /// @return True if the key existed and was removed, false otherwise.
    bool remove(const String &key);
};

/// @brief Outputs a Neyson::Value to the provided output stream.
/// @details The exact textual representation is determined by the implementation.
/// @param os The output stream where the Value will be written.
/// @param value The Value to print.
/// @return A reference to the modified output stream.
std::ostream &operator<<(std::ostream &os, const Value &value);

/// @brief Outputs a Neyson::Array to the provided output stream.
/// @details The exact textual representation is determined by the implementation.
/// @param os The output stream where the Array will be written.
/// @param array The Array to print.
/// @return A reference to the modified output stream.
std::ostream &operator<<(std::ostream &os, const Array &array);

/// @brief Outputs a Neyson::Object to the provided output stream.
/// @details The exact textual representation is determined by the implementation.
/// @param os The output stream where the Object will be written.
/// @param object The Object to print.
/// @return A reference to the modified output stream.
std::ostream &operator<<(std::ostream &os, const Object &object);
}  // namespace Neyson

namespace std
{
/// @brief Specialization of std::hash for Neyson::Value.
/// @details Provides a hash function for a Neyson::Value. The implementation
/// is free to choose how the hash is computed based on the internal data.
template <>
class hash<Neyson::Value>
{
public:
    /// @brief Computes and returns a hash value for the given Neyson::Value.
    /// @param value The Neyson::Value object for which the hash is to be computed.
    /// @return A std::size_t hash value representing the Value.
    size_t operator()(const Neyson::Value &value) const;
};

/// @brief Specialization of std::hash for Neyson::Array.
/// @details Provides a hash function for a Neyson::Array. The implementation
/// is free to choose how the hash is computed based on the internal data.
template <>
class hash<Neyson::Array>
{
public:
    /// @brief Computes and returns a hash value for the given Neyson::Array.
    /// @param value The Neyson::Array object for which the hash is to be computed.
    /// @return A std::size_t hash value representing the Array.
    size_t operator()(const Neyson::Array &value) const;
};

/// @brief Specialization of std::hash for Neyson::Object.
/// @details Provides a hash function for a Neyson::Object. The implementation
/// is free to choose how the hash is computed based on the internal data.
template <>
class hash<Neyson::Object>
{
public:
    /// @brief Computes and returns a hash value for the given Neyson::Object.
    /// @param value The Neyson::Object for which the hash is to be computed.
    /// @return A std::size_t hash value representing the Object.
    size_t operator()(const Neyson::Object &value) const;
};
}  // namespace std
