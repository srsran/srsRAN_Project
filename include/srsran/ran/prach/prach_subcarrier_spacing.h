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

#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/support/srsran_assert.h"

namespace srsran {

/// Random Access subcarrier spacing.
enum class prach_subcarrier_spacing : uint8_t { kHz15 = 0, kHz30, kHz60, kHz120, kHz1_25, kHz5, invalid };

/// Checks if SCS value is valid.
constexpr inline bool is_scs_valid(prach_subcarrier_spacing scs)
{
  return scs < prach_subcarrier_spacing::invalid;
}

/// Checks if the RA subcarrier spacing is suitable for long preambles formats.
constexpr inline bool is_long_preamble(prach_subcarrier_spacing ra_scs)
{
  return (ra_scs != prach_subcarrier_spacing::invalid) && (ra_scs >= prach_subcarrier_spacing::kHz1_25);
}

/// Checks if the RA subcarrier spacing is suitable for short preambles formats.
constexpr inline bool is_short_preamble(prach_subcarrier_spacing ra_scs)
{
  return (ra_scs != prach_subcarrier_spacing::invalid) && (ra_scs < prach_subcarrier_spacing::kHz1_25);
}

/// Converts SCS to string.
inline const char* to_string(prach_subcarrier_spacing ra_scs)
{
  switch (ra_scs) {
    case prach_subcarrier_spacing::kHz15:
      return "15kHz";
    case prach_subcarrier_spacing::kHz30:
      return "30kHz";
    case prach_subcarrier_spacing::kHz60:
      return "60kHz";
    case prach_subcarrier_spacing::kHz120:
      return "120kHz";
    case prach_subcarrier_spacing::kHz1_25:
      return "1.25kHz";
    case prach_subcarrier_spacing::kHz5:
      return "5kHz";
    case prach_subcarrier_spacing::invalid:
    default:
      return "invalid";
  }
}

/// Converts SCS to numerology index (\f$\mu\f$).
constexpr inline unsigned to_numerology_value(prach_subcarrier_spacing ra_scs)
{
  srsran_assert(
      is_short_preamble(ra_scs), "RA subcarrier spacing (i.e., {}) must be for short preamble.", to_string(ra_scs));
  return static_cast<unsigned>(ra_scs);
}

/// Converts SCS to its integer value in hertz.
constexpr inline unsigned ra_scs_to_Hz(prach_subcarrier_spacing ra_scs)
{
  srsran_assert(is_scs_valid(ra_scs), "Invalid SCS.");
  switch (ra_scs) {
    case prach_subcarrier_spacing::kHz15:
    case prach_subcarrier_spacing::kHz30:
    case prach_subcarrier_spacing::kHz60:
    case prach_subcarrier_spacing::kHz120:
    case prach_subcarrier_spacing::invalid:
    default:
      return 15000U << to_numerology_value(ra_scs);
    case prach_subcarrier_spacing::kHz1_25:
      return 1250;
    case prach_subcarrier_spacing::kHz5:
      return 5000;
  }
}

/// Adapts common resource grid subcarrier spacing to RA subcarrier spacing.
constexpr inline prach_subcarrier_spacing to_ra_subcarrier_spacing(subcarrier_spacing scs)
{
  // Convert to numerology.
  unsigned numerology = to_numerology_value(scs);

  // Check if the numerology exceeds the maximum for RA.
  if (numerology > static_cast<unsigned>(prach_subcarrier_spacing::kHz120)) {
    return prach_subcarrier_spacing::invalid;
  }

  // Convert to RA subcarrier spacing.
  return static_cast<prach_subcarrier_spacing>(numerology);
}

/// Converts a numerical value (a bandwidth in hertz) to an SCS.
inline prach_subcarrier_spacing to_ra_subcarrier_spacing(unsigned in_scs_Hz)
{
  for (unsigned index = 0, index_end = static_cast<unsigned>(prach_subcarrier_spacing::invalid); index != index_end;
       ++index) {
    prach_subcarrier_spacing scs    = static_cast<prach_subcarrier_spacing>(index);
    unsigned                 scs_Hz = ra_scs_to_Hz(scs);

    if ((in_scs_Hz == scs_Hz) || (in_scs_Hz == scs_Hz * 1000)) {
      return scs;
    }
  }
  return prach_subcarrier_spacing::invalid;
}

/// Converts a numeric string (representing a bandwidth in kilohertz) to an SCS.
inline prach_subcarrier_spacing to_ra_subcarrier_spacing(const char* str)
{
  unsigned in_scs_Hz = static_cast<unsigned>(1e3F * std::strtof(str, nullptr));
  return to_ra_subcarrier_spacing(in_scs_Hz);
}

} // namespace srsran