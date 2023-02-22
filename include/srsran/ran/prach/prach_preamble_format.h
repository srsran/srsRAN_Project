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

namespace srsran {

/// \brief PRACH preamble formats.
///
/// PRACH preamble formats are described in TS38.211 Table 6.3.3.1-1 and 6.3.3.1-2.
class preamble_format
{
public:
  /// Preamble types.
  enum values { FORMAT0 = 0, FORMAT1, FORMAT2, FORMAT3, OTHER };

  /// Creates a default format.
  preamble_format() : value(FORMAT0) {}

  /// Creates a preamble format from value.
  preamble_format(values value_) : value(value_) {}

  /// \brief Checks whether the preamble format is a long or short preamble.
  ///
  /// The preambles are listed as per
  /// - TS38.211 Table 6.3.3.1-1 for long preambles, and
  /// - TS38.211 Table 6.3.3.1-2 for short preambles.
  bool is_long_preamble() const { return (value < OTHER); }

  /// Equality operator overload: comparison with a preamble_format object.
  bool operator==(const preamble_format& other) const { return other.value == value; }

  /// Equality operator overload: comparison with a preamble_format::values value.
  bool operator==(values other_value) const { return other_value == value; }

  /// Inequality operator overload: comparison with a preamble_format object.
  bool operator!=(const preamble_format& other) const { return other.value != value; }

  /// Inequality operator overload: comparison with a preamble_format::values value.
  bool operator!=(values other_value) const { return other_value != value; }

  /// Parenthesis operator overload: returns the value.
  operator values() const { return value; }

private:
  /// Preamble actual value.
  values value;
};

inline const char* to_string(preamble_format format)
{
  switch (format) {
    case preamble_format::FORMAT0:
      return "0";
    case preamble_format::FORMAT1:
      return "1";
    case preamble_format::FORMAT2:
      return "2";
    case preamble_format::FORMAT3:
      return "3";
    case preamble_format::OTHER:
    default:
      return "other";
  }
}

} // namespace srsran
