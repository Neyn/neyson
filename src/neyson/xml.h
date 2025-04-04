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

#include <neyson/value.h>

#include <map>

namespace Neyson
{
namespace XML
{
/// @brief Enumeration of possible errors that can occur while working with XML.
///
/// @details These errors indicate why an XML operation (such as reading or writing)
///          might have failed.
enum class Error
{
    /// @brief No error occurred.
    None,

    /// @brief The file could not be opened, read, or written.
    FileIOError,

    /// @brief A general parsing error occurred (malformed XML structure).
    ParseError,
};

/// @brief Result structure for XML operations.
///
/// @details This struct holds the status of an XML operation, including an error
///          code, the position/index of the error if any, and an optional message.
struct Result
{
    /// @brief The error code indicating success or type of failure.
    Error error;

    /// @brief The index (e.g., character or byte position) where an error was detected.
    size_t index;

    /// @brief Additional message describing the error or context.
    std::string message;

    /// @brief Constructs a Result object.
    ///
    /// @param error   The error code. Defaults to Error::None (no error).
    /// @param index   The position where an error was encountered (if any). Defaults to size_t(-1).
    /// @param message An optional string describing the error in more detail.
    inline Result(Error error = Error::None, size_t index = size_t(-1), const String &message = {})
        : error(error), index(index), message(message)
    {
    }

    /// @brief Boolean conversion operator indicating success or failure.
    ///
    /// @return True if @p error is Error::None; otherwise false.
    inline operator bool() const { return error == Error::None; }
};

/// @brief Forward declaration of Node class representing an XML node.
struct Node;

/// @brief Forward declaration of Writer class for writing XML documents.
struct Writer;

/// @brief Forward declaration of Reader class for reading XML documents.
struct Reader;

/// @brief Alias representing a collection of Node objects.
using Nodes = std::vector<Node>;

/// @brief A class representing an XML node.
///
/// @details Each Node may be one of several types (Element, CData, Comment, Declaration,
///          DocType, ProcInfo) and may contain child nodes, attributes, or textual data.
class Node
{
    friend class Writer;
    friend class Reader;

public:
    /// @brief Enumeration of node types.
    enum Type
    {
        /// @brief An element node, which may have a tag name, value, attributes, and child nodes.
        Element,

        /// @brief A CDATA section node, storing unescaped character data.
        CData,

        /// @brief A comment node, storing comment text.
        Comment,

        /// @brief An XML declaration node, possibly holding version, encoding, and standalone attributes.
        Declaration,

        /// @brief A DOCTYPE node, storing the document type definition or reference.
        DocType,

        /// @brief A processing instruction node, typically containing a target and instructions.
        ProcInfo,
    };

    /// @brief A map of attribute key-value pairs for this Node.
    using Attribs = std::unordered_map<String, String>;

private:
    Type _type;        ///< The type of this node.
    String _name;      ///< The node name (e.g., element name, target, etc.).
    String _value;     ///< The node value (e.g., element text, comment text, etc.).
    Attribs _attribs;  ///< A collection of attributes for element or declaration nodes.
    Nodes _nodes;      ///< Child nodes if this node is an Element.

public:
    /// @brief Constructs a Node with the specified type.
    ///
    /// @param type The type of the node. Defaults to Type::Element.
    inline explicit Node(Type type = Type::Element) : _type(type) {}

    /// @brief Copy constructor.
    ///
    /// @param other The Node to copy from.
    Node(const Node &other) = default;

    /// @brief Move constructor.
    ///
    /// @param other The Node to move from.
    Node(Node &&other) = default;

    /// @brief Copy assignment operator.
    ///
    /// @param other The Node to copy from.
    /// @return A reference to this Node.
    Node &operator=(const Node &other) = default;

    /// @brief Move assignment operator.
    ///
    /// @param other The Node to move from.
    /// @return A reference to this Node.
    Node &operator=(Node &&other) = default;

    /// @brief Resets the Node to its default state (Type::Element with empty data).
    ///
    /// @details This function clears the node name, value, attributes,
    ///          and child nodes, and sets the node type to Element.
    inline void reset()
    {
        _type = Type::Element;
        _name.clear();
        _value.clear();
        _attribs.clear();
        _nodes.clear();
    }

    /// @brief Gets the type of this Node.
    ///
    /// @return The current Type of this Node.
    inline Type type() const { return _type; }

    /// @brief Checks if this Node is of the specified Type.
    ///
    /// @param type The Type to compare against.
    /// @return True if they match, otherwise false.
    inline bool operator==(Type type) const { return _type == type; }

    /// @brief Checks if this Node is not of the specified Type.
    ///
    /// @param type The Type to compare against.
    /// @return True if they differ, otherwise false.
    inline bool operator!=(Type type) const { return _type != type; }

    /// @brief Checks if this Node is equal to another Node.
    ///
    /// @details Two Nodes are considered equal if they have the same type, name, value,
    ///          attributes, and child nodes.
    /// @param other The Node to compare with.
    /// @return True if they are equal, otherwise false.
    inline bool operator==(const Node &other) const
    {
        return _type == other._type && _name == other._name && _value == other._value && _attribs == other._attribs &&
               _nodes == other._nodes;
    }

    /// @brief Swaps the contents of two Nodes.
    ///
    /// @details This function exchanges the type, name, value, attributes, and child
    ///          nodes of @p first and @p second, providing a no-throw swap mechanism.
    /// @param first  The first Node.
    /// @param second The second Node.
    friend void swap(Node &first, Node &second);

    ////////////////////////////////////////

    /// @brief Access the child node at the specified index (non-const).
    ///
    /// @details This operator assumes that the current node can have child nodes
    ///          (i.e., the Node is typically of Type::Element). If @p index
    ///          is out of range, the behavior is undefined or may trigger an assertion.
    ///
    /// @param index The zero-based index of the desired child node.
    /// @return A reference to the child node at the given index.
    inline Node &operator[](size_t index) { return nodes()[index]; }

    /// @brief Access the child node at the specified index (const).
    ///
    /// @details This operator returns a const reference to the child node at @p index.
    ///          If @p index is out of range, the behavior is undefined or may trigger
    ///          an assertion.
    ///
    /// @param index The zero-based index of the desired child node.
    /// @return A const reference to the child node at the given index.
    inline const Node &operator[](size_t index) const { return nodes()[index]; }

    /// @brief Access or create an attribute with the specified key (non-const).
    ///
    /// @details If the attribute @p key exists, returns a reference to its value.
    ///          Otherwise, creates a new attribute with key @p key and a default-constructed
    ///          value, then returns a reference to that value.
    ///
    /// @param key The name of the attribute to access.
    /// @return A reference to the attribute value (which can be modified).
    inline String &operator[](const String &key) { return attribs()[key]; }

    /// @brief Access the attribute with the specified key (const).
    ///
    /// @details If the attribute @p key exists, returns a const reference to its value.
    ///          If the attribute does not exist, an assertion triggers a runtime exception.
    ///
    /// @param key The name of the attribute to access.
    /// @return A const reference to the attribute value.
    inline const String &operator[](const String &key) const
    {
        const Attribs &attribs = this->attribs();
        auto it = attribs.find(key);
        NEYSON_ASSERT(it != attribs.end(), "No attribute with name \"" + key + "\" found!");
        return it->second;
    }

    /// @brief Returns a const iterator pointing to the first child node.
    ///
    /// @details This function assumes the Node can hold child nodes (Type::Element).
    ///          If there are no child nodes, @p begin() equals @p end().
    ///
    /// @return A const iterator to the beginning of the child node list.
    inline Nodes::const_iterator begin() const { return nodes().begin(); }

    /// @brief Returns an iterator pointing to the first child node.
    ///
    /// @details This function allows modification of child nodes. If there are no
    ///          child nodes, @p begin() equals @p end().
    ///
    /// @return An iterator to the beginning of the child node list.
    inline Nodes::iterator begin() { return nodes().begin(); }

    /// @brief Returns a const iterator pointing to the end of the child node list.
    ///
    /// @details This marks the past-the-end position; it does not refer to a valid child node.
    ///
    /// @return A const iterator to the end of the child node list.
    inline Nodes::const_iterator end() const { return nodes().end(); }

    /// @brief Returns an iterator pointing to the end of the child node list.
    ///
    /// @details This marks the past-the-end position; it does not refer to a valid child node.
    ///
    /// @return An iterator to the end of the child node list.
    inline Nodes::iterator end() { return nodes().end(); }

    /// @brief Adds a copy of the given node to the child node list.
    ///
    /// @details This function pushes @p node onto the list of child nodes.
    ///          It is assumed the current Node can hold child nodes (Type::Element).
    ///
    /// @param node The Node to be copied and appended.
    /// @return A reference to the current Node (for chaining).
    inline Node &add(const Node &node)
    {
        nodes().push_back(node);
        return *this;
    }

    /// @brief Adds a moved node to the child node list.
    ///
    /// @details This function pushes @p node (via std::move) onto the list of child nodes.
    ///          It is assumed the current Node can hold child nodes (Type::Element).
    ///
    /// @param node The Node to be moved and appended.
    /// @return A reference to the current Node (for chaining).
    inline Node &add(Node &&node)
    {
        nodes().push_back(std::move(node));
        return *this;
    }

    /// @brief Adds or updates an attribute with the specified key-value pair (copy).
    ///
    /// @details If the attribute @p key already exists, it is overwritten. Otherwise,
    ///          a new attribute is created.
    ///
    /// @param key   The attribute name.
    /// @param value The attribute value.
    /// @return A reference to the current Node (for chaining).
    inline Node &add(const String &key, const String &value)
    {
        attribs().emplace(key, value);
        return *this;
    }

    /// @brief Adds or updates an attribute with the specified key-value pair (move).
    ///
    /// @details If the attribute @p key already exists, it is overwritten. Otherwise,
    ///          a new attribute is created using the moved string.
    ///
    /// @param key   The attribute name.
    /// @param value The attribute value (will be moved).
    /// @return A reference to the current Node (for chaining).
    inline Node &add(const String &key, String &&value)
    {
        attribs().emplace(key, std::move(value));
        return *this;
    }

    /// @brief Returns the number of child nodes in this Node.
    ///
    /// @return The size of the child node list.
    inline size_t size() const { return nodes().size(); }

    /// @brief Checks if this Node has any child nodes.
    ///
    /// @return True if there are no child nodes, otherwise false.
    inline bool empty() const { return nodes().empty(); }

    /// @brief Clears all child nodes from this Node.
    ///
    /// @details This operation does not remove attributes or alter the node type/name/value.
    ///          It only clears the child nodes vector.
    inline void clear() { nodes().clear(); }

    /// @brief Removes the child node at the specified index.
    ///
    /// @details This function erases the child node at position @p index.
    ///          If @p index is out of range, the behavior is undefined or may trigger an assertion.
    /// @param index The zero-based index of the child node to remove.
    inline void remove(size_t index)
    {
        Nodes &nodes = this->nodes();
        nodes.erase(nodes.begin() + index);
        // (void)0 is just a no-op
    }

    /// @brief Removes an attribute by its key.
    ///
    /// @details If the attribute @p key exists, it is erased from the internal attributes map.
    /// @param key The name of the attribute to remove.
    /// @return True if the attribute was removed, otherwise false.
    inline bool remove(const String &key) { return attribs().erase(key); }

    ////////////////////////////////////////

    /// @brief Checks if this Node is an Element type.
    ///
    /// @return True if the node type is Type::Element, otherwise false.
    inline bool iselem() const { return _type == Type::Element; }

    /// @brief Checks if this Node is a CData section type.
    ///
    /// @return True if the node type is Type::CData, otherwise false.
    inline bool iscdata() const { return _type == Type::CData; }

    /// @brief Checks if this Node is a Comment type.
    ///
    /// @return True if the node type is Type::Comment, otherwise false.
    inline bool iscomment() const { return _type == Type::Comment; }

    /// @brief Checks if this Node is a Declaration type.
    ///
    /// @return True if the node type is Type::Declaration, otherwise false.
    inline bool isdecl() const { return _type == Type::Declaration; }

    /// @brief Checks if this Node is a DocType type.
    ///
    /// @return True if the node type is Type::DocType, otherwise false.
    inline bool isdoctype() const { return _type == Type::DocType; }

    /// @brief Checks if this Node is a Processing Instruction type.
    ///
    /// @return True if the node type is Type::ProcInfo, otherwise false.
    inline bool ispi() const { return _type == Type::ProcInfo; }

    ////////////////////////////////////////

    /// @brief Gets the name of the Node (const).
    ///
    /// @details This function is only valid if the Node type is either Element or ProcInfo.
    ///          If the Node's type is neither, an assertion triggers a runtime exception.
    ///
    /// @return A const reference to the Node's name.
    inline const String &name() const
    {
        NEYSON_ASSERT(_type == Type::Element || _type == Type::ProcInfo,
                      "Only Element and ProcInfo nodes have name property!");
        return _name;
    }

    /// @brief Gets the name of the Node (non-const).
    ///
    /// @details This function is only valid if the Node type is either Element or ProcInfo.
    ///          If the Node's type is neither, an assertion triggers a runtime exception.
    ///
    /// @return A reference to the Node's name, allowing modification.
    inline String &name()
    {
        NEYSON_ASSERT(_type == Type::Element || _type == Type::ProcInfo,
                      "Only Element and ProcInfo nodes have name property!");
        return _name;
    }

    /// @brief Sets the name of the Node using a copy, then returns a reference to it.
    ///
    /// @details This function is only valid if the Node type is either Element or ProcInfo.
    ///          If the Node's type is neither, an assertion triggers a runtime exception.
    ///
    /// @param name The new name to assign to this Node.
    /// @return A reference to the Node's name after assignment.
    inline String &name(const String &name) { return (this->name() = name, _name); }

    /// @brief Sets the name of the Node using a move operation, then returns a reference to it.
    ///
    /// @details This function is only valid if the Node type is either Element or ProcInfo.
    ///          If the Node's type is neither, an assertion triggers a runtime exception.
    ///
    /// @param name The new name to assign to this Node (will be moved).
    /// @return A reference to the Node's name after assignment.
    inline String &name(String &&name) { return (this->name() = std::move(name), _name); }

    /// @brief Gets the value of the Node (const).
    ///
    /// @details This function is not valid for Declaration nodes. If the Node's type
    ///          is Type::Declaration, an assertion triggers a runtime exception.
    ///
    /// @return A const reference to the Node's value.
    inline const String &value() const
    {
        NEYSON_ASSERT(_type != Type::Declaration, "Declaration doesn't have a value property!");
        return _value;
    }

    /// @brief Gets the value of the Node (non-const).
    ///
    /// @details This function is not valid for Declaration nodes. If the Node's type
    ///          is Type::Declaration, an assertion triggers a runtime exception.
    ///
    /// @return A reference to the Node's value, allowing modification.
    inline String &value()
    {
        NEYSON_ASSERT(_type != Type::Declaration, "Declaration doesn't have a value property!");
        return _value;
    }

    /// @brief Sets the value of the Node using a copy, then returns a reference to it.
    ///
    /// @details This function is not valid for Declaration nodes. If the Node's type
    ///          is Type::Declaration, an assertion triggers a runtime exception.
    ///
    /// @param value The new value to assign to this Node.
    /// @return A reference to the Node's value after assignment.
    inline String &value(const String &value) { return (this->value() = value, _value); }

    /// @brief Sets the value of the Node using a move operation, then returns a reference to it.
    ///
    /// @details This function is not valid for Declaration nodes. If the Node's type
    ///          is Type::Declaration, an assertion triggers a runtime exception.
    ///
    /// @param value The new value to assign to this Node (will be moved).
    /// @return A reference to the Node's value after assignment.
    inline String &value(String &&value) { return (this->value() = std::move(value), _value); }

    /// @brief Gets the attribute map (non-const).
    ///
    /// @details This function is only valid if the Node type is Element or Declaration.
    ///          Otherwise, an assertion triggers a runtime exception.
    /// @return A reference to the map of attribute key-value pairs.
    Attribs &attribs()
    {
        NEYSON_ASSERT(_type == Type::Element || _type == Type::Declaration, "Only element nodes can have attributes!");
        return _attribs;
    }

    /// @brief Gets the attribute map (const).
    ///
    /// @details This function is only valid if the Node type is Element or Declaration.
    ///          Otherwise, an assertion triggers a runtime exception.
    /// @return A const reference to the map of attribute key-value pairs.
    inline const Attribs &attribs() const
    {
        NEYSON_ASSERT(_type == Type::Element || _type == Type::Declaration, "Only element nodes can have attributes!");
        return _attribs;
    }

    /// @brief Sets the attribute map by copy, then returns a reference to it.
    ///
    /// @details This function is only valid if the Node type is Element or Declaration.
    ///          Otherwise, an assertion triggers a runtime exception.
    ///
    /// @param attribs The new attribute map to copy.
    /// @return A reference to the Node's attribute map after assignment.
    inline Attribs &attribs(const Attribs &attribs) { return (this->attribs() = attribs, _attribs); }

    /// @brief Sets the attribute map by move, then returns a reference to it.
    ///
    /// @details This function is only valid if the Node type is Element or Declaration.
    ///          Otherwise, an assertion triggers a runtime exception.
    ///
    /// @param attribs The new attribute map to move.
    /// @return A reference to the Node's attribute map after assignment.
    inline Attribs &attribs(Attribs &&attribs) { return (this->attribs() = std::move(attribs), _attribs); }

    /// @brief Gets the collection of child nodes (non-const).
    ///
    /// @details This function is only valid if the Node type is Element.
    ///          Otherwise, an assertion triggers a runtime exception.
    /// @return A reference to the vector of child nodes.
    inline Nodes &nodes()
    {
        NEYSON_ASSERT(_type == Type::Element, "Only element nodes can have children!");
        return _nodes;
    }

    /// @brief Gets the collection of child nodes (const).
    ///
    /// @details This function is only valid if the Node type is Element.
    ///          Otherwise, an assertion triggers a runtime exception.
    /// @return A const reference to the vector of child nodes.
    inline const Nodes &nodes() const
    {
        NEYSON_ASSERT(_type == Type::Element, "Only element nodes can have children!");
        return _nodes;
    }

    /// @brief Sets the collection of child nodes by copy, then returns a reference to it.
    ///
    /// @details This function is only valid if the Node type is Element.
    ///          Otherwise, an assertion triggers a runtime exception.
    ///
    /// @param nodes The new list of child nodes to copy.
    /// @return A reference to the Node's children after assignment.
    inline Nodes &nodes(const Nodes &nodes) { return (this->nodes() = nodes, _nodes); }

    /// @brief Sets the collection of child nodes by move, then returns a reference to it.
    ///
    /// @details This function is only valid if the Node type is Element.
    ///          Otherwise, an assertion triggers a runtime exception.
    ///
    /// @param nodes The new list of child nodes to move.
    /// @return A reference to the Node's children after assignment.
    inline Nodes &nodes(Nodes &&nodes) { return (this->nodes() = std::move(nodes), _nodes); }
};

/// @brief Parsing modes for XML input.
///
/// @details These modes control how the XML input is parsed and trimmed.
enum class Parse
{
    /// @brief Read only element nodes, trimming whitespace within elements.
    ElementsTrimmed,

    /// @brief Read the entire XML document, trimming all extraneous whitespace.
    FullTrimmed,

    /// @brief Read only element nodes without trimming whitespace.
    Elements,

    /// @brief Read the entire XML document without trimming whitespace.
    Full,
};

/// @brief Output modes for XML serialization.
///
/// @details These modes control the formatting of XML output when writing.
enum class Mode
{
    /// @brief Compact output with minimal whitespace.
    Compact,

    /// @brief Readable output with indentation and newlines.
    Readable,
};

/// @brief Parses an XML document from a C-string into a list of Nodes.
///
/// @details Depending on @p mode, whitespace and certain node types may be trimmed or discarded.
/// @param nodes Reference to a Nodes container where parsed nodes will be stored.
/// @param input A null-terminated C-string containing XML data.
/// @param mode  The parsing mode indicating how whitespace and nodes are handled.
/// @return A Result indicating success or error details if parsing fails.
Result sread(Nodes &nodes, const char *input, Parse mode = Parse::ElementsTrimmed);

/// @brief Parses an XML document from an std::string into a list of Nodes.
///
/// @details Depending on @p mode, whitespace and certain node types may be trimmed or discarded.
/// @param nodes  Reference to a Nodes container where parsed nodes will be stored.
/// @param input  A std::string containing XML data.
/// @param mode   The parsing mode indicating how whitespace and nodes are handled.
/// @return A Result indicating success or error details if parsing fails.
Result read(Nodes &nodes, const std::string &input, Parse mode = Parse::ElementsTrimmed);

/// @brief Reads and parses an XML document from a file into a list of Nodes.
///
/// @details Opens the file at @p path, reads its contents, and parses them according to @p mode.
/// @param nodes  Reference to a Nodes container where parsed nodes will be stored.
/// @param path   The path of the file containing the XML data.
/// @param mode   The parsing mode indicating how whitespace and nodes are handled.
/// @return A Result indicating success or error details if reading/parsing fails.
Result fread(Nodes &nodes, const std::string &path, Parse mode = Parse::ElementsTrimmed);

///////////////////////////////////////////////////////////////////////////////
// Writing Functions

/// @brief Writes a list of Nodes to a string in XML format.
///
/// @details The @p mode parameter controls whether the output is compact or readable
///          with indentation and newlines.
/// @param nodes  The list of Nodes to serialize.
/// @param output A reference to the string where the XML data will be written.
/// @param mode   The output mode (compact or readable).
/// @return A Result indicating success or error details if writing fails.
Result write(const Nodes &nodes, std::string &output, Mode mode = Mode::Readable);

/// @brief Writes a list of Nodes to a given output stream in XML format.
///
/// @details The @p mode parameter controls whether the output is compact or readable
///          with indentation and newlines.
/// @param nodes  The list of Nodes to serialize.
/// @param stream The output stream to write to.
/// @param mode   The output mode (compact or readable).
/// @return A Result indicating success or error details if writing fails.
Result write(const Nodes &nodes, std::ostream &stream, Mode mode = Mode::Readable);

/// @brief Writes a list of Nodes to a file at the specified path in XML format.
///
/// @details Opens or creates the file at @p path and writes the serialized XML data.
///          The @p mode parameter controls whether the output is compact or readable.
/// @param nodes The list of Nodes to serialize.
/// @param path  The file path where the XML data will be written.
/// @param mode  The output mode (compact or readable).
/// @return A Result indicating success or error details if writing fails.
Result fwrite(const Nodes &nodes, std::string &path, Mode mode = Mode::Readable);

/// @brief Inserts a textual representation of an Error enum into the given output stream.
///
/// @details This operator typically displays the name of the error code (e.g., "None",
///          "FileIOError", "ParseError").
/// @param os    The output stream to write to.
/// @param error The Error enum value to display.
/// @return A reference to the modified output stream.
std::ostream &operator<<(std::ostream &os, Error error);

/// @brief Inserts a textual representation of a Result object into the given output stream.
///
/// @details This operator may display the error code, index, and any associated message.
/// @param os     The output stream to write to.
/// @param result The Result object to display.
/// @return A reference to the modified output stream.
std::ostream &operator<<(std::ostream &os, const Result &result);

/// @brief Inserts a textual representation of a single Node into the given output stream.
///
/// @details The exact formatting is implementation-specific; typically includes
///          node type, name, attributes, and value.
/// @param os   The output stream to write to.
/// @param node The Node to display.
/// @return A reference to the modified output stream.
std::ostream &operator<<(std::ostream &os, const Node &node);

/// @brief Inserts a textual representation of multiple Nodes into the given output stream.
///
/// @details The exact formatting is implementation-specific; typically prints each Node in turn.
/// @param os    The output stream to write to.
/// @param nodes The list of Nodes to display.
/// @return A reference to the modified output stream.
std::ostream &operator<<(std::ostream &os, const Nodes &nodes);

/// @brief Inserts a textual representation of a Parse enum into the given output stream.
///
/// @details This operator typically displays the name of the parse mode (e.g., "ElementsTrimmed").
/// @param os    The output stream to write to.
/// @param parse The Parse enum value to display.
/// @return A reference to the modified output stream.
std::ostream &operator<<(std::ostream &os, Parse parse);

/// @brief Inserts a textual representation of a Mode enum into the given output stream.
///
/// @details This operator typically displays either "Compact" or "Readable".
/// @param os   The output stream to write to.
/// @param mode The Mode enum value to display.
/// @return A reference to the modified output stream.
std::ostream &operator<<(std::ostream &os, Mode mode);
}  // namespace XML
}  // namespace Neyson
