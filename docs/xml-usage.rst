XML Usage Tutorial
==================

.. contents::
   :depth: 2
   :local:

Introduction
------------
The **Neyson::XML** namespace provides a high-level API for reading, parsing, and writing XML documents. The core data structure is the **Node**, which represents an XML node (element, comment, etc.) and can contain attributes, child nodes, and textual content. This tutorial covers the essential functionalities you need to build, parse, and serialize XML documents using this interface.

Key Components
--------------
- **Error**: Enum class listing possible error types (e.g., **None**, **FileIOError**, **ParseError**).
- **Result**: Struct combining an **Error** code with an index (position of error) and a message.
- **Node**: Class representing an XML node (element, comment, etc.) with attributes, child nodes, and textual content.
- **Nodes**: A type alias for `std::vector<Node>`—i.e., a list of **Node** objects.
- **Parse**: Enum class controlling how strictly to parse an XML document (trimmed vs. full).
- **Mode**: Enum class controlling how to format the output when writing XML.

Creating and Manipulating XML Nodes
-----------------------------------
The **Node** class encapsulates an XML node that can be of the following types:

- **Element**: A standard XML element that can have a name, attributes, and child nodes.
- **CData**: Represents a CDATA section.
- **Comment**: Represents an XML comment (e.g., `<!-- comment -->`).
- **Declaration**: The `<?xml ... ?>` declaration node typically found at the top of XML documents.
- **DocType**: A `<!DOCTYPE ...>` node.
- **ProcInfo**: A processing instruction node (similar to declaration but can appear anywhere).

Basic Construction
~~~~~~~~~~~~~~~~~~
.. code-block:: c++

    #include <neyson/neyson.h>
    using namespace Neyson;

    int main() {
        // Create a root element node
        XML::Node root(XML::Node::Element);
        root.name("root");  // Set element name to "root"

        // Add an attribute to the root
        root.add("version", "1.0");

        // Create a child element node and set its text
        XML::Node child(XML::Node::Element);
        child.name("child");
        child.value("Some text content");

        // Attach the child node to the root
        root.add(std::move(child));

        // Output to show structure
        std::cout << "Root node: " << root.name() << std::endl;
        return 0;
    }

Node Class Overview
~~~~~~~~~~~~~~~~~~~
Key member functions and operators:

- **name()**: Get or set the element name (for Element or ProcInfo nodes).
- **value()**: Get or set text content (not for Declaration nodes).
- **attribs()**: Access the attribute map. Valid for Element and Declaration nodes.
- **nodes()**: Access the child nodes (as a `std::vector<Node>`). Valid for Element nodes.
- **operator[](size_t index)**: Access a child node by index.
- **operator[](const String &key)**: Access an attribute by key.
- **add(...)**: Append child nodes or add attributes.
- **reset()**: Clear and reset the node to default (Element type).
- **size()**, **empty()**, **clear()**, **remove()**: Manipulate children or attributes.

Parsing XML
-----------
You can parse XML from three different sources:

1. **C-string** via `sread(Nodes &nodes, const char *input, Parse mode)`
2. **std::string** via `read(Nodes &nodes, const std::string &input, Parse mode)`
3. **File** via `fread(Nodes &nodes, const std::string &path, Parse mode)`

The parsing **mode** can be one of:

- **Parse::ElementsTrimmed**
  Only element nodes are read; whitespace is trimmed within elements.
- **Parse::FullTrimmed**
  Reads the entire XML document (including comments, CDATA, etc.), trimming all extraneous whitespace.
- **Parse::Elements**
  Reads only element nodes without trimming whitespace.
- **Parse::Full**
  Reads the entire XML document without trimming whitespace.

Example: Parsing from a String
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.. code-block:: c++

    #include <neyson/neyson.h>
    using namespace Neyson;

    int main() {
        // Suppose we have an XML string
        std::string xmlData = R"(
            <root version="1.0">
                <child>Hello World</child>
            </root>
        )";

        XML::Nodes nodeList;
        auto result = XML::read(nodeList, xmlData, XML::Parse::ElementsTrimmed);
        if (!result) {
            // Parsing failed
            std::cerr << "Error parsing XML: " << result.message
                      << " at index " << result.index << std::endl;
            return 1;
        }

        // nodeList now contains the top-level nodes of the parsed XML
        if (!nodeList.empty()) {
            const auto &root = nodeList[0];
            std::cout << "Root node name: " << root.name() << std::endl;
            std::cout << "Root version attribute: " << root["version"] << std::endl;
            // Child node
            const auto &child = root[0];
            std::cout << "Child content: " << child.value() << std::endl;
        }

        return 0;
    }

Example: Parsing from a File
~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.. code-block:: c++

    #include <neyson/neyson.h>
    using namespace Neyson;

    int main() {
        XML::Nodes xmlNodes;
        auto result = XML::fread(xmlNodes, "example.xml", XML::Parse::Full);
        if (!result) {
            std::cerr << "Failed to read file: " << result.message << std::endl;
            return 1;
        }

        // Process the XML nodes...
        return 0;
    }

Inspecting Parse Errors
~~~~~~~~~~~~~~~~~~~~~~~
All parse functions return an **XML::Result**, which contains:
- **error**: The **Error** code (e.g., **None**, **FileIOError**, **ParseError**).
- **index**: The character/byte position where parsing encountered an issue.
- **message**: Additional context for the error.

Example:

.. code-block:: c++

    auto res = XML::read(xmlNodes, malformedXmlString);
    if (!res) {
        std::cerr << "Parse error: " << res.error     // e.g., ParseError
                  << " at "         << res.index
                  << ", message: "  << res.message
                  << std::endl;
    }

Writing XML
-----------
After constructing or modifying XML **Nodes**, you can write them back to various destinations:

1. **std::string** via `write(const Nodes &nodes, std::string &output, Mode mode)`
2. **std::ostream** via `write(const Nodes &nodes, std::ostream &stream, Mode mode)`
3. **File** via `fwrite(const Nodes &nodes, const std::string &path, Mode mode)`

The **Mode** enum determines the formatting:

- **Mode::Compact**: Minimizes whitespace.
- **Mode::Readable**: Uses indentation and newlines for human readability.

Example: Writing to a String
~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.. code-block:: c++

    XML::Nodes nodeList;

    // Construct an example Node
    XML::Node root(XML::Node::Element);
    root.name("greeting");
    root.value("Hello, World!");

    nodeList.push_back(root);

    std::string output;
    auto result = XML::write(nodeList, output, XML::Mode::Readable);
    if (!result) {
        std::cerr << "Failed to write XML: " << result.message << std::endl;
    } else {
        std::cout << "XML output:\n" << output << std::endl;
    }

Example: Writing to a File
~~~~~~~~~~~~~~~~~~~~~~~~~~
.. code-block:: c++

    XML::Nodes nodeList;
    // Suppose we have populated nodeList

    auto result = XML::fwrite(nodeList, "output.xml", XML::Mode::Readable);
    if (!result) {
        std::cerr << "Failed to write to file: " << result.message << std::endl;
    } else {
        std::cout << "XML written to output.xml\n";
    }

Advanced Node Usage
-------------------
1. **Multiple Child Nodes**:

   .. code-block:: c++

       XML::Node root(XML::Node::Element);
       root.name("root");

       // Create child element1
       XML::Node elem1(XML::Node::Element);
       elem1.name("child1");
       elem1.value("Value 1");

       // Create child element2
       XML::Node elem2(XML::Node::Element);
       elem2.name("child2");
       elem2.add("attr", "value");
       elem2.value("Value 2");

       // Append them to root
       root.add(elem1).add(std::move(elem2));

2. **Comments, Declarations, and CData**:

   .. code-block:: c++

       // Declaration
       XML::Node decl(XML::Node::Declaration);
       // Declarations can have attributes, e.g., version, encoding
       decl.add("version", "1.0");
       decl.add("encoding", "UTF-8");

       // Comment
       XML::Node comment(XML::Node::Comment);
       comment.value("This is a comment");

       // CData
       XML::Node cdata(XML::Node::CData);
       cdata.value("Some <tag> content that won't be parsed as XML.");

       // Add these to top-level nodes
       XML::Nodes nodeList;
       nodeList.push_back(decl);
       nodeList.push_back(comment);
       nodeList.push_back(cdata);

       // ... Then write them out or manipulate further

3. **Attributes**:
   - Access or create an attribute: `root["key"] = "value";`
   - Retrieve an attribute (const): `auto val = root["key"];`

4. **Assertion and Type Checking**:
   - Attempting to call `name()` on a Comment node will trigger an assertion.
   - Similarly, `attribs()` on a CData node is invalid.

Error Reporting
---------------
All major read/write functions return an **XML::Result** struct, which indicates whether the operation succeeded (`result`) or failed (`!result`). You can inspect `result.error`, `result.index`, and `result.message` for debugging or logging.

Inspecting Result
~~~~~~~~~~~~~~~~~
.. code-block:: c++

    if (!result) {
        // For example, we can see if it's a FileIOError or ParseError
        switch (result.error) {
        case XML::Error::FileIOError:
            std::cerr << "File IO Error at index " << result.index << ": " << result.message << std::endl;
            break;
        case XML::Error::ParseError:
            std::cerr << "Parse Error at index " << result.index << ": " << result.message << std::endl;
            break;
        default:
            break;
        }
    }

Practical Example
-----------------
Below is an end-to-end example of building, writing, reading, and re-writing XML data.

.. code-block:: c++

    #include <neyson/neyson.h>
    #include <iostream>

    using namespace Neyson;

    int main() {
        // 1. Construct some XML nodes
        XML::Nodes doc;

        // Declaration
        XML::Node decl(XML::Node::Declaration);
        decl.add("version", "1.0");
        decl.add("encoding", "UTF-8");
        doc.push_back(decl);

        // Root element
        XML::Node root(XML::Node::Element);
        root.name("root");
        root.add("attrib", "example");

        // Child element
        XML::Node child(XML::Node::Element);
        child.name("child");
        child.value("Sample content");

        // Add child to root
        root.add(std::move(child));
        doc.push_back(root);

        // 2. Write the XML to a string
        std::string output;
        auto writeRes = XML::write(doc, output, XML::Mode::Readable);
        if (!writeRes) {
            std::cerr << "Write error: " << writeRes.message << std::endl;
            return 1;
        }

        std::cout << "XML Document:\n" << output << std::endl;

        // 3. Parse it back to verify
        XML::Nodes parsed;
        auto readRes = XML::read(parsed, output, XML::Parse::Full);
        if (!readRes) {
            std::cerr << "Read error: " << readRes.message << " at " << readRes.index << std::endl;
            return 1;
        }

        // 4. Inspect the parsed structure
        std::cout << "Parsed " << parsed.size() << " top-level nodes.\n";
        if (!parsed.empty()) {
            // Declaration is parsed[0], root is parsed[1]
            auto &parsedRoot = parsed[1];
            std::cout << "Parsed root name: " << parsedRoot.name() << std::endl;
            std::cout << "Parsed root attrib: " << parsedRoot["attrib"] << std::endl;
            auto &parsedChild = parsedRoot[0];
            std::cout << "Parsed child content: " << parsedChild.value() << std::endl;
        }

        return 0;
    }

Summary
-------
The **Neyson::XML** API makes it straightforward to:

1. **Parse** XML from strings or files into a tree of **Node** objects.
2. **Manipulate** the node tree by adding elements, attributes, comments, etc.
3. **Serialize** (write) the node tree back to strings, streams, or files in either compact or readable mode.
