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

namespace srsran {

/// Labels for the frequency ranges described in TS38.104 Table 5.1-1.
enum class frequency_range {
  /// Frequency range 1, from 410 to 7125 MHz.
  FR1 = 0,
  /// Frequency range 2, from 24250 to 52600 MHz.
  FR2
};

constexpr const char* to_string(frequency_range freq_range)
{
  switch (freq_range) {
    case frequency_range::FR1:
      return "FR1";
    case frequency_range::FR2:
      return "FR2";
    default:
      return "invalid";
  }
}

} // namespace srsran
