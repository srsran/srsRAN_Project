/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_PRACH_SUBCARRIER_SPACING_H
#define SRSGNB_PRACH_SUBCARRIER_SPACING_H

#include "srsgnb/support/srsran_assert.h"

class prach_subcarrier_spacing
{
public:
  /// Possible PRACH subcarrier spacing.
  enum values { kHz15, kHz30, kHz60, kHz120, kHz1dot25, kHz5 };

  /// Default constructor.
  prach_subcarrier_spacing() : value(kHz15) {}

  /// Construct from value.
  prach_subcarrier_spacing(values value_) : value(value_) {}

  /// Copy constructor.
  prach_subcarrier_spacing(const prach_subcarrier_spacing& other) : value(other.value) {}

  /// Get PRACH subcarrier spacing in Hz.
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
        return 60000;
      case kHz1dot25:
        return 1250;
      case kHz5:
      default:
        break;
    }
    return 5000;
  }

  /// Override operator equal.
  bool operator==(const prach_subcarrier_spacing& other) const { return other.value == value; }

private:
  /// Actual value.
  values value;
};

#endif // SRSGNB_PRACH_SUBCARRIER_SPACING_H
