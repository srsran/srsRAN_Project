/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/scheduler/config/dmrs.h"

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
