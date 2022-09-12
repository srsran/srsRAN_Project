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

#include "srsgnb/scheduler/dmrs.h"

namespace srsgnb {

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
  /// \brief Indicates the PUSCH transmission duration in OFDM symbols, as per TS38.211, Section 6.4.1.1.3.
  ///
  /// Possible values are {1, ..., 14}.
  unsigned duration;
};

/// \brief Calculates the DM-RS for PUSCH symbol mask for single duration.
///
/// It is implemented as per TS38.211 Table 6.4.1.1.3-3.
///
/// Any configuration that is not covered in the table triggers an assertion.
dmrs_symbol_mask pusch_dmrs_symbol_mask_mapping_type_A_single_get(
    const pusch_dmrs_symbol_mask_mapping_type_A_single_configuration& config);

} // namespace srsgnb
