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

#include "srsran/support/srsran_assert.h"
#include <array>

namespace srsran {

/// \brief Calculates the ratio between the Physical Shared Channel EPRE and the DM-RS EPRE.
///
/// The calculation is common for PDSCH and PUSCH, as defined in TS38.214 Table 4.1-1 and Table 6.2.2-1.
///
/// \param[in] nof_cdm_groups_without_data Number of CDM groups without data.
/// \return Parameter \f$\beta_{\textup{DMRS}}\f$ in decibels.
inline float get_sch_to_dmrs_ratio_dB(unsigned nof_cdm_groups_without_data)
{
  srsran_assert(nof_cdm_groups_without_data > 0 && nof_cdm_groups_without_data < 4,
                "Invalid number of DMRS CDM groups without data.");
  static constexpr std::array<float, 4> beta_dmrs_values = {NAN, 0, -3, -4.77};
  return beta_dmrs_values[nof_cdm_groups_without_data];
}

} // namespace srsran