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

#include "srsran/ran/frequency_range.h"
#include "srsran/support/srsran_assert.h"

namespace srsran {

/// Number of possible numerology values.
const size_t NOF_NUMEROLOGIES = 5;

/// Representation of subcarrier spacing.
enum class subcarrier_spacing { kHz15 = 0, kHz30, kHz60, kHz120, kHz240, invalid };

/// Check if SCS value is valid.
constexpr inline bool is_scs_valid(subcarrier_spacing scs)
{
  return scs <= subcarrier_spacing::kHz240;
}

/// Checks whether the provided SCS&ndash;FR pair is valid.
constexpr inline bool is_scs_valid(subcarrier_spacing scs, frequency_range fr)
{
  return is_scs_valid(scs) and (((fr == frequency_range::FR2) and scs >= subcarrier_spacing::kHz60) or
                                ((fr == frequency_range::FR1) and scs <= subcarrier_spacing::kHz60));
}

/// Convert SCS to numerology index (\f$\mu\f$).
constexpr inline unsigned to_numerology_value(subcarrier_spacing scs)
{
  return static_cast<unsigned>(scs);
}

/// Converts SCS into integer in kHz.
constexpr inline unsigned scs_to_khz(subcarrier_spacing scs)
{
  srsran_assert(is_scs_valid(scs), "Invalid SCS.");
  return 15U << to_numerology_value(scs);
}

/// Convert numerology index (\f$\mu\f$) to SCS.
constexpr inline subcarrier_spacing to_subcarrier_spacing(unsigned numerology)
{
  return static_cast<subcarrier_spacing>(numerology);
}

/// Convert a string to SCS.
inline subcarrier_spacing to_subcarrier_spacing(const std::string& str)
{
  unsigned in_scs = static_cast<unsigned>(std::strtof(str.data(), nullptr));
  for (unsigned numerology = 0, numerology_end = to_numerology_value(subcarrier_spacing::invalid);
       numerology != numerology_end;
       ++numerology) {
    subcarrier_spacing scs     = to_subcarrier_spacing(numerology);
    unsigned           scs_kHz = scs_to_khz(scs);

    if ((in_scs == scs_kHz) || (in_scs == scs_kHz * 1000)) {
      return scs;
    }
  }
  return subcarrier_spacing::invalid;
}

/// Convert SCS to string.
inline const char* to_string(subcarrier_spacing scs)
{
  switch (scs) {
    case subcarrier_spacing::kHz15:
      return "15kHz";
    case subcarrier_spacing::kHz30:
      return "30kHz";
    case subcarrier_spacing::kHz60:
      return "60kHz";
    case subcarrier_spacing::kHz120:
      return "120kHz";
    case subcarrier_spacing::kHz240:
      return "240kHz";
    case subcarrier_spacing::invalid:
    default:
      return "invalid";
  }
}

/// Calculates number of slots per subframe.
constexpr inline unsigned get_nof_slots_per_subframe(subcarrier_spacing scs)
{
  return 1U << to_numerology_value(scs);
}

} // namespace srsran
