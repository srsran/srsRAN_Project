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

#include "srsran/support/srsran_assert.h"

namespace srsran {

class prach_subcarrier_spacing
{
public:
  /// Possible PRACH subcarrier spacing.
  enum values { kHz15, kHz30, kHz60, kHz120, kHz1_25, kHz5 };

  /// Default constructor.
  prach_subcarrier_spacing() : value(kHz15) {}

  /// Construct from value.
  prach_subcarrier_spacing(values value_) : value(value_) {}

  /// Copy constructor.
  prach_subcarrier_spacing(const prach_subcarrier_spacing& other) : value(other.value) {}

  /// Gets PRACH subcarrier spacing in Hz.
  unsigned to_Hz() const
  {
    switch (value) {
      case kHz15:
        return 15000;
      case kHz30:
        return 30000;
      case kHz60:
        return 60000;
      case kHz120:
        return 120000;
      case kHz1_25:
        return 1250;
      case kHz5:
      default:
        return 5000;
    }
  }

  /// Implicit cast to an enum.
  operator values() const { return value; }

private:
  /// Actual value.
  values value;
};

} // namespace srsran