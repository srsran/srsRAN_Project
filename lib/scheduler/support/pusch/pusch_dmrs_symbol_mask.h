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

#include "srsran/scheduler/config/dmrs.h"

namespace srsran {

/// \brief Collects the necessary parameters to calculate the DM-RS symbol mask for a TypeA PUSCH mapping and single
/// duration DMRS.
struct pusch_dmrs_symbol_mask_mapping_type_A_single_configuration {
  /// \brief Indicates the first OFDM symbol within the slot carrying DMRS.
  ///
  /// This value is given by higher layer parameter \c dmrs-TypeA-Position. Possible values are {2, 3}.
  dmrs_typeA_position typeA_pos;
  /// \brief Indicates the number of additional DM-RS positions in time domain.
  ///
  /// This value is given by higher layer parameter \c dmrs-AdditionalPosition. Possible values are {0, ..., 3}.
  dmrs_additional_positions additional_position;
  /// \brief Last OFDM symbol scheduled for PUSCH
  ///
  /// It indicates the number of symbols between the first OFDM symbol of the slot and the last OFDM symbol of the
  /// scheduled PUSCH resources in the slot for PUSCH in OFDM symbols. It corresponds to parameter \f$l_d\f$ as
  /// per TS38.211, Section 6.4.1.1.3.
  ///
  /// Possible values are {5, ..., 14}.
  bounded_integer<uint8_t, 5, 14> last_symbol;
};

/// \brief Calculates the DM-RS for PUSCH symbol mask for single duration.
///
/// It is implemented as per TS38.211 Table 6.4.1.1.3-3.
///
/// Any configuration that is not covered in the table triggers an assertion.
dmrs_symbol_mask pusch_dmrs_symbol_mask_mapping_type_A_single_get(
    const pusch_dmrs_symbol_mask_mapping_type_A_single_configuration& config);

} // namespace srsran
