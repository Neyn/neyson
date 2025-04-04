Value Usage Tutorial
====================

.. contents::
   :depth: 2
   :local:

Introduction
------------
The **Value** class in the **Neyson** namespace represents a flexible data type capable of holding various kinds of values such as none, booleans, integers, floating-point numbers, strings, arrays, and objects. It is designed for JSON-like data structures and SQLite values. This tutorial will walk you through all aspects of using the **Value** class, from creation and manipulation to advanced operations, indexing, conversions, and more.

Overview of the Value Class
---------------------------
The **Value** class wraps a `union` of different possible data types:

- **Null**: Represents an empty or uninitialized value.
- **Boolean**: Represents a boolean (`true` or `false`).
- **Integer**: Represents a 64-bit signed integer (`int64_t`).
- **Real**: Represents a floating-point value (`double`).
- **String**: Represents a string (`std::string`).
- **Array**: Represents a dynamically resizable array of `Value` objects (`std::vector<Value>`).
- **Object**: Represents a key-value map of `std::string` to `Value` (`std::unordered_map<std::string, Value>`).

This section provides an overview, while the subsequent sections give in-depth coverage of usage details.

Basic Construction
------------------
Constructing a **Value** can be as straightforward as:

.. code-block:: c++

    #include <neyson/neyson.h>

    using namespace Neyson;

    int main() {
        Value v1;           // Default-initialized as Type::Null
        Value v2(true);     // Constructed as Boolean
        Value v3(42);       // Constructed as Integer
        Value v4(3.14);     // Constructed as Real
        Value v5("Hello");  // Constructed as String

        // Construct as an Array from an initializer list
        Value v6 { 1, 2, 3 };

        // Construct as an Object
        Value obj = Object({{"key1", "value1"}, {"key2", 123}});

        return 0;
    }

Detailed Constructor List
-------------------------
1. **Default Constructor**

   .. code-block:: c++

       Value();

   - Creates a **Value** of type **Null**.

2. **Boolean Constructor**

   .. code-block:: c++

       Value(bool value);

   - Creates a **Value** of type **Boolean**.

3. **Integral Constructor** (Template)

   .. code-block:: c++

       template <typename T, std::__enable_if_t<std::is_integral<T>::value, bool> = true>
       Value(T value);

   - Creates a **Value** of type **Integer** from any integral type (e.g. `int`, `long`, etc.).

4. **Floating-Point Constructor** (Template)

   .. code-block:: c++

       template <typename T, std::__enable_if_t<std::is_floating_point<T>::value, bool> = true>
       Value(T value);

   - Creates a **Value** of type **Real** from any floating-point type (e.g. `float`, `double`).

5. **C-String Constructor**

   .. code-block:: c++

       Value(const char* value);

   - Creates a **Value** of type **String** from a C-style string.

6. **String Constructor**

   .. code-block:: c++

       Value(const String& value);
       Value(String&& value);

   - Creates a **Value** of type **String** from either an lvalue or rvalue `std::string`.

7. **Array Constructor**

   .. code-block:: c++

       Value(const Array& value);
       Value(Array&& value);

   - Creates a **Value** of type **Array** from a `std::vector<Value>` or an rvalue reference to one.

8. **Object Constructor**

   .. code-block:: c++

       Value(const Object& value);
       Value(Object&& value);

   - Creates a **Value** of type **Object** from a `std::unordered_map<std::string, Value>` or an rvalue reference to one.

9. **Initializer List Constructor** (for Arrays)

   .. code-block:: c++

       Value(const std::initializer_list<Value>& list);

   - Creates a **Value** of type **Array** directly from an initializer list of other **Value** objects.

Type Inspection
---------------
There are multiple inline functions to check the type of the **Value**:

.. code-block:: c++

    bool isnull() const;  // Returns true if the type is Null
    bool isbool() const;  // Returns true if the type is Boolean
    bool isint()  const;  // Returns true if the type is Integer
    bool isreal() const;  // Returns true if the type is Real
    bool isstr()  const;  // Returns true if the type is String
    bool isarr()  const;  // Returns true if the type is Array
    bool isobj()  const;  // Returns true if the type is Object

There is also:

.. code-block:: c++

    bool isnum()  const;  // Returns true if the type is Integer or Real

Reading and Writing Values
--------------------------
Once a **Value** is created, you can access or modify the underlying data through several member functions:

1. **Boolean Accessors**

   .. code-block:: c++

       Boolean boolean() const;         // Get the stored boolean
       Boolean &boolean();              // Get the stored boolean (modifiable reference)
       Boolean &boolean(Boolean value); // Set the stored boolean, returns reference

2. **Integer Accessors**

   .. code-block:: c++

       Integer integer() const;
       Integer &integer();
       Integer &integer(Integer value);

3. **Real Accessors**

   .. code-block:: c++

       Real real() const;
       Real &real();
       Real &real(Real value);

4. **String Accessors**

   .. code-block:: c++

       const String& string() const;
       String& string();
       String& string(const String& value);
       String& string(String&& value);

5. **Array Accessors**

   .. code-block:: c++

       const Array& array() const;
       Array& array();
       Array& array(const Array& value);
       Array& array(Array&& value);

6. **Object Accessors**

   .. code-block:: c++

       const Object& object() const;
       Object& object();
       Object& object(const Object& value);
       Object& object(Object&& value);

Important:

- If you attempt to call, for example, `integer()` when the **Value** is not of type **Integer**, an exception will be thrown.
- The setter variants (e.g., `boolean(Boolean value)`) update the **Value** in-place and return the stored data as a reference for further use.

Assignment Operators
--------------------
You can also directly assign new values to an existing **Value**:

.. code-block:: c++

    inline Value &operator=(Boolean value);
    template <typename T, std::__enable_if_t<std::is_integral<T>::value, bool> = true>
    Value &operator=(T value);
    template <typename T, std::__enable_if_t<std::is_floating_point<T>::value, bool> = true>
    Value &operator=(T value);
    Value &operator=(const char *value);
    Value &operator=(const String &value);
    Value &operator=(String &&value);
    Value &operator=(const Array &value);
    Value &operator=(Array &&value);
    Value &operator=(const Object &value);
    Value &operator=(Object &&value);
    Value &operator=(const std::initializer_list<Value> &list);

For example:

.. code-block:: c++

    Value val;
    val = true;          // Now val is Boolean
    val = 42;            // Now val is Integer
    val = 3.14;          // Now val is Real
    val = "Hello world"; // Now val is String
    val = {1,2,3};       // Now val is an Array
    // etc.

Implicit and Explicit Conversions
---------------------------------
The **Value** class provides explicit operators to convert a **Value** to certain fundamental types:

.. code-block:: c++

    explicit operator Boolean() const;
    explicit operator Integer() const;
    explicit operator Real() const;
    explicit operator String() const;

Additionally, there are helper methods:

.. code-block:: c++

    Boolean tobool() const;
    Integer toint() const;
    Real toreal() const;
    String tostr() const;

These methods or operators attempt to convert the current data to the requested type. Here are the conversions that can happen:

- to boolean:

  * Null -> false
  * Boolean -> its value
  * Integer -> non-zero: true, zero: false
  * Real -> non-zero: true, close-to-zero: false
  * String -> not-empty: true, empty: false
  * Array -> not-empty: true, empty: false
  * Object -> not-empty: true, empty: false

- to integer:

  * Null -> 0
  * Boolean -> true: 1, false: 0
  * Integer -> itself
  * Real -> integer value of floating-point
  * String -> converted to integer if convertable, else an exception is thrown
  * Else -> an exception is thrown

- to real:

  * Null -> 0.0
  * Boolean -> true: 1.0, false: 0.0
  * Integer -> its value
  * Real -> itself
  * String -> converted to floating-point if convertable, else an exception is thrown
  * Else -> an exception is thrown

- to string:

  * Null -> empty string
  * Boolean -> true: "true", false: "false"
  * Integer -> its value to string
  * Real -> its value to string
  * String -> itself
  * Else -> an exception is thrown

Indexing and Iteration
----------------------
Arrays and objects (maps) can be indexed to get or set elements:

1. **Array Indexing**:

   .. code-block:: c++

       Value& operator[](size_t index);
       const Value& operator[](size_t index) const;

   Example usage:

   .. code-block:: c++

       Value arr = { 1, 2, 3 };
       arr[1] = 42; // changes second element from 2 to 42

2. **Object Indexing**:

   .. code-block:: c++

       Value& operator[](const std::string &key);
       const Value& operator[](const std::string &key) const;

   Example usage:

   .. code-block:: c++

       Value obj = Object({{"id", 1234}, {"name", "test"}});
       // retrieve
       std::cout << "ID: " << obj["id"].integer() << std::endl;

3. **Iteration**:

   - You can iterate over **Array** elements via standard C++ range-based loops or iterators:

     .. code-block:: c++

         Value arr = { 1, 2, 3 };
         for (auto& item : arr) {
             std::cout << item.integer() << std::endl;
         }

   - To iterate over an **Object**, use the underlying container (`object()`), which returns a `std::unordered_map<std::string, Value>`:

     .. code-block:: c++

       Value obj = Object({{"one", 1}, {"two", 2}});
         for (const auto& [key, val] : obj.object()) {
             std::cout << key << " -> " << val.integer() << std::endl;
         }

Pointer Access
--------------
The class provides:

.. code-block:: c++

    Value &operator()(const std::string &pointer);
    const Value &operator()(const std::string &pointer) const;

This is intended for more advanced usage (JSON Pointer navigation). If you store hierarchical data in arrays and objects, `(pointer)` can give you a direct path to the nested element.

Utility Methods
---------------
1. **Add Methods**

   .. code-block:: c++

       Value &add(const Value &value);
       Value &add(Value &&value);
       Value &add(const String &key, const Value &value);
       Value &add(const String &key, Value &&value);

   - For arrays: `add()` appends a new element to the **Array**.
   - For objects: `add(key, value)` inserts a key-value pair into the **Object**.

2. **contains**

   .. code-block:: c++

       bool contains(const String &key) const;

   - Returns `true` if the **Object** contains the specified key. Throws an exception or has undefined behavior if called on a non-object.

3. **size**

   .. code-block:: c++

       size_t size() const;

   - For an **Array**, returns the number of elements.
   - For an **Object**, returns the number of key-value pairs.
   - For other types, returns `0`.

4. **empty**

   .. code-block:: c++

       bool empty() const;

   - Returns `true` if the object is Null, or if the Array/Object has zero elements, or if the String is empty.

5. **clear**

   .. code-block:: c++

       void clear();

   - Resets the **Value** to Null if it was not Null. If it's an Array or Object, clears all elements.

6. **remove**

   .. code-block:: c++

       void remove(size_t index);
       bool remove(const String &key);

   - For an **Array**, removes the element at the given index.
   - For an **Object**, removes the key-value pair for the given key and returns `true` if found.

Copying and Moving
------------------
The **Value** class supports copy and move semantics:

- **Copy Constructor** and **Copy Assignment**: Performs a deep copy of the data.
- **Move Constructor** and **Move Assignment**: Transfers ownership of resources without copying.

Be aware that copy operations can be expensive if the **Value** is large or nested. Use moves when possible for performance.

Comparison and Hashing
----------------------
The class implements:

- **operator==**: Compares two **Value** objects for equality. Equality is type- and data-sensitive.
- **operator<**: Provides a strict-weak ordering. Typically used for sorting or storing in ordered containers.

In addition, specialized `std::hash` implementations exist for **Value**, **Array**, and **Object**, enabling use as keys in hash-based data structures (e.g., `std::unordered_map<Neyson::Value, T>`).

Exception Handling
------------------
Any misuse or type mismatch throws a `Neyson::Exception`, which extends `std::exception`. The exception includes a descriptive message about what went wrong.

Common Usage Patterns
---------------------
1. **Building a nested JSON-like object**:

   .. code-block:: c++

       Value root = Value::Object();
       root["name"] = "Example";
       root["version"] = 1;
       root["tags"] = { "tag1", "tag2", "tag3" };
       root["nested"] = Value::Object().add("key1", 42).add("key2", "value2");

2. **Reading from a Value**:

   .. code-block:: c++

       if (root.contains("name")) {
           std::string name = root["name"].string();
           std::cout << "Name: " << name << std::endl;
       }
       if (root["tags"].isarr()) {
           for (auto& tag : root["tags"]) {
               std::cout << "Tag: " << tag.string() << std::endl;
           }
       }

3. **Modifying in place**:

   .. code-block:: c++

       root["version"].integer(root["version"].integer() + 1); // Increment version

4. **Resetting**:

   .. code-block:: c++

       root.clear();   // root is now Type::Null

Summary
-------
The **Value** class is a powerful building block for representing diverse data in JSON-like or similarly structured formats. Its versatility allows for easy usage in:
- Building hierarchical objects and arrays.
- Storing and retrieving strongly typed data (booleans, integers, reals, strings).
- Iterating through arrays and objects in a natural, C++-friendly way.
- Safely performing type checks and conversions to avoid runtime errors.
