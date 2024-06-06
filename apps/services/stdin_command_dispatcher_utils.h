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
#include <charconv>
#include <string_view>

namespace srsran {
namespace app_services {

/// Parses integer values from a console command.
template <typename Integer>
inline expected<Integer, std::string> parse_int(std::string_view value)
{
  Integer out_value;
  auto [ptr, errorcode] = std::from_chars(value.begin(), value.end(), out_value);

  if (errorcode == std::errc() && ptr == value.end()) {
    return out_value;
  }

  return fmt::format("Could not convert '{}' to integer", value);
}

/// Parses hex integer values from a console command.
template <typename Integer>
inline expected<Integer, std::string> parse_unsigned_hex(std::string_view value)
{
  try {
    return std::strtoul(value.begin(), nullptr, 16);
  } catch (const std::invalid_argument& e) {
    return {e.what()};
  } catch (const std::out_of_range& e) {
    return {e.what()};
  }
}

/// Parses floating point values from a console command and attempts to store them in a double.
inline expected<double, std::string> parse_double(std::string_view value)
{
  double out_value;
  auto [ptr, errorcode] = std::from_chars(value.begin(), value.end(), out_value);

  if (errorcode == std::errc() && ptr == value.end()) {
    return out_value;
  }

  return fmt::format("Could not convert '{}' to double", value);
}

} // namespace app_services
} // namespace srsran
