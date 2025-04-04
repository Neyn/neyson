JSON Usage Tutorial
===================

.. contents::
   :depth: 2
   :local:

Introduction
------------
The **Neyson::Json** namespace provides a set of functions and types specifically geared toward reading, parsing, and writing JSON data. At its core, this interface leverages the **Value** class from the Neyson library to represent JSON structures in memory. You can parse JSON strings or files into a **Value**, manipulate that data, and then serialize it back to a string, file, or stream. This tutorial will walk you through how to effectively use the JSON interface—covering reading, writing, error handling, and key enumerations like **Error**, **Result**, and **Mode**.

Overview
--------
The JSON interface primarily consists of:

- **Error**: An enum indicating various parse or write error types.
- **Result**: A struct that holds an **Error** code and an index (useful for parsing).
- **Mode**: An enum controlling the formatting style (compact vs. readable) when writing JSON.
- **read / sread / fread**: Functions for parsing JSON from different sources (string, C-string, file).
- **write / fwrite**: Functions for serializing a **Value** to different targets (string, stream, file).

These functions work in concert to provide a simple yet robust API for JSON I/O. Below, each function is explained in detail along with common usage patterns.

Error Codes
-----------
The **Error** enum class defines possible outcomes when parsing or writing JSON:

.. code-block:: c++

    enum class Error
    {
        None,
        FileIOError,
        InvalidNumber,
        InvalidString,
        InvalidValueType,
        ExpectedColon,
        ExpectedComma,
        ExpectedStart,
        ExpectedQuoteOpen,
        ExpectedQuoteClose,
        ExpectedBraceOpen,
        ExpectedBraceClose,
        ExpectedBracketOpen,
        ExpectedBracketClose,
        ExpectedCommaOrBraceClose,
        ExpectedCommaOrBracketClose,
        FailedToReachEnd,
        UnexpectedValueStart,
    };

- **None**: No error occurred.
- **FileIOError**: The JSON file could not be accessed or written.
- **InvalidNumber**, **InvalidString**: A malformed number or string in the JSON.
- **InvalidValueType**: Encountered an unsupported or unrecognized type when writing.
- **ExpectedColon**, **ExpectedComma**, etc.: Common structural errors when parsing.
- **FailedToReachEnd**: The parser didn’t consume the entire input; unexpected data remains.
- **UnexpectedValueStart**: A token was invalid at the beginning of what should have been a JSON value.

Understanding these codes is crucial for diagnosing JSON parsing or serialization problems.

Result Structure
----------------
The **Result** struct pairs an **Error** with an **index**, indicating how many bytes have been processed when parsing:

.. code-block:: c++

    struct Result
    {
        Error error;
        size_t index;

        inline Result(Error error = Error::None, size_t index = size_t(-1))
            : error(error), index(index) {}

        inline operator bool() const { return error == Error::None; }
    };

Key points:

- **error**: Holds the **Error** code.
- **index**: Reflects the position in the string/file stream where parsing ended or where an error occurred.
- **operator bool()**: Evaluates to `true` if `error == Error::None`, making it easy to check if the operation succeeded.

Mode Enumeration
----------------
The **Mode** enum controls how JSON output is formatted:

.. code-block:: c++

    enum class Mode
    {
        Compact,
        Readable,
    };

- **Compact** mode: Produces minimal whitespace—ideal for space-efficient output.
- **Readable** mode: Adds indentation, newlines, and spacing to enhance human readability.

Reading JSON
------------
The interface provides multiple functions for reading JSON data into a **Value**:

1. **sread**

   .. code-block:: c++

       Result sread(Value &value, const char *str);

   - Parses a null-terminated C-string `str` containing JSON data.
   - Populates `value` with the parsed JSON structure.
   - Returns a **Result** indicating success or error.

2. **read**

   .. code-block:: c++

       Result read(Value &value, const std::string &str);

   - Similar to `sread`, but takes an `std::string` instead of a C-string.
   - On success, `value` is fully populated.

3. **fread**

   .. code-block:: c++

       Result fread(Value &value, const std::string &path);

   - Attempts to open the file at the given path.
   - Reads and parses its contents into `value`.
   - Returns an **Error** code of **FileIOError** if the file cannot be opened.

Example: Reading JSON from a String
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.. code-block:: c++

    #include <neyson/neyson.h>
    using namespace Neyson;

    int main()
    {
        Value data;
        std::string jsonString = R"({
            "id": 123,
            "name": "Example",
            "values": [1, 2, 3]
        })";

        auto result = Json::read(data, jsonString);
        if (!result) {
            // Something went wrong
            std::cerr << "Error parsing JSON: " << result.error << " at index " << result.index << std::endl;
            return 1;
        }

        std::cout << "JSON read successfully!\n";
        std::cout << "Name: " << data["name"].string() << std::endl;
        return 0;
    }

Writing JSON
------------
To convert a **Value** back to JSON text, use one of the following:

1. **write** (overload that writes to a `std::string`)

   .. code-block:: c++

       Result write(const Value &value, std::string &data, Mode mode = Mode::Readable);

   - Serializes `value` into `data`.
   - You can specify **Mode::Compact** for dense output or **Mode::Readable** for pretty-printed output.

2. **write** (overload that writes to a `std::ostream *`)

   .. code-block:: c++

       Result write(const Value &value, std::ostream *stream, Mode mode = Mode::Readable);

   - Writes the serialized JSON directly to an output stream (e.g., `std::cout`, file streams, etc.).

3. **fwrite**

   .. code-block:: c++

       Result fwrite(const Value &value, const std::string &path, Mode mode = Mode::Compact);

   - Serializes `value` and saves it to the specified file.

Example: Writing JSON to a File
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.. code-block:: c++

    #include <neyson/neyson.h>
    using namespace Neyson;

    int main()
    {
        Value root;
        root["status"] = "success";
        root["count"] = 3;
        root["items"] = { "alpha", "beta", "gamma" };

        auto result = Json::fwrite(root, "output.json", Json::Mode::Readable);
        if (!result) {
            std::cerr << "Error writing JSON: " << result.error << std::endl;
            return 1;
        }

        std::cout << "JSON written to output.json\n";
        return 0;
    }

Error Handling
--------------
Every read/write function returns a **Result** that indicates whether the operation succeeded. Common checks include:

.. code-block:: c++

    if (!result) {
        // The error code can be result.error
        // The index can help locate parsing issues
        std::cerr << "Operation failed with error: " << result.error
                  << " at index: " << result.index << std::endl;
        // Handle or return
    }

Because `Result` has an `operator bool()`, you can also test it directly in if statements, as shown above.

Logging or Inspecting Errors
~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The JSON interface overloads `operator<<` for **Error** and **Result**, making them easy to log:

.. code-block:: c++

    std::cerr << "Error code: " << result.error << std::endl;
    // or
    std::cerr << "Full result: " << result << std::endl;

The latter prints both the error code and index.

Practical Example
-----------------
1. **Parsing and Editing**
   Suppose you read JSON from a file, modify it, then save it back:

   .. code-block:: c++

       Value data;
       auto parseResult = Json::fread(data, "config.json");
       if (!parseResult) {
           std::cerr << "Failed to read config.json: " << parseResult << std::endl;
           return;
       }

       // Modify data
       data["enabled"].boolean(true);
       data["threshold"] = 0.9;

       // Write changes
       auto writeResult = Json::fwrite(data, "config_modified.json", Json::Mode::Readable);
       if (!writeResult) {
           std::cerr << "Failed to write config_modified.json: " << writeResult << std::endl;
       } else {
           std::cout << "JSON successfully modified and saved.\n";
       }

2. **Using Strings Directly**
   If you have JSON data in a string (e.g., from a network response), use `Json::read` to parse and `Json::write` to output the modified data:

   .. code-block:: c++

       std::string input = R"({"field": "value"})";
       Value doc;
       auto res = Json::read(doc, input);
       if (res) {
           doc["newKey"] = 42;

           std::string output;
           Json::write(doc, output, Json::Mode::Compact);
           std::cout << "New JSON: " << output << std::endl;
       }

Summary
-------
The **Neyson::Json** interface offers a straightforward API for reading, parsing, and writing JSON data using the **Value** class:

- Use **sread**, **read**, or **fread** to parse data from various sources.
- Use **write** or **fwrite** to serialize a **Value** back into JSON text, with **Mode** controlling the formatting.
- Always check the returned **Result** for errors, and use the **Error** enum to diagnose issues.

This powerful yet flexible interface simplifies common tasks like loading configuration files, sending or receiving JSON over the network, or building complex data structures for storage or processing.
