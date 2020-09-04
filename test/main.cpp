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

#include <neyson/neyson.h>

#include <cmath>
#include <ctime>
#include <iostream>
#include <random>

#define CLEAR "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define SEPARATOR "--------------------" << endl

#define CHECK(expr)                                                                                          \
    if (!static_cast<bool>(expr))                                                                            \
    {                                                                                                        \
        cout << RED << "\"" << #expr << "\" Failed at Line " << to_string(__LINE__) << "!" << CLEAR << endl; \
        throw 0;                                                                                             \
    }

#define NTHROW(expr)                                                                                         \
    try                                                                                                      \
    {                                                                                                        \
        expr;                                                                                                \
    }                                                                                                        \
    catch (...)                                                                                              \
    {                                                                                                        \
        cout << RED << "\"" << #expr << "\" Failed at Line " << to_string(__LINE__) << "!" << CLEAR << endl; \
        throw 0;                                                                                             \
    }

#define TEST(name)                                                                      \
    void name##Test_();                                                                 \
    void name##Test()                                                                   \
    {                                                                                   \
        cout << #name << " Test Started..." << endl;                                    \
        try                                                                             \
        {                                                                               \
            name##Test_();                                                              \
            cout << GREEN << #name << " Test succeeded!" << CLEAR << endl << SEPARATOR; \
        }                                                                               \
        catch (...)                                                                     \
        {                                                                               \
            Code = 1;                                                                   \
            cout << RED << #name << " Test Failed!" << CLEAR << endl << SEPARATOR;      \
        }                                                                               \
    }                                                                                   \
    void name##Test_()

#define Parse                                                  \
    R = IO::read(V, S);                                        \
    if (!R) cout << "Error: " << Errors[int(R.error)] << endl; \
    CHECK(R);                                                  \
    if (R) NTHROW(Checker::check(V, E));

#define Deparse       \
    O = IO::write(E); \
    CHECK(S == O);

using namespace std;
using namespace Neyson;

int Code = 0;

vector<string> Errors = {
    "None",
    "WrongEnd",
    "WrongNumber",
    "WrongString",
    "WrongStart",
    "ExpectedColon",
    "ExpectedComma",
    "ExpectedStart",
    "ExpectedQuoteOpen",
    "ExpectedQuoteClose",
    "ExpectedBraceOpen",
    "ExpectedBraceClose",
    "ExpectedBracketOpen",
    "ExpectedBracketClose",
    "ExpectedCommaOrBraceClose",
    "ExpectedCommaOrBracketClose",
};
vector<string> Types = {"Object", "Array", "String", "Real", "Integer", "Bool", "Null"};

namespace Checker
{
void checkValue(const Value &value1, const Value &value2);

void checkArray(const Array &array1, const Array &array2)
{
    if (array1.size() != array2.size()) throw 0;
    for (size_t i = 0; i < array1.size(); ++i) checkValue(array1[i], array2[i]);
}

void checkObject(const Object &object1, const Object &object2)
{
    if (object1.size() != object2.size()) throw 0;
    for (const auto &pair : object1)
    {
        auto it = object2.find(pair.first);
        if (it == object2.end()) throw 0;
        checkValue(pair.second, it->second);
    }
}

void checkValue(const Value &value1, const Value &value2)
{
    if (value1.type() != value2.type())
        cout << "Type: " << Types[int(value1.type())] << " " << Types[int(value2.type())] << endl;

    auto type = value1.type();
    if (value1.type() != value2.type()) throw 0;
    if (type == Type::Object) checkObject(value1.object(), value2.object());
    if (type == Type::Array) checkArray(value1.array(), value2.array());
    if (type == Type::String && value1.string() != value2.string()) throw 0;
    if (type == Type::Real && abs(value1.real() - value2.real()) > std::numeric_limits<Real>::epsilon()) throw 0;
    if (type == Type::Integer && value1.integer() != value2.integer()) throw 0;
    if (type == Type::Bool && value1.boolean() != value2.boolean()) throw 0;
}

void check(const Value &value1, const Value &value2) { checkValue(value1, value2); }
}  // namespace Checker

namespace Random
{
size_t Count = 1000;
Value randomValue();

string randomString()
{
    string str(rand() % 10, 0);
    for (auto &c : str) c = rand() % 128;
    return str;
}

Object randomObject()
{
    Object object;
    auto size = min(Count, size_t(rand() % 10));
    Count -= size;
    for (size_t i = 0; i < size; ++i) object.insert({randomString(), randomValue()});
    return object;
}

Array randomArray()
{
    auto size = min(Count, size_t(rand() % 10));
    Count -= size;
    Array array(size);
    for (auto &value : array) value = randomValue();
    return array;
}

Real randomReal() { return (rand() / 2 + 1) / Real(RAND_MAX); }

Integer randomInt() { return rand(); }

Value randomValue()
{
    auto type = Type(rand() % int(Type::Null));
    if (type == Type::Object) return randomObject();
    if (type == Type::Array) return randomArray();
    if (type == Type::String) return randomString();
    if (type == Type::Real) return randomReal();
    if (type == Type::Integer) return randomInt();
    if (type == Type::Bool) return bool(rand() % 2);
    return Value();
}

Value random()
{
    Count = 1000;
    return randomValue();
}
}  // namespace Random

TEST(Value)
{
    Value value;
    CHECK(value.type() == Type::Null);

    value.reset();
    CHECK(value.type() == Type::Null);

    value = false;
    CHECK(value.type() == Type::Bool);
    CHECK(value.boolean() == false);

    value = int8_t(10);
    CHECK(value.type() == Type::Integer);
    CHECK(value.integer() == 10);

    value = int16_t(10);
    CHECK(value.type() == Type::Integer);
    CHECK(value.integer() == 10);

    value = int32_t(10);
    CHECK(value.type() == Type::Integer);
    CHECK(value.integer() == 10);

    value = int64_t(10);
    CHECK(value.type() == Type::Integer);
    CHECK(value.integer() == 10);

    value = uint8_t(10);
    CHECK(value.type() == Type::Integer);
    CHECK(value.integer() == 10);

    value = uint16_t(10);
    CHECK(value.type() == Type::Integer);
    CHECK(value.integer() == 10);

    value = uint32_t(10);
    CHECK(value.type() == Type::Integer);
    CHECK(value.integer() == 10);

    value = uint64_t(10);
    CHECK(value.type() == Type::Integer);
    CHECK(value.integer() == 10);

    value = .01;
    CHECK(value.type() == Type::Real);
    CHECK(value.real() == .01);

    value = .01f;
    CHECK(value.type() == Type::Real);
    CHECK(value.real() == .01f);

    value.reset();
    CHECK(value.type() == Type::Null);

    value = 10;
    Value value1 = std::move(value);
    CHECK(value.type() == Type::Null);
    CHECK(value1.type() == Type::Integer);
}

TEST(Read)
{
    Result R;
    Value V, E;
    std::string S;

    S = "0";
    E = 0;
    Parse;

    S = "0.0";
    E = 0.0;
    Parse;

    S = "-0.0";
    E = 0.0;
    Parse;

    S = "+0.0";
    E = 0.0;
    Parse;

    S = ".0";
    E = 0.0;
    Parse;

    S = "3.141592653589793238462643383279";
    E = 3.141592653589793238462643383279;
    Parse;

    S = "true";
    E = true;
    Parse;

    S = "false";
    E = false;
    Parse;

    S = "null";
    E = Value();
    Parse;

    S = "\"\"";
    E = std::string();
    Parse;

    S = "\"\\\"\\\"\"";
    E = "\"\"";
    Parse;

    S = "[]";
    E = Array();
    Parse;

    S = "[null]";
    E = Array{Value()};
    Parse;

    S = "{}";
    E = Object();
    Parse;

    S = "{\"\":null}";
    E = Object{{"", Value()}};
    Parse;
}

TEST(Write)
{
    Value E;
    std::string S, O;

    S = "0";
    E = 0;
    Deparse;

    S = "3.141592653589793";
    E = 3.141592653589793238462643383279;
    Deparse;

    S = "true";
    E = true;
    Deparse;

    S = "false";
    E = false;
    Deparse;

    S = "null";
    E = Value();
    Deparse;

    S = "\"\"";
    E = std::string();
    Deparse;

    S = "\"\\\"\\\"\"";
    E = "\"\"";
    Deparse;

    S = "[]";
    E = Array();
    Deparse;

    S = "[null]";
    E = Array{Value()};
    Deparse;

    S = "{}";
    E = Object();
    Deparse;

    S = "389751105";
    E = 389751105;
    Deparse;

    S = "18.85826309";
    E = 18.85826309;
    Deparse;
}

TEST(Random)
{
    for (size_t i = 0; i < 1000; ++i)
    {
        Result R;
        Value V, E = Random::random();
        auto S = IO::write(E);
        Parse;

        if (false)
        {
            for (size_t i = 0; i < 80; ++i) cout << '-';
            cout << endl;
            cout << S << endl;
            for (size_t i = 0; i < 80; ++i) cout << '-';
            cout << endl;
        }
    }
}

int main()
{
    srand(static_cast<unsigned int>(time(nullptr)));
    cout << SEPARATOR << "Running Tests..." << endl << SEPARATOR;
    ValueTest();
    ReadTest();
    WriteTest();
    RandomTest();
    cout << "Tests Done!" << endl << SEPARATOR;
    return Code;
}
