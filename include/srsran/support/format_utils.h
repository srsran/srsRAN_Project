/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "fmt/format.h"

namespace srsran {

/// \brief Converts fmt memoryy buffer to c_str() without the need for conversion to intermediate std::string.
template <size_t N>
const char* to_c_str(fmt::basic_memory_buffer<char, N>& mem_buffer)
{
  mem_buffer.push_back('\0');
  return mem_buffer.data();
}

/// \brief Format helper used to insert delimiters between structure fields.
///
/// This class is used to format the structures used by the channel processors. Its methods can be called from an \c fmt
/// custom formatter specialization to aid in format argument parsing and formatting of the structure fields. It
/// automatically inserts the chosen delimiter between the formatted fields.
/// \note The default delimiter is a space character, however, a new line delimiter can be selected by formatting with
/// the \c n format specifier, as in <tt>{:n}</tt>.
/// \note A short formatting mode can be selected by formatting with the \c s format specifier, as in <tt>{:s}</tt>.
/// This option can be used to exclude some of the structure fields for a more compact representation.
class delimited_formatter
{
public:
  /// Default constructor.
  delimited_formatter()
  {
    static const fmt::string_view DEFAULT_FORMAT    = "{}";
    static const fmt::string_view DEFAULT_DELIMITER = " ";
    format_buffer.append(DEFAULT_FORMAT.begin(), DEFAULT_FORMAT.end());
    delimiter_buffer.append(DEFAULT_DELIMITER.begin(), DEFAULT_DELIMITER.end());
  }

  /// Constructor that sets the default delimiter according to the string \c default_delimiter.
  explicit delimited_formatter(const std::string& default_delimiter)
  {
    static const fmt::string_view DEFAULT_FORMAT    = "{}";
    static const fmt::string_view DEFAULT_DELIMITER = default_delimiter;
    format_buffer.append(DEFAULT_FORMAT.begin(), DEFAULT_FORMAT.end());
    delimiter_buffer.append(DEFAULT_DELIMITER.begin(), DEFAULT_DELIMITER.end());
  }

  /// \brief Parsing helper for format specifiers.
  ///
  /// This helper detects the \c s and \c n short mode and new line delimiter specifiers. It also generates a format
  /// string with the parsed specifiers that can be used to propagate the formatting options to nested structures or
  /// structure fields.
  ///
  /// \tparam ParseContext Parse context type.
  /// \param[in] context A character range including the format specifiers.
  /// \return An iterator at the end of the parsed range.
  template <typename ParseContext>
  auto parse(ParseContext& context)
  {
    // Set the first field indicator.
    first = true;

    static const fmt::string_view PREAMBLE_FORMAT   = "{:";
    static const fmt::string_view NEWLINE_DELIMITER = "\n  ";

    // Skip if context is empty and use default format.
    if (context.begin() == context.end()) {
      return context.end();
    }

    format_buffer.clear();
    format_buffer.append(PREAMBLE_FORMAT.begin(), PREAMBLE_FORMAT.end());

    for (auto& it : context) {
      switch (it) {
        case 'n':
          // New line delimiter.
          delimiter_buffer.clear();
          delimiter_buffer.append(NEWLINE_DELIMITER.begin(), NEWLINE_DELIMITER.end());
          break;
        case 's':
          // Short representation.
          verbose = false;
          break;
        case '}':
          format_buffer.push_back(it);
          return &it;
      }
      // Propagate formatting to underlying structures.
      format_buffer.push_back(it);
    }
    // No end of context was found.
    return context.end();
  }

  /// \brief Formats one or more fields with the provided formatting options.
  ///
  /// Generates a string representation of the structure fields, according to the provided \c format formatting string,
  /// and writes it into the output iterator of the formatting \c context.
  ///
  /// \note The fields formatted with this method will be present in the default and the short form structure
  /// representations.
  ///
  /// \tparam FormatContext Format context type.
  /// \tparam Args Formatting arguments parameter pack.
  /// \param[in] context Formatting context, including an output iterator used to write the formatted representation.
  /// \param[in] format Formatting string, containing format specifiers.
  /// \param[in] args Fields to be formatted.
  template <typename FormatContext, typename... Args>
  void format_always(FormatContext& context, const char* format, Args&&... args)
  {
    this->format_to(context, format, std::forward<Args>(args)...);
  }

  /// \brief Formats a single field or nested structure with the parsed formatting options.
  ///
  /// Generates a string representation of the structure field, according to the previously parsed formatting string,
  /// and writes it into the output iterator of the formatting \c context. It can be used to propagate the parsed
  /// formatting options to nested structures.
  ///
  /// \note The fields formatted with this method will be present in the default and the short form structure
  /// representations.
  ///
  /// \tparam FormatContext Format context type.
  /// \tparam Args Formatting arguments parameter pack.
  /// \param[in] context Formatting context, including an output iterator used to write the formatted representation.
  /// \param[in] args Fields to be formatted.
  template <typename FormatContext, typename... Args>
  void format_always(FormatContext& context, Args&&... args)
  {
    this->format_to(context, std::forward<Args>(args)...);
  }

  /// \brief Formats one or more fields with the provided formatting options.
  ///
  /// Generates a string representation of the structure fields, according to the provided \c format formatting string,
  /// and writes it into the output iterator of the formatting \c context.
  ///
  /// \note The fields formatted with this method will not be present in the short form structure representation.
  ///
  /// \tparam FormatContext Format context type.
  /// \tparam Args Formatting arguments parameter pack.
  /// \param[in] context Formatting context, including an output iterator used to write the formatted representation.
  /// \param[in] format Formatting string, containing format specifiers.
  /// \param[in] args Fields to be formatted.
  template <typename FormatContext, typename... Args>
  void format_if_verbose(FormatContext& context, const char* format, Args&&... args)
  {
    if (verbose) {
      this->format_to(context, format, std::forward<Args>(args)...);
    }
  }

  /// \brief Formats a single field or nested structure with the parsed formatting options.
  ///
  /// Generates a string representation of the structure field, according to the previously parsed formatting string,
  /// and writes it into the output iterator of the formatting \c context. It can be used to propagate the parsed
  /// formatting options to nested structures.
  ///
  /// \note The fields formatted with this method will not be present in the short form structure representation.
  ///
  /// \tparam FormatContext Format context type.
  /// \tparam Args Formatting arguments parameter pack.
  /// \param[in] context Formatting context, including an output iterator used to write the formatted representation.
  /// \param[in] args Fields to be formatted.
  template <typename FormatContext, typename... Args>
  void format_if_verbose(FormatContext& context, Args&&... args)
  {
    if (verbose) {
      this->format_to(context, std::forward<Args>(args)...);
    }
  }
  /// \brief Returns \c true if the verbose representation is selected, \c false otherwise.
  /// \remark it must be called after \ref parse.
  bool is_verbose() { return verbose; }

private:
  /// Internal method used to format with any formatting options.
  template <typename FormatContext, typename... Args>
  void format_to(FormatContext& context, const char* format, Args&&... args)
  {
    if (!first) {
      // Buffer to hold the formatted string.
      fmt::memory_buffer temp_buffer;
      fmt::format_to(temp_buffer, format, std::forward<Args>(args)...);

      if (temp_buffer.size() > 0) {
        // Prepend delimiter to the formatted output.
        fmt::format_to(context.out(), "{}", fmt::string_view(delimiter_buffer.data(), delimiter_buffer.size()));
      }

      // Append the formatted string to the context iterator.
      fmt::format_to(context.out(), "{}", fmt::string_view(temp_buffer.data(), temp_buffer.size()));

      return;
    }
    // Format without prepending delimiter.
    fmt::format_to(context.out(), format, std::forward<Args>(args)...);
    first = false;
  }

  /// Internal method used to format with the parsed formatting options.
  template <typename FormatContext, typename... Args>
  void format_to(FormatContext& context, Args&&... args)
  {
    if (!first) {
      // Buffer to hold the formatted string.
      fmt::memory_buffer temp_buffer;
      fmt::format_to(
          temp_buffer, fmt::string_view(format_buffer.data(), format_buffer.size()), std::forward<Args>(args)...);

      if (temp_buffer.size() > 0) {
        // Prepend delimiter to the formatted output.
        fmt::format_to(context.out(), "{}", fmt::string_view(delimiter_buffer.data(), delimiter_buffer.size()));
      }

      // Append the formatted string to the context iterator.
      fmt::format_to(context.out(), "{}", fmt::string_view(temp_buffer.data(), temp_buffer.size()));

      return;
    }
    // Format without prepending delimiter.
    fmt::format_to(
        context.out(), fmt::string_view(format_buffer.data(), format_buffer.size()), std::forward<Args>(args)...);
    first = false;
  }

  /// First field flag. It is used to determine when to insert delimiters between fields.
  bool first = true;
  /// Verbose flag. Verbose format option includes all the fields of the structure in the formatted output.
  bool verbose = true;

  /// Delimiter string.
  fmt::memory_buffer delimiter_buffer;
  /// Format string, used to propagate formatting options to nested structures.
  fmt::memory_buffer format_buffer;
};

} // namespace srsran

namespace fmt {

struct basic_fmt_parser {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }
};

} // namespace fmt
