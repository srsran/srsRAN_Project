/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RAN_PRACH_PRACH_PREAMBLES_H
#define SRSGNB_RAN_PRACH_PRACH_PREAMBLES_H

#include "srsgnb/ran/prach/prach_subcarrier_spacing.h"

namespace srsgnb {

/// \brief PRACH preamble formats.
///
/// PRACH preamble formats are described in TS38.211 Table 6.3.3.1-1 and 6.3.3.1-2.
class preamble_format
{
public:
  /// Preambles types.
  enum values { FORMAT0 = 0, FORMAT1, FORMAT2, FORMAT3, OTHER };

  /// Create a default format.
  preamble_format() : value(FORMAT0) {}

  /// Create a preamble format from value.
  preamble_format(values value_) : value(value_) {}

  /// \brief Check whether the preamble format is a long or short preamble.
  ///
  /// The preambles are listed as per
  /// - TS38.211 Table 6.3.3.1-1 for long preambles, and
  /// - TS38.211 Table 6.3.3.1-2 for short preambles.
  bool is_long_preamble() const { return (value < OTHER); }

  /// Override operator equal to other format.
  bool operator==(const preamble_format& other) const { return other.value == value; }

  /// Override operator equal to other format value.
  bool operator==(values other_value) const { return other_value == value; }

  /// Override parenthesis operator for switch cases.
  operator values() const { return value; }

private:
  /// Preamble actual value.
  values value;
};

} // namespace srsgnb

#endif // SRSGNB_RAN_PRACH_PRACH_PREAMBLES_H
