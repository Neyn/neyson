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

namespace Neyson
{
namespace Json
{
/// @brief Enumeration of error codes that may result from parsing or writing JSON.
///
/// @details These error codes provide insight into why a JSON operation may have failed.
///          For example, InvalidNumber indicates the parser encountered a malformed
///          numeric value in the input.
enum class Error
{
    /// @brief No error occurred.
    None,

    /// @brief File could not be opened, read, or written to.
    FileIOError,

    /// @brief An invalid or malformed number was encountered during parsing.
    InvalidNumber,

    /// @brief An invalid or malformed string was encountered during parsing.
    InvalidString,

    /// @brief An invalid or unknown value type was encountered when writing.
    InvalidValueType,

    /// @brief Expected a colon ':' in the JSON structure.
    ExpectedColon,

    /// @brief Expected a comma ',' in the JSON structure.
    ExpectedComma,

    /// @brief Expected a valid JSON start.
    ExpectedStart,

    /// @brief Expected a double-quote '"' to open a string.
    ExpectedQuoteOpen,

    /// @brief Expected a double-quote '"' to close a string.
    ExpectedQuoteClose,

    /// @brief Expected an opening brace '{' in the JSON object.
    ExpectedBraceOpen,

    /// @brief Expected a closing brace '}' in the JSON object.
    ExpectedBraceClose,

    /// @brief Expected an opening bracket '[' in the JSON array.
    ExpectedBracketOpen,

    /// @brief Expected a closing bracket ']' in the JSON array.
    ExpectedBracketClose,

    /// @brief Expected either a comma ',' or a closing brace '}' to continue/close an object.
    ExpectedCommaOrBraceClose,

    /// @brief Expected either a comma ',' or a closing bracket ']' to continue/close an array.
    ExpectedCommaOrBracketClose,

    /// @brief The parser did not reach the end of the input as expected.
    FailedToReachEnd,

    /// @brief An unexpected token was encountered where a JSON was supposed to start.
    UnexpectedValueStart,
};

/// @brief A struct representing the result of parsing or writing a JSON document.
///
/// @details This struct holds the error code indicating whether the operation succeeded,
///          and, in case of reading, how many bytes were processed.
struct Result
{
    /// @brief The error code indicating success or failure of the operation.
    Error error;

    /// @brief The number of bytes processed (e.g., from an input string).
    ///        Typically set when reading/parsing a JSON document.
    size_t index;

    /// @brief Constructs a Result object.
    ///
    /// @param error The error code (defaults to Error::None).
    /// @param index The number of bytes processed (defaults to size_t(-1)).
    inline Result(Error error = Error::None, size_t index = size_t(-1)) : error(error), index(index) {}

    /// @brief Indicates success or failure of the operation as a boolean.
    ///
    /// @return True if error is Error::None, otherwise false.
    inline operator bool() const { return error == Error::None; }
};

/// @brief An enumeration of modes for writing JSON.
///
/// @details These modes control the formatting of output when serializing
///          a JSON Value to text.
enum class Mode
{
    /// @brief Compact mode produces minimal whitespace and no newlines.
    Compact,

    /// @brief Readable mode uses indentation, spacing, and newlines for clarity.
    Readable,
};

/// @brief Reads/parses a JSON document from a C-string into the given Value.
///
/// @details This function attempts to parse the null-terminated string @p str
///          and populate @p value with the resulting JSON structure.
/// @param value The Value that will be populated with the parsed JSON data.
/// @param str   A null-terminated C-string containing the JSON document.
/// @return A Result indicating success (Error::None) or the type of error if parsing fails.
Result sread(Value &value, const char *str);

/// @brief Reads/parses a JSON document from an std::string into the given Value.
///
/// @details This function attempts to parse the string @p str and populate
///          @p value with the resulting JSON structure.
/// @param value The Value that will be populated with the parsed JSON data.
/// @param str   An std::string containing the JSON document.
/// @return A Result indicating success (Error::None) or the type of error if parsing fails.
Result read(Value &value, const std::string &str);

/// @brief Reads/parses a JSON document from a file into the given Value.
///
/// @details This function attempts to open the file at path @p path, read its contents,
///          and parse the data to populate @p value.
/// @param value The Value that will be populated with the parsed JSON data.
/// @param path  The path to the file containing the JSON document.
/// @return A Result indicating success (Error::None) or the type of error if reading/parsing fails.
Result fread(Value &value, const std::string &path);

/// @brief Writes a JSON Value to a string using the given output mode.
///
/// @details This function serializes @p value into a textual JSON representation
///          and stores it in @p data. The @p mode parameter controls whether the output
///          is formatted for readability or in a compact form.
/// @param value The JSON Value to be serialized.
/// @param data  A reference to a std::string where the JSON text will be stored.
/// @param mode  The desired output formatting mode. Defaults to Mode::Readable.
/// @return A Result indicating success (Error::None) or the type of error if serialization fails.
Result write(const Value &value, std::string &data, Mode mode = Mode::Readable);

/// @brief Writes a JSON Value to a given output stream using the given mode.
///
/// @details This function serializes @p value into JSON text and writes it
///          directly to the output stream pointed to by @p stream.
/// @param value  The JSON Value to be serialized.
/// @param stream Pointer to a valid std::ostream instance where the JSON text will be written.
/// @param mode   The desired output formatting mode. Defaults to Mode::Readable.
/// @return A Result indicating success (Error::None) or the type of error if serialization fails.
Result write(const Value &value, std::ostream *stream, Mode mode = Mode::Readable);

/// @brief Writes a JSON Value to a file at the specified path using the given mode.
///
/// @details This function serializes @p value into JSON text and writes it to the file
///          at @p path. The @p mode parameter controls whether the output is formatted
///          for readability or in a compact form.
/// @param value The JSON Value to be serialized.
/// @param path  The file path where the JSON text will be written.
/// @param mode  The desired output formatting mode. Defaults to Mode::Readable.
/// @return A Result indicating success (Error::None) or the type of error if the operation fails.
Result fwrite(const Value &value, const std::string &path, Mode mode = Mode::Readable);

/// @brief Outputs the textual representation of an Error code to the provided output stream.
///
/// @details This operator inserts a string or other representation of the error
///          enum value into the provided @p os.
/// @param os    The output stream to write to.
/// @param error The Error code to print.
/// @return A reference to the modified output stream.
std::ostream &operator<<(std::ostream &os, Error error);

/// @brief Outputs the textual representation of a Result object to the provided output stream.
///
/// @details This operator inserts a string representation of both the error code
///          and any relevant index information into the provided @p os.
/// @param os     The output stream to write to.
/// @param result The Result object to print.
/// @return A reference to the modified output stream.
std::ostream &operator<<(std::ostream &os, const Result &result);

/// @brief Outputs the textual representation of a Mode enum to the provided output stream.
///
/// @details This operator inserts a string indicating whether the mode is Compact or Readable.
/// @param os   The output stream to write to.
/// @param mode The Mode enum value to print.
/// @return A reference to the modified output stream.
std::ostream &operator<<(std::ostream &os, Mode mode);
}  // namespace Json
}  // namespace Neyson
