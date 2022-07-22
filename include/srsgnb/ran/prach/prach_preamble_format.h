/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

namespace srsgnb {

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

} // namespace srsgnb
