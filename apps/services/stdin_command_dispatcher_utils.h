/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/expected.h"
#include <cerrno>
#include <charconv>
#include <cstdlib>

namespace srsran {
namespace app_services {

/// Parses integer values from a console command.
template <typename Integer>
inline expected<Integer, std::string> parse_int(std::string_view value)
{
  static_assert(std::is_integral<Integer>::value, "Template type is not an integral");

  Integer out_value;
  auto [ptr, errorcode] = std::from_chars(value.begin(), value.end(), out_value);

  if (errorcode == std::errc() && ptr == value.end()) {
    return out_value;
  }

  return make_unexpected(fmt::format("Could not convert '{}' to integer", value));
}

/// Parses hex integer values from a console command.
template <typename Integer>
inline expected<Integer, std::string> parse_unsigned_hex(std::string_view value)
{
  static_assert(std::is_integral<Integer>::value, "Template type is not an integral");

  // Skip '0x' or '0X' as std::from_chars does not manage it.
  auto start_pos = value.find('x');
  if (start_pos == value.npos) {
    start_pos = value.find('X');
  }

  Integer out_value;
  auto [ptr, errorcode] = std::from_chars(
      (start_pos == value.npos) ? value.begin() : value.begin() + (start_pos + 1), value.end(), out_value, 16);

  if (errorcode == std::errc() && ptr == value.end()) {
    return out_value;
  }

  return make_unexpected(fmt::format("Could not convert '{}' to integer", value));
}

/// \brief Parses floating point values from a console command and attempts to store them in a double.
///
/// Note: using std::string as argument as std::strod requires the array to be null-termitated and std::string_view does
/// not guarantee it.
inline expected<double, std::string> parse_double(const std::string& value)
{
  // Reset a possible previous error.
  errno                = 0;
  const char* str_init = value.c_str();
  char*       str_end  = nullptr;

  double out_value = std::strtod(str_init, &str_end);

  if (errno == ERANGE || str_end == str_init) {
    return make_unexpected(fmt::format("Could not convert '{}' to double", value));
  }

  return out_value;
}

} // namespace app_services
} // namespace srsran
