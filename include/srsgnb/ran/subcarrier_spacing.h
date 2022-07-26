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

#include "srsgnb/ran/frequency_range.h"
#include "srsgnb/support/srsran_assert.h"

namespace srsgnb {

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

/// Convert SCS into integer in kHz
constexpr inline unsigned scs_to_khz(subcarrier_spacing scs)
{
  switch (scs) {
    case subcarrier_spacing::kHz15:
      return 15;
    case subcarrier_spacing::kHz30:
      return 30;
    case subcarrier_spacing::kHz60:
      return 60;
    case subcarrier_spacing::kHz120:
      return 120;
    case subcarrier_spacing::kHz240:
      return 240;
    default:
      break;
  }
  srsgnb_terminate("Invalid SCS");
  return 0;
}

/// Convert SCS to numerology index (mu).
constexpr inline unsigned to_numerology_value(subcarrier_spacing scs)
{
  return static_cast<unsigned>(scs);
}

/// Convert numerology index (mu) to SCS.
constexpr inline subcarrier_spacing to_subcarrier_spacing(unsigned numerology)
{
  return static_cast<subcarrier_spacing>(numerology);
}

/// Calculates number of slots per subframe.
constexpr inline unsigned get_nof_slots_per_subframe(subcarrier_spacing scs)
{
  return 1U << to_numerology_value(scs);
}

} // namespace srsgnb
