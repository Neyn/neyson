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

#include "xml.h"

#include <fstream>
#include <iterator>
#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_print.hpp>
#include <sstream>
#include <unordered_map>

namespace Neyson
{
namespace XML
{
bool readAll(const std::string &path, std::string &content)
{
    auto file = fopen(path.c_str(), "r");
    if (file == NULL) return false;

    fseek(file, 0, SEEK_END);
    content.resize(ftell(file));
    rewind(file);
    ::fread(&content[0], 1, content.size(), file);

    fclose(file);
    return true;
}

struct Reader
{
    using XDoc = rapidxml::xml_document<String::value_type>;
    using XNode = rapidxml::xml_node<String::value_type>;
    using XAttr = rapidxml::xml_attribute<String::value_type>;
    using XError = rapidxml::parse_error;
    using XType = rapidxml::node_type;

    static Node::Type convert(XType xtype)
    {
        if (xtype == XType::node_element) return Node::Element;
        if (xtype == XType::node_cdata) return Node::CData;
        if (xtype == XType::node_comment) return Node::Comment;
        if (xtype == XType::node_declaration) return Node::Declaration;
        if (xtype == XType::node_doctype) return Node::DocType;
        if (xtype == XType::node_pi) return Node::ProcInfo;
        NEYSON_ASSERT(false, "Internal error!");
    }

    static std::pair<String, String> convert(XAttr *xattr)
    {
        String name(xattr->name(), xattr->name_size());
        String value(xattr->value(), xattr->value_size());
        return {name, value};
    }

    static Node convert(XNode *xnode)
    {
        Node node(convert(xnode->type()));
        node._name = String(xnode->name(), xnode->name_size());
        node._value = String(xnode->value(), xnode->value_size());

        auto it = node._attribs.begin();
        for (auto a = xnode->first_attribute(); a; a = a->next_attribute())  //
            it = node._attribs.emplace_hint(it, convert(a));

        for (auto n = xnode->first_node(); n; n = n->next_sibling())  //
            node._nodes.push_back(convert(n));

        return node;
    }

    static Result read(Nodes &nodes, const char *input, Parse mode)
    {
        XDoc doc;

        try
        {
            using namespace rapidxml;
            auto ptr = (char *)input;
            const int flags = parse_fastest, space = parse_trim_whitespace;
            const int full = parse_declaration_node | parse_comment_nodes | parse_doctype_node | parse_pi_nodes;

            if (mode == Parse::ElementsTrimmed)
                doc.parse<flags | space>(ptr);
            else if (mode == Parse::FullTrimmed)
                doc.parse<flags | full | space>(ptr);
            else if (mode == Parse::Elements)
                doc.parse<flags>(ptr);
            else if (mode == Parse::Full)
                doc.parse<flags | full>(ptr);
        }
        catch (const XError &error)
        {
            auto ptr = error.where<std::string::value_type>();
            return Result(Error::ParseError, ptr - input, error.what());
        }

        nodes.clear();
        for (auto n = doc.first_node(); n; n = n->next_sibling())  //
            nodes.push_back(convert(n));

        return Result();
    }

    static Result fread(Nodes &nodes, const std::string &path, Parse mode)
    {
        std::string content;
        if (!readAll(path, content))
        {
            Result result(Error::FileIOError);
            result.message = "Failed to open file for reading";
            return result;
        }
        return read(nodes, content.data(), mode);
    }
};

struct Writer
{
    using XDoc = rapidxml::xml_document<String::value_type>;
    using XNode = rapidxml::xml_node<String::value_type>;
    using XAttr = rapidxml::xml_attribute<String::value_type>;
    using XType = rapidxml::node_type;

    XDoc document;

    static XType convert(Node::Type type)
    {
        if (type == Node::Element) return XType::node_element;
        if (type == Node::CData) return XType::node_cdata;
        if (type == Node::Comment) return XType::node_comment;
        if (type == Node::Declaration) return XType::node_declaration;
        if (type == Node::DocType) return XType::node_doctype;
        if (type == Node::ProcInfo) return XType::node_pi;
        NEYSON_ASSERT(false, "Unrecognized node type!");
    }

    XAttr *convert(const std::pair<const String, String> &attr)
    {
        auto S = attr.first.data(), V = attr.second.data();
        return document.allocate_attribute(S, V, attr.first.size(), attr.second.size());
    }

    XNode *convert(const Node &node)
    {
        XNode *xnode;
        auto N = node._name.data(), V = node._value.data();
        auto SN = node._name.size(), VN = node._value.size();

        if (node == Node::Element && !node.empty())
        {
            xnode = document.allocate_node(convert(node._type), N, 0, SN, 0);
            xnode->append_node(document.allocate_node(XType::node_data, 0, V, 0, VN));
        }
        else
            xnode = document.allocate_node(convert(node._type), N, V, SN, VN);

        for (const auto &N : node._nodes) xnode->append_node(convert(N));
        for (const auto &A : node._attribs) xnode->append_attribute(convert(A));
        return xnode;
    }

    Result write(const Nodes &nodes, std::ostream &stream, Mode mode)
    {
        std::ostream_iterator<std::string::value_type> iter(stream);
        rapidxml::print(iter, document, mode == Mode::Compact ? rapidxml::print_no_indenting : 0);
        return Result();
    }

    Result write(const Nodes &nodes, std::string &output, Mode mode)
    {
        for (const auto &node : nodes)  //
            document.append_node(convert(node));

        std::stringstream stream;
        auto result = write(nodes, stream, mode);
        output = stream.str();
        return result;
    }

    Result fwrite(const Nodes &nodes, const std::string &path, Mode mode)
    {
        for (const auto &node : nodes)  //
            document.append_node(convert(node));

        std::ofstream stream(path);
        if (!stream.is_open()) return Result(Error::FileIOError);
        return write(nodes, stream, mode);
    }
};

Result sread(Nodes &nodes, const char *input, Parse mode) { return Reader::read(nodes, input, mode); }

Result read(Nodes &nodes, const std::string &input, Parse mode) { return Reader::read(nodes, input.data(), mode); }

Result fread(Nodes &nodes, const std::string &path, Parse mode) { return Reader::fread(nodes, path, mode); }

Result write(const Nodes &nodes, std::string &output, Mode mode) { return Writer().write(nodes, output, mode); }

Result write(const Nodes &nodes, std::ostream &stream, Mode mode) { return Writer().write(nodes, stream, mode); }

Result fwrite(const Nodes &nodes, std::string &path, Mode mode) { return Writer().fwrite(nodes, path, mode); }

std::ostream &operator<<(std::ostream &os, Error error)
{
    if (error == Error::None)
        return os << "Error::None";
    else if (error == Error::FileIOError)
        return os << "Error::FileIOError";
    else if (error == Error::ParseError)
        return os << "Error::ParseError";
    return os << "Error::Unknown";
}

std::ostream &operator<<(std::ostream &os, const Result &result)
{
    return os << "Result(" << result.error << ", " << result.index << ", " << result.message << ")";
}

std::ostream &operator<<(std::ostream &os, const Nodes &nodes)
{
    // TODO directly from stream doesn't work
    std::string data;
    write(nodes, data);
    return os << data;
}

std::ostream &operator<<(std::ostream &os, const Node &node) { return os << Nodes{node}; }

std::ostream &operator<<(std::ostream &os, Parse parse)
{
    if (parse == Parse::ElementsTrimmed)
        return os << "Parse::ElementsTrimmed";
    else if (parse == Parse::FullTrimmed)
        return os << "Parse::FullTrimmed";
    else if (parse == Parse::Elements)
        return os << "Parse::Elements";
    else if (parse == Parse::Full)
        return os << "Parse::Full";
    return os << "Parse::Unknown";
}

std::ostream &operator<<(std::ostream &os, Mode mode)
{
    if (mode == Mode::Compact)
        return os << "Mode::Compact";
    else if (mode == Mode::Readable)
        return os << "Mode::Readable";
    return os << "Mode::Unknown";
}

void swap(Node &first, Node &second)
{
    std::swap(first._type, second._type);
    std::swap(first._name, second._name);
    std::swap(first._value, second._value);
    std::swap(first._attribs, second._attribs);
    std::swap(first._nodes, second._nodes);
}

}  // namespace XML
}  // namespace Neyson
