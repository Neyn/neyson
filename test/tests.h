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

#include <neyson/neyson.h>

#include <cmath>
#include <ctime>
#include <fstream>
#include <functional>
#include <iostream>
#include <random>
#include <vector>

#define CLEAR "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define SEPARATOR "--------------------" << std::endl

#define EXPECT_TRUE(expr)                                                                    \
    if (!static_cast<bool>(expr))                                                            \
    {                                                                                        \
        std::cout << RED << "\"" << #expr << "\" is True Failed at " << __FILE__ << " Line " \
                  << std::to_string(__LINE__) << "!" << CLEAR << std::endl;                  \
        throw 0;                                                                             \
    }

#define EXPECT_FALSE(expr)                                                                    \
    if (static_cast<bool>(expr))                                                              \
    {                                                                                         \
        std::cout << RED << "\"" << #expr << "\" is False Failed at " << __FILE__ << " Line " \
                  << std::to_string(__LINE__) << "!" << CLEAR << std::endl;                   \
        throw 0;                                                                              \
    }

#define EXPECT_EQ(first, second)                                                                         \
    if (!((first) == (second)))                                                                          \
    {                                                                                                    \
        std::cout << RED << "\"" << first << " == " << second << "\" Failed at " << __FILE__ << " Line " \
                  << std::to_string(__LINE__) << "!" << CLEAR << std::endl;                              \
        throw 0;                                                                                         \
    }

#define EXPECT_NE(first, second)                                                                         \
    if ((first) == (second))                                                                             \
    {                                                                                                    \
        std::cout << RED << "\"" << first << " != " << second << "\" Failed at " << __FILE__ << " Line " \
                  << std::to_string(__LINE__) << "!" << CLEAR << std::endl;                              \
        throw 0;                                                                                         \
    }

#define EXPECT_GT(first, second)                                                                        \
    if ((first) <= (second))                                                                            \
    {                                                                                                   \
        std::cout << RED << "\"" << first << " > " << second << "\" Failed at " << __FILE__ << " Line " \
                  << std::to_string(__LINE__) << "!" << CLEAR << std::endl;                             \
        throw 0;                                                                                        \
    }

#define EXPECT_NEAR(first, second, epsilon)                                                                \
    if (std::abs(first - second) > epsilon)                                                                \
    {                                                                                                      \
        std::cout << RED << "\"" << first << " near " << second << "\" Failed at " << __FILE__ << " Line " \
                  << std::to_string(__LINE__) << "!" << CLEAR << std::endl;                                \
        throw 0;                                                                                           \
    }

#define EXPECT_THROW(expr)                                                                                       \
    try                                                                                                          \
    {                                                                                                            \
        expr;                                                                                                    \
        std::cout << RED << "\"" << #expr << "\" Failed at " << __FILE__ << " Line " << std::to_string(__LINE__) \
                  << "!" << CLEAR << std::endl;                                                                  \
        throw 0;                                                                                                 \
    }                                                                                                            \
    catch (...)                                                                                                  \
    {                                                                                                            \
    }

#define EXPECT_NO_THROW(expr)                                                                                    \
    try                                                                                                          \
    {                                                                                                            \
        expr;                                                                                                    \
    }                                                                                                            \
    catch (...)                                                                                                  \
    {                                                                                                            \
        std::cout << RED << "\"" << #expr << "\" Failed at " << __FILE__ << " Line " << std::to_string(__LINE__) \
                  << "!" << CLEAR << std::endl;                                                                  \
        throw 0;                                                                                                 \
    }

#define TEST(group, name)                                                                                           \
    void group##_##name##Test_();                                                                                   \
    void group##_##name##Test()                                                                                     \
    {                                                                                                               \
        std::cout << #group << "::" << #name << " Test Started..." << std::endl;                                    \
        try                                                                                                         \
        {                                                                                                           \
            group##_##name##Test_();                                                                                \
            std::cout << GREEN << #group << "::" << #name << " Test succeeded!" << CLEAR << std::endl << SEPARATOR; \
        }                                                                                                           \
        catch (const std::exception &e)                                                                             \
        {                                                                                                           \
            Code = 1;                                                                                               \
            std::cout << RED << "Exception: " << e.what() << std::endl;                                             \
            std::cout << RED << #group << "::" << #name << " Test Failed!" << CLEAR << std::endl << SEPARATOR;      \
        }                                                                                                           \
    }                                                                                                               \
    static bool group##_##name##_registered = [] {                                                                  \
        TestList.push_back(group##_##name##Test);                                                                   \
        return true;                                                                                                \
    }();                                                                                                            \
    void group##_##name##Test_()

using namespace Neyson;

int Code = 0;

std::vector<std::function<void()>> TestList = {};

String randstr(size_t limit = 16)
{
    std::string data(1 + (rand() % (limit)), 0);
    const static std::string all = "0123456789_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for (auto &c : data) c = all[rand() % all.size()];
    return data;
}

Value _randvar(size_t &left)
{
    auto type = Type(rand() % (int(Type::Object) + 1));

    if (type == Type::Null)
        return Value();
    else if (type == Type::Integer)
        return ssize_t(rand()) + ssize_t(rand()) - RAND_MAX;
    else if (type == Type::Real)
        return (Real(rand()) / RAND_MAX + 1) / 2;
    else if (type == Type::String)
    {
        String value(rand() % 101, 0);
        for (auto &c : value) c = rand() % 128;
        return value;
    }
    else if (type == Type::Array)
    {
        auto size = std::min(left, size_t(rand() % (left + 1)));
        Array array(size);
        left -= size;
        for (auto &value : array) value = _randvar(left);
        return array;
    }
    else if (type == Type::Object)
    {
        auto size = std::min(left, size_t(rand() % (left + 1)));
        Object object(size);
        left -= size;

        for (size_t i = 0; i < size; ++i)
        {
            String value(rand() % 101, 0);
            for (auto &c : value) c = rand() % 128;
            object.insert({value, _randvar(left)});
        }
        return object;
    }

    return Value();
}

Value randvar(size_t count = 100) { return _randvar(count); }

XML::Node _randnode(size_t &count)
{
    using XML::Node;

    std::vector<Node::Type> types = {Node::Element, Node::Comment,  //
                                     Node::Declaration, Node::DocType, Node::ProcInfo};

    Node node(types[rand() % types.size()]);

    if (node == Node::Element)
    {
        node.name(randstr());
        node.value(randstr());

        size_t attr_count = rand() % 6;
        for (size_t i = 0; i < attr_count; ++i)  //
            node[randstr()] = randstr();

        size_t nodes_count = rand() % (count + 1);
        count -= nodes_count;
        for (size_t i = 0; i < nodes_count; ++i)  //
            node.add(_randnode(count));
    }
    else if (node == Node::Comment)
    {
        node.value(randstr());
    }
    else if (node == Node::CData)
    {
        node.value(randstr());
    }
    else if (node == Node::Declaration)
    {
        node["version"] = randstr();
        node["encoding"] = randstr();
        node["standalone"] = randstr();
    }
    else if (node == Node::DocType)
    {
        node.value(randstr());
    }
    else if (node == Node::ProcInfo)
    {
        node.name(randstr());
        node.value(randstr());
    }

    return node;
}

XML::Nodes randnode(size_t count = 100)
{
    XML::Nodes nodes((rand() % count) + 1);
    count -= nodes.size();
    for (auto &node : nodes) node = _randnode(count);
    return nodes;
}
