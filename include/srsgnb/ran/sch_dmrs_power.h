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

#include "srsgnb/support/srsgnb_assert.h"
#include <array>

namespace srsgnb {

/// \brief Calculates the ratio between the Physical Shared Channel EPRE and the DM-RS EPRE.
///
/// The calculation is common for PDSCH and PUSCH, as defined in TS38.214 Table 4.1-1 and Table 6.2.2-1.
///
/// \param[in] nof_cdm_groups_without_data Number of CDM groups without data.
/// \return Parameter \f$\beta_{\textup{DMRS}}\f$ in decibels.
inline float get_sch_to_dmrs_ratio_dB(unsigned nof_cdm_groups_without_data)
{
  srsgnb_assert(nof_cdm_groups_without_data > 0 && nof_cdm_groups_without_data < 4,
                "Invalid number of DMRS CDM groups without data.");
  static constexpr std::array<float, 4> beta_dmrs_values = {NAN, 0, -3, -4.77};
  return beta_dmrs_values[nof_cdm_groups_without_data];
}

} // namespace srsgnb