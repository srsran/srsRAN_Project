/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
  constexpr static const char* names[] = {"FR1", "FR2", "invalid"};
  return names[std::min(static_cast<unsigned>(freq_range), static_cast<unsigned>(frequency_range::FR2) + 1U)];
}

} // namespace srsran
