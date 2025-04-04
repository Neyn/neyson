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

#define Parse             \
    R = Json::read(V, S); \
    EXPECT_TRUE(R);       \
    if (R) EXPECT_TRUE(V == E);

#define Deparse                                 \
    R = Json::write(E, O, Json::Mode::Compact); \
    EXPECT_TRUE(R);                             \
    EXPECT_TRUE(S == O);

TEST(JSON, Read)
{
    Json::Result R;
    Value V, E;
    std::string S, O;

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

TEST(JSON, Write)
{
    Json::Result R;
    Value V, E;
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

TEST(JSON, Unicode)
{
    Json::Result R;
    Value V, E;
    std::string S, O;

    S = "\"\\u2606\"";
    E = "☆";
    Parse;

    S = "\"\\u273A\\u2606\"";
    E = "✺☆";
    Parse;

    S = "\"|\\u272B|\\u2606|\\u2732|\"";
    E = "|✫|☆|✲|";
    Parse;

    S = "\"☆\"";
    E = "☆";
    Deparse;

    S = "\"✺☆\"";
    E = "✺☆";
    Deparse;

    S = "\"|✫|☆|✲|\"";
    E = "|✫|☆|✲|";
    Deparse;
}

TEST(JSON, Random)
{
    for (size_t i = 0; i < 1000; ++i)
    {
        Json::Result R;
        String S;
        Value V, E = randvar();
        R = Json::write(E, S);
        EXPECT_TRUE(R);
        Parse;
    }
}

#undef Parse
#undef Deparse

// A helper function to create or overwrite a file with content.
static bool createFile(const std::string &path, const std::string &content)
{
    std::ofstream out(path);
    if (!out.is_open())
    {
        return false;
    }
    out << content;
    out.close();
    return true;
}

TEST(JsonReadTests, ReadValidSimpleValuesFromCString)
{
    Neyson::Value value;
    // Booleans
    {
        auto result = Json::read(value, "true");
        EXPECT_TRUE(result);
        EXPECT_EQ(result.error, Json::Error::None);
        EXPECT_TRUE(value.isbool());
        EXPECT_TRUE(value.boolean());
    }
    {
        auto result = Json::read(value, "false");
        EXPECT_TRUE(result);
        EXPECT_EQ(result.error, Json::Error::None);
        EXPECT_TRUE(value.isbool());
        EXPECT_FALSE(value.boolean());
    }
    // Null
    {
        auto result = Json::read(value, "null");
        EXPECT_TRUE(result);
        EXPECT_EQ(result.error, Json::Error::None);
        EXPECT_TRUE(value.isnull());
    }
    // Integer
    {
        auto result = Json::read(value, "123");
        EXPECT_TRUE(result);
        EXPECT_EQ(result.error, Json::Error::None);
        EXPECT_TRUE(value.isint());
        EXPECT_EQ(value.integer(), 123);
    }
    // Real
    {
        auto result = Json::read(value, "3.14");
        EXPECT_TRUE(result);
        EXPECT_EQ(result.error, Json::Error::None);
        EXPECT_TRUE(value.isreal());
        EXPECT_NEAR(value.real(), 3.14, 1e-8);
    }
    // String
    {
        auto result = Json::read(value, "\"Hello\"");
        EXPECT_TRUE(result);
        EXPECT_EQ(result.error, Json::Error::None);
        EXPECT_TRUE(value.isstr());
        EXPECT_EQ(value.string(), "Hello");
    }
}

TEST(JsonReadTests, ReadValidContainersFromCString)
{
    Neyson::Value value;

    // Array
    {
        auto result = Json::read(value, "[1, true, \"str\", null]");
        EXPECT_TRUE(result);
        EXPECT_EQ(result.error, Json::Error::None);
        EXPECT_TRUE(value.isarr());
        EXPECT_EQ(value.size(), 4u);
        EXPECT_TRUE(value[0].isint());
        EXPECT_TRUE(value[1].isbool());
        EXPECT_TRUE(value[2].isstr());
        EXPECT_TRUE(value[3].isnull());
    }

    // Object
    {
        auto result = Json::read(value, "{\"a\":1,\"b\":false,\"c\":\"text\"}");
        EXPECT_TRUE(result);
        EXPECT_EQ(result.error, Json::Error::None);
        EXPECT_TRUE(value.isobj());
        EXPECT_EQ(value.size(), 3u);
        EXPECT_EQ(value["a"].integer(), 1);
        EXPECT_FALSE(value["b"].boolean());
        EXPECT_EQ(value["c"].string(), "text");
    }

    // Nested
    {
        auto result = Json::read(value, R"json(
        {
          "nestedArr": [10, 20, {"x": 30}],
          "nestedObj": {
              "flag": true,
              "arr": [1, 2, 3]
          }
        }
        )json");
        EXPECT_TRUE(result);
        EXPECT_EQ(result.error, Json::Error::None);
        EXPECT_TRUE(value.isobj());
        auto &nestedArr = value["nestedArr"];
        EXPECT_TRUE(nestedArr.isarr());
        EXPECT_EQ(nestedArr.size(), 3u);
        EXPECT_EQ(nestedArr[2]["x"].integer(), 30);

        auto &nestedObj = value["nestedObj"];
        EXPECT_TRUE(nestedObj.isobj());
        EXPECT_TRUE(nestedObj["flag"].boolean());
        EXPECT_TRUE(nestedObj["arr"].isarr());
        EXPECT_EQ(nestedObj["arr"].size(), 3u);
    }
}

TEST(JsonReadTests, ReadInvalidValuesFromCString_ExpectErrors)
{
    Neyson::Value value;
    Json::Result result;

    // Malformed number
    result = Json::read(value, "123abc");
    EXPECT_FALSE(result);
    EXPECT_EQ(result.error, Json::Error::FailedToReachEnd);

    // Missing quotes for string
    result = Json::read(value, "hello");
    EXPECT_FALSE(result);
    EXPECT_EQ(result.error, Json::Error::UnexpectedValueStart);

    // Unclosed string
    result = Json::read(value, "\"abc");
    EXPECT_FALSE(result);
    EXPECT_EQ(result.error, Json::Error::ExpectedQuoteClose);

    // Expected colon
    result = Json::read(value, "{\"key\" 100}");
    EXPECT_FALSE(result);
    EXPECT_EQ(result.error, Json::Error::ExpectedColon);

    // Expected comma
    result = Json::read(value, "[1 2]");
    EXPECT_FALSE(result);
    EXPECT_EQ(result.error, Json::Error::ExpectedComma);

    // Unclosed bracket
    result = Json::read(value, "[1,2");
    EXPECT_FALSE(result);
    EXPECT_EQ(result.error, Json::Error::ExpectedCommaOrBracketClose);

    // Unclosed brace
    result = Json::read(value, "{\"key\":123");
    EXPECT_FALSE(result);
    EXPECT_EQ(result.error, Json::Error::ExpectedCommaOrBraceClose);

    // Extra data after valid JSON
    result = Json::read(value, "true false");
    EXPECT_FALSE(result);
    EXPECT_EQ(result.error, Json::Error::FailedToReachEnd);
}

TEST(JsonReadTests, ReadUsingStdStringOverload)
{
    Neyson::Value value;
    std::string jsonData = R"({"flag":true,"num":123})";

    auto result = Json::read(value, jsonData);
    EXPECT_TRUE(result);
    EXPECT_EQ(result.error, Json::Error::None);

    EXPECT_TRUE(value.isobj());
    EXPECT_TRUE(value["flag"].boolean());
    EXPECT_EQ(value["num"].integer(), 123);
}

TEST(JsonReadTests, FReadValidFile)
{
    Neyson::Value value;
    std::string filePath = "test_valid.json";
    std::string content = R"({"hello":"world","arr":[1,2,3]})";

    EXPECT_TRUE(createFile(filePath, content));
    auto result = Json::fread(value, filePath);
    EXPECT_TRUE(result);
    EXPECT_EQ(result.error, Json::Error::None);
    EXPECT_TRUE(value.isobj());
    EXPECT_EQ(value["hello"].string(), "world");
    EXPECT_EQ(value["arr"].size(), 3u);

    // Clean up the file if desired
    std::remove(filePath.c_str());
}

TEST(JsonReadTests, FReadInvalidFile)
{
    Neyson::Value value;
    std::string filePath = "non_existent_file.json";

    auto result = Json::fread(value, filePath);
    EXPECT_FALSE(result);
    EXPECT_EQ(result.error, Json::Error::FileIOError);
    // value should remain unchanged (likely Null)
}

TEST(JsonReadTests, FReadInvalidJsonInFile)
{
    Neyson::Value value;
    std::string filePath = "test_invalid.json";
    std::string content = "{\"missing_end\":123";  // Unclosed brace

    EXPECT_TRUE(createFile(filePath, content));
    auto result = Json::fread(value, filePath);
    EXPECT_FALSE(result);
    EXPECT_EQ(result.error, Json::Error::ExpectedCommaOrBraceClose);

    // Clean up
    std::remove(filePath.c_str());
}

TEST(JsonWriteTests, WriteToStdStringReadable)
{
    // Prepare a Value
    Neyson::Value root(Neyson::Type::Object);
    root["key"] = 123;
    root["arr"] = Neyson::Value({Neyson::Value("one"), Neyson::Value("two")});

    std::string out;
    auto result = Json::write(root, out, Json::Mode::Readable);
    EXPECT_TRUE(result);
    EXPECT_EQ(result.error, Json::Error::None);

    // We expect a "pretty" JSON. The exact formatting depends on your implementation.
    // We can at least check for curly braces, brackets, etc.
    EXPECT_FALSE(out.empty());
    EXPECT_NE(out.find("\"key\""), std::string::npos);
    EXPECT_NE(out.find("\"arr\""), std::string::npos);
}

TEST(JsonWriteTests, WriteToStdStringCompact)
{
    // Prepare a Value
    Neyson::Value root(Neyson::Type::Object);
    root["boolean"] = false;
    root["value"] = 3.14;

    std::string out;
    auto result = Json::write(root, out, Json::Mode::Compact);
    EXPECT_TRUE(result);
    EXPECT_EQ(result.error, Json::Error::None);

    // Should be more compact than Readable
    // For example: {"boolean":false,"value":3.14}
    EXPECT_FALSE(out.empty());
    EXPECT_EQ(out.front(), '{');
    EXPECT_EQ(out.back(), '}');
    EXPECT_EQ(out.find('\n'), std::string::npos);  // Typically no new lines in Compact
}

TEST(JsonWriteTests, WriteToOStream)
{
    // Prepare a Value
    Neyson::Value arrayVal = {1, 2, 3};
    std::ostringstream oss;
    auto result = Json::write(arrayVal, &oss, Json::Mode::Compact);

    EXPECT_TRUE(result);
    EXPECT_EQ(result.error, Json::Error::None);

    std::string out = oss.str();
    EXPECT_FALSE(out.empty());
    EXPECT_EQ(out, "[1,2,3]");  // Likely outcome in compact mode
}

TEST(JsonWriteTests, FWriteValidFile)
{
    Neyson::Value root(Neyson::Type::Object);
    root["name"] = "Test";
    root["flag"] = true;

    std::string filePath = "output.json";
    // Attempt writing in compact mode
    auto result = Json::fwrite(root, filePath, Json::Mode::Compact);
    EXPECT_TRUE(result);
    EXPECT_EQ(result.error, Json::Error::None);

    // Optionally, read it back to confirm content
    Neyson::Value readBack;
    auto readResult = Json::fread(readBack, filePath);
    EXPECT_TRUE(readResult);
    EXPECT_EQ(readBack["name"].string(), "Test");
    EXPECT_TRUE(readBack["flag"].boolean());

    // Clean up
    std::remove(filePath.c_str());
}

TEST(JsonWriteTests, FWriteInvalidPath)
{
    // Provide an invalid path or a location you know is not writable.
    // On some systems, writing to a path like "/root/out.json" without permissions
    // might fail. We'll simulate that:

    Neyson::Value dummy(123);
    std::string filePath = "/root/non_writable.json";

    auto result = Json::fwrite(dummy, filePath, Json::Mode::Compact);
    EXPECT_FALSE(result);
    EXPECT_EQ(result.error, Json::Error::FileIOError);
}

TEST(JsonWriteTests, RoundTripTest)
{
    // This test writes a Value to JSON and then reads it back, verifying equality.

    Neyson::Value original(Neyson::Type::Object);
    original["intVal"] = 42;
    original["boolVal"] = true;
    original["stringVal"] = "Hello";
    original["arrVal"] = Neyson::Value({1, 2.5, "str", false});

    // Nested object
    Neyson::Value nestedObj(Neyson::Type::Object);
    nestedObj["nestedKey"] = "nestedValue";
    original["objVal"] = nestedObj;

    // Write to string (Readable mode)
    std::string out;
    auto writeRes = Json::write(original, out, Json::Mode::Readable);
    EXPECT_TRUE(writeRes);
    EXPECT_EQ(writeRes.error, Json::Error::None);
    EXPECT_FALSE(out.empty());

    // Read back
    Neyson::Value parsed;
    auto readRes = Json::read(parsed, out);
    EXPECT_TRUE(readRes);
    EXPECT_EQ(readRes.error, Json::Error::None);

    // Now verify the equality. If your Value implements operator==(Value),
    // you can just do: EXPECT_TRUE(original == parsed);
    // Otherwise, check fields manually:
    EXPECT_EQ(parsed["intVal"].integer(), 42);
    EXPECT_EQ(parsed["boolVal"].boolean(), true);
    EXPECT_EQ(parsed["stringVal"].string(), "Hello");
    auto &arrVal = parsed["arrVal"];
    EXPECT_TRUE(arrVal.isarr());
    EXPECT_EQ(arrVal.size(), 4u);
    EXPECT_EQ(arrVal[0].integer(), 1);
    EXPECT_NEAR(arrVal[1].real(), 2.5, 1e-8);
    EXPECT_EQ(arrVal[2].string(), "str");
    EXPECT_FALSE(arrVal[3].boolean());

    auto &objVal = parsed["objVal"];
    EXPECT_TRUE(objVal.isobj());
    EXPECT_EQ(objVal["nestedKey"].string(), "nestedValue");
}

TEST(JsonWriteTests, CheckResultIndexUsageForParsingErrors)
{
    // Some parsers fill the `index` field in Json::Result
    // to indicate where the error was found.
    // This is a simple demonstration; adjust if your implementation differs.

    Neyson::Value value;
    auto result = Json::read(value, R"({"key": "value", "err": )");
    EXPECT_FALSE(result);
    // We expect some parse error like `ExpectedValueStart` or `ExpectedQuoteOpen`,
    // with the index around the end of the input.
    EXPECT_NE(result.index, size_t(-1));
    // We won't assert a specific index because it depends on your parser,
    // but you could do something like:
    // EXPECT_NEAR(result.index, 20u, 2u);
    // to give some range if you know approximate location.
}
