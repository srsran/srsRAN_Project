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

#include <algorithm>

namespace srsran {

/// Labels for the frequency ranges described in TS38.104 Table 5.1-1.
enum class frequency_range {
  /// Frequency range 1, from 410 to 7125 MHz.
  FR1 = 0,
  /// Frequency range 2, from 24250 to 52600 MHz.
  FR2
};

inline const char* to_string(frequency_range freq_range)
{
  static constexpr const char* names[] = {"FR1", "FR2", "invalid"};
  return names[std::min(static_cast<unsigned>(freq_range), static_cast<unsigned>(frequency_range::FR2) + 1U)];
}

} // namespace srsran
