/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/adt/bounded_integer.h"

namespace srsran {

/// Phase Tracking Reference Signals (PT-RS) frequency domain density.
enum class ptrs_frequency_density : uint8_t { two = 2, four = 4 };

/// Converts PT-RS frequency domain density to a string.
inline const char* to_string(ptrs_frequency_density freq_density)
{
  switch (freq_density) {
    case ptrs_frequency_density::two:
      return "two";
    case ptrs_frequency_density::four:
      return "four";
  }
  return "invalid";
}

/// Converts PT-RS frequency domain density to an integer.
inline unsigned to_value(ptrs_frequency_density freq_density)
{
  return static_cast<unsigned>(freq_density);
}

/// Phase Tracking Reference Signals (PT-RS) time domain density.
enum class ptrs_time_density : uint8_t { one = 1, two = 2, four = 4 };

/// Converts PT-RS time domain density to a string.
inline const char* to_string(ptrs_time_density time_density)
{
  switch (time_density) {
    case ptrs_time_density::one:
      return "one";
    case ptrs_time_density::two:
      return "two";
    case ptrs_time_density::four:
      return "four";
  }
  return "invalid";
}

/// Converts PT-RS time domain density to an integer.
inline unsigned to_value(ptrs_time_density time_density)
{
  return static_cast<unsigned>(time_density);
}

/// Phase Tracking Reference Signals (PT-RS) RE offset.
enum class ptrs_re_offset : uint8_t { offset00 = 0, offset01 = 1, offset10 = 2, offset11 = 3 };

/// Converts PT-RS RE offset to a string.
inline const char* to_string(ptrs_re_offset re_offset)
{
  switch (re_offset) {
    case ptrs_re_offset::offset00:
      return "offset00";
    case ptrs_re_offset::offset01:
      return "offset01";
    case ptrs_re_offset::offset10:
      return "offset10";
    case ptrs_re_offset::offset11:
      return "offset11";
  }
  return "invalid";
}

/// Converts PT-RS RE offset to an integer.
inline unsigned to_value(ptrs_re_offset re_offset)
{
  return static_cast<unsigned>(re_offset);
}

} // namespace srsran
