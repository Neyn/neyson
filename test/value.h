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

#include "tests.h"

TEST(Value, General)
{
    Value value;
    EXPECT_TRUE(value.isnull());

    value.reset();
    EXPECT_TRUE(value.isnull());

    value = false;
    EXPECT_TRUE(value.isbool());
    EXPECT_TRUE(value.boolean() == false);

    value = int8_t(10);
    EXPECT_TRUE(value.isint());
    EXPECT_TRUE(value.integer() == 10);

    value = int16_t(10);
    EXPECT_TRUE(value.isint());
    EXPECT_TRUE(value.integer() == 10);

    value = int32_t(10);
    EXPECT_TRUE(value.isint());
    EXPECT_TRUE(value.integer() == 10);

    value = int64_t(10);
    EXPECT_TRUE(value.isint());
    EXPECT_TRUE(value.integer() == 10);

    value = uint8_t(10);
    EXPECT_TRUE(value.isint());
    EXPECT_TRUE(value.integer() == 10);

    value = uint16_t(10);
    EXPECT_TRUE(value.isint());
    EXPECT_TRUE(value.integer() == 10);

    value = uint32_t(10);
    EXPECT_TRUE(value.isint());
    EXPECT_TRUE(value.integer() == 10);

    value = uint64_t(10);
    EXPECT_TRUE(value.isint());
    EXPECT_TRUE(value.integer() == 10);

    value = .01;
    EXPECT_TRUE(value.isreal());
    EXPECT_TRUE(value.real() == .01);

    value = .01f;
    EXPECT_TRUE(value.isreal());
    EXPECT_TRUE(value.real() == .01f);

    value.reset();
    EXPECT_TRUE(value.isnull());

    value = 10;
    Value value1 = std::move(value);
    EXPECT_TRUE(value.isnull());
    EXPECT_TRUE(value1.isint());

    value = 10;
    EXPECT_THROW(value.real());
    EXPECT_THROW(value.array());
    EXPECT_THROW(value.object());

    value = Array{10};
    EXPECT_THROW(value.real());
    EXPECT_THROW(value.integer());
    EXPECT_THROW(value.object());
    EXPECT_THROW(value["test"]);

    value = Object{{"test", 10}};
    EXPECT_THROW(value.real());
    EXPECT_THROW(value.integer());
    EXPECT_THROW(value.array());
    EXPECT_THROW(value[0]);
    EXPECT_THROW(static_cast<const Value&>(value)["name"]);
}

/**
 * A helper function to create a nested structure:
 * {
 *   "nestedObj": {
 *       "intVal": 42,
 *       "arr": [ "hello", "world" ],
 *       "obj": {
 *           "boolVal": true,
 *           "doubleVal": 3.14
 *        }
 *    },
 *    "topLevelArr": [ 1, 2, 3 ]
 * }
 */
static Value createNestedValue()
{
    Object innerObj;
    innerObj["boolVal"] = Value(true);
    innerObj["doubleVal"] = Value(3.14);

    Array innerArr;
    innerArr.emplace_back("hello");
    innerArr.emplace_back("world");

    Object nestedObj;
    nestedObj["intVal"] = Value(42);
    nestedObj["arr"] = Value(innerArr);
    nestedObj["obj"] = Value(innerObj);

    Array topLevelArr = {Value(1), Value(2), Value(3)};

    Object rootObj;
    rootObj["nestedObj"] = Value(nestedObj);
    rootObj["topLevelArr"] = Value(topLevelArr);

    return Value(rootObj);
}

TEST(Value, DefaultConstructor)
{
    Value v;
    EXPECT_EQ(v.type(), Type::Null);
    EXPECT_TRUE(v.isnull());
    EXPECT_FALSE(v.isbool());
    EXPECT_FALSE(v.isint());
    EXPECT_FALSE(v.isreal());
    EXPECT_FALSE(v.isstr());
    EXPECT_FALSE(v.isarr());
    EXPECT_FALSE(v.isobj());
}

TEST(ValueTest, BooleanConstructor)
{
    Value v_true(true);
    Value v_false(false);

    EXPECT_EQ(v_true.type(), Type::Boolean);
    EXPECT_EQ(v_false.type(), Type::Boolean);
    EXPECT_TRUE(v_true.boolean());
    EXPECT_FALSE(v_false.boolean());
}

TEST(ValueTest, IntegerConstructor)
{
    Value v_int(42);
    EXPECT_EQ(v_int.type(), Type::Integer);
    EXPECT_EQ(v_int.integer(), 42);
    EXPECT_TRUE(v_int.isint());

    // Large integer
    Value v_large_int(static_cast<int64_t>(1234567890123LL));
    EXPECT_EQ(v_large_int.type(), Type::Integer);
    EXPECT_EQ(v_large_int.integer(), 1234567890123LL);
}

TEST(ValueTest, RealConstructor)
{
    Value v_double(3.14159);
    EXPECT_EQ(v_double.type(), Type::Real);
    EXPECT_NEAR(v_double.real(), 3.14159, 1e-7);
    EXPECT_TRUE(v_double.isreal());

    // Negative double
    Value v_neg(-123.456);
    EXPECT_EQ(v_neg.type(), Type::Real);
    EXPECT_NEAR(v_neg.real(), -123.456, 1e-7);
}

TEST(ValueTest, StringConstructor)
{
    Value v_str("Hello");
    EXPECT_EQ(v_str.type(), Type::String);
    EXPECT_EQ(v_str.string(), "Hello");
    EXPECT_TRUE(v_str.isstr());

    // std::string constructor
    std::string str = "World";
    Value v_str2(str);
    EXPECT_EQ(v_str2.type(), Type::String);
    EXPECT_EQ(v_str2.string(), "World");
}

TEST(ValueTest, ArrayConstructorFromInitializer)
{
    Value v_arr = {Value(1), Value(2), Value(3)};
    EXPECT_EQ(v_arr.type(), Type::Array);
    EXPECT_EQ(v_arr.size(), 3u);
    EXPECT_EQ(v_arr[0].integer(), 1);
    EXPECT_EQ(v_arr[1].integer(), 2);
    EXPECT_EQ(v_arr[2].integer(), 3);
}

TEST(ValueTest, ArrayConstructorFromVector)
{
    Array vec = {Value(10), Value(20), Value(30)};
    Value v_arr(vec);

    EXPECT_EQ(v_arr.type(), Type::Array);
    EXPECT_EQ(v_arr.size(), 3u);
    EXPECT_EQ(v_arr[0].integer(), 10);
    EXPECT_EQ(v_arr[1].integer(), 20);
    EXPECT_EQ(v_arr[2].integer(), 30);
}

TEST(ValueTest, ObjectConstructor)
{
    Object obj;
    obj["key1"] = Value(123);
    obj["key2"] = Value("value2");

    Value v_obj(obj);
    EXPECT_EQ(v_obj.type(), Type::Object);
    EXPECT_EQ(v_obj.size(), 2u);
    EXPECT_EQ(v_obj["key1"].integer(), 123);
    EXPECT_EQ(v_obj["key2"].string(), "value2");
}

TEST(ValueTest, CopyConstructor)
{
    Value original(42);
    Value copy = original;

    EXPECT_EQ(copy.type(), Type::Integer);
    EXPECT_EQ(copy.integer(), 42);
    EXPECT_EQ(original.integer(), 42);

    // Confirm they are independent
    copy.integer() = 100;
    EXPECT_EQ(original.integer(), 42);
    EXPECT_EQ(copy.integer(), 100);
}

TEST(ValueTest, MoveConstructor)
{
    Value original(42);
    Value moved(std::move(original));

    EXPECT_EQ(moved.type(), Type::Integer);
    EXPECT_EQ(moved.integer(), 42);
    EXPECT_TRUE(original.isnull());
}

TEST(ValueTest, CopyAssignment)
{
    Value original(3.14);
    Value assigned;
    assigned = original;

    EXPECT_EQ(assigned.type(), Type::Real);
    EXPECT_NEAR(assigned.real(), 3.14, 1e-8);

    assigned.real() = 2.71;
    EXPECT_NEAR(original.real(), 3.14, 1e-8);
    EXPECT_NEAR(assigned.real(), 2.71, 1e-8);
}

TEST(ValueTest, MoveAssignment)
{
    Value original("Hello World");
    Value assigned;
    assigned = std::move(original);

    EXPECT_EQ(assigned.type(), Type::String);
    EXPECT_EQ(assigned.string(), "Hello World");
    EXPECT_TRUE(original.isnull());
}

TEST(ValueTest, AssignmentOperators)
{
    Value v;
    v = true;
    EXPECT_TRUE(v.isbool());
    EXPECT_TRUE(v.boolean());

    v = 12345;
    EXPECT_TRUE(v.isint());
    EXPECT_EQ(v.integer(), 12345);

    v = 2.71828;
    EXPECT_TRUE(v.isreal());
    EXPECT_NEAR(v.real(), 2.71828, 1e-6);

    v = "test string";
    EXPECT_TRUE(v.isstr());
    EXPECT_EQ(v.string(), "test string");

    v = {Value(1), Value(2)};
    EXPECT_TRUE(v.isarr());
    EXPECT_EQ(v.size(), 2u);

    Object obj;
    obj["k"] = Value("v");
    v = obj;
    EXPECT_TRUE(v.isobj());
    EXPECT_EQ(v["k"].string(), "v");
}

TEST(ValueTest, SwapValues)
{
    Value v1(123);
    Value v2("abc");

    // Before swap
    EXPECT_EQ(v1.type(), Type::Integer);
    EXPECT_EQ(v2.type(), Type::String);

    swap(v1, v2);

    // After swap
    EXPECT_EQ(v1.type(), Type::String);
    EXPECT_EQ(v2.type(), Type::Integer);

    EXPECT_EQ(v1.string(), "abc");
    EXPECT_EQ(v2.integer(), 123);
}

TEST(ValueTest, OperatorSquareBracketsArray)
{
    Value v = {1, 2, 3};
    EXPECT_EQ(v[0].integer(), 1);
    EXPECT_EQ(v[1].integer(), 2);
    EXPECT_EQ(v[2].integer(), 3);

    // Modify
    v[1] = 42;
    EXPECT_EQ(v[1].integer(), 42);
}

TEST(ValueTest, OperatorSquareBracketsObject)
{
    Value v(Type::Object);
    v["one"] = Value(1);
    v["two"] = Value(2);

    EXPECT_TRUE(v.isobj());
    EXPECT_EQ(v["one"].integer(), 1);
    EXPECT_EQ(v["two"].integer(), 2);

    // Modify
    v["one"] = 10;
    EXPECT_EQ(v["one"].integer(), 10);
}

TEST(ValueTest, SquareBracketsOutOfRangeAccess)
{
    Value arr = {1, 2, 3};
    EXPECT_NO_THROW({
        // Safe access
        auto& val = arr[0];
        (void)val;
    });

    EXPECT_THROW({
        auto& invalidVal = arr[999];
        (void)invalidVal;
    });
}

TEST(ValueTest, AddMethods)
{
    Value arr(Type::Array);
    arr.add(Value(1));
    arr.add(Value(2));
    arr.add(3);

    EXPECT_EQ(arr.size(), 3u);
    EXPECT_EQ(arr[0].integer(), 1);
    EXPECT_EQ(arr[1].integer(), 2);
    EXPECT_EQ(arr[2].integer(), 3);

    Value obj(Type::Object);
    obj.add("a", Value("alpha"));
    obj.add("b", Value("beta"));

    EXPECT_TRUE(obj.isobj());
    EXPECT_EQ(obj["a"].string(), "alpha");
    EXPECT_EQ(obj["b"].string(), "beta");
}

TEST(ValueTest, RemoveMethods)
{
    // Array removal
    Value arr = {1, 2, 3, 4};
    arr.remove(1);  // remove second element
    EXPECT_EQ(arr.size(), 3u);
    EXPECT_EQ(arr[0].integer(), 1);
    EXPECT_EQ(arr[1].integer(), 3);
    EXPECT_EQ(arr[2].integer(), 4);

    EXPECT_THROW({ arr.remove(999); });
    EXPECT_EQ(arr.size(), 3u);  // unchanged, presumably

    // Object removal
    Value obj(Type::Object);
    obj["k1"] = 10;
    obj["k2"] = 20;
    obj["k3"] = 30;
    bool removed = obj.remove("k2");
    EXPECT_TRUE(removed);
    EXPECT_EQ(obj.size(), 2u);
    EXPECT_EQ(obj["k1"].integer(), 10);
    EXPECT_EQ(obj["k3"].integer(), 30);

    // Removing a non-existent key
    bool removedAgain = obj.remove("not_found");
    EXPECT_FALSE(removedAgain);
    EXPECT_EQ(obj.size(), 2u);
}

TEST(ValueTest, ClearAndEmpty)
{
    // Array
    Value arr = {1, 2, 3};
    EXPECT_FALSE(arr.empty());
    arr.clear();
    EXPECT_TRUE(arr.empty());
    EXPECT_EQ(arr.size(), 0u);
    EXPECT_TRUE(arr.isarr());

    // Re-add elements after clear
    arr.add(100);
    EXPECT_FALSE(arr.empty());
    EXPECT_EQ(arr.size(), 1u);
    EXPECT_EQ(arr[0].integer(), 100);

    // Object
    Value obj(Type::Object);
    obj["k1"] = 10;
    EXPECT_FALSE(obj.empty());
    obj.clear();
    EXPECT_TRUE(obj.empty());
    EXPECT_EQ(obj.size(), 0u);
    EXPECT_TRUE(obj.isobj());

    // Re-add after clear
    obj.add("k2", 123);
    EXPECT_FALSE(obj.empty());
    EXPECT_EQ(obj.size(), 1u);
    EXPECT_EQ(obj["k2"].integer(), 123);
}

TEST(ValueTest, NestedStructures)
{
    Value root = createNestedValue();
    EXPECT_TRUE(root.isobj());
    EXPECT_EQ(root.size(), 2u);

    const Value& nestedObj = root["nestedObj"];
    EXPECT_TRUE(nestedObj.isobj());
    EXPECT_EQ(nestedObj["intVal"].integer(), 42);
    EXPECT_TRUE(nestedObj["arr"].isarr());
    EXPECT_EQ(nestedObj["arr"].size(), 2u);
    EXPECT_EQ(nestedObj["arr"][0].string(), "hello");
    EXPECT_EQ(nestedObj["arr"][1].string(), "world");

    const Value& innerObj = nestedObj["obj"];
    EXPECT_TRUE(innerObj["boolVal"].boolean());
    EXPECT_NEAR(innerObj["doubleVal"].real(), 3.14, 1e-7);

    const Value& topArr = root["topLevelArr"];
    EXPECT_TRUE(topArr.isarr());
    EXPECT_EQ(topArr.size(), 3u);
    EXPECT_EQ(topArr[0].integer(), 1);
    EXPECT_EQ(topArr[1].integer(), 2);
    EXPECT_EQ(topArr[2].integer(), 3);
}

TEST(ValueTest, Comparisons)
{
    Value v1(42);
    Value v2(42);
    Value v3(43);

    EXPECT_EQ(v1, v2);
    EXPECT_NE(v1, v3);

    Value s1("abc");
    Value s2("abc");
    Value s3("abcd");

    EXPECT_TRUE(s1 == s2);
    EXPECT_FALSE(s1 == s3);

    // Different types
    Value b(true);
    EXPECT_NE(b, v1);  // boolean vs. integer
    EXPECT_NE(b, v1);
}

TEST(ValueTest, TypeCastingOperators)
{
    Value b(true);
    Boolean boolVal = static_cast<Boolean>(b);
    EXPECT_TRUE(boolVal);

    Value i(100);
    Integer intVal = static_cast<Integer>(i);
    EXPECT_EQ(intVal, 100);

    Value r(3.14);
    Real realVal = static_cast<Real>(r);
    EXPECT_NEAR(realVal, 3.14, 1e-8);

    Value s("test");
    String strVal = static_cast<String>(s);
    EXPECT_EQ(strVal, "test");
}
