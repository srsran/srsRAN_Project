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

#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"
#include "srsran/phy/support/mask_types.h"
#include "srsran/phy/upper/dmrs_mapping.h"
#include "srsran/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "srsran/ran/resource_allocation/rb_bitmap.h"
#include "srsran/ran/resource_block.h"
#include <array>

namespace srsran {

/// \brief Helper function to generate DM-RS sequences.
///
/// This function generates DM-RS sequences based on a pseudo-random sequence generation. It generates \c
/// nof_dmrs_per_rb for every active resource block given in \c crb_mask and skips the \c nof_dmrs_per_rb for every
/// inactive resource block.
///
/// \param[out]    sequence             Destination storage for the generated sequence.
/// \param[in,out] prg                  Pseudo-random generator to use.
/// \param[in]     amplitude            Generated sequence amplitude.
/// \param[in]     reference_point_k_rb Reference Common Resource Block index (CRB).
/// \param[in]     nof_dmrs_per_rb      Number of DM-RS to generate per active CRB.
/// \param[in]     crb_mask             Active resource blocks.
/// \note It assumes that the pseudo-random generator is initialized.
/// \note The sequence size must be consistent with the number of active RBs and the number of DM-RS per RB.
void dmrs_sequence_generate(span<cf_t>               sequence,
                            pseudo_random_generator& prg,
                            float                    amplitude,
                            unsigned                 reference_point_k_rb,
                            unsigned                 nof_dmrs_per_rb,
                            const crb_bitmap&        crb_mask);

/// Parameters for PDSCH and PUSCH DM-RS.
struct dmrs_pxsch_parameters {
  /// DM-RS frequency allocation within a resource block.
  re_prb_mask re_pattern;
  /// Frequency domain weights.
  std::array<float, 2> w_f;
  /// Time domain weights.
  std::array<float, 2> w_t;
};

/// \brief Retrieves DM-RS parameters for both PUSCH and PDSCH channels.
///
/// The DM-RS parameters for PUSCH type 1 and 2 configuration are given in TS38.211 Table 6.4.1.1.3-1 and Table
/// 6.4.1.1.3-2, respectively.
///
/// The DM-RS parameters for PDSCH type 1 and 2 configuration are given in TS38.211 Table 7.4.1.1.2-1 and Table
/// 7.4.1.1.2-2, respectively.
///
/// \param type        Specifies the DM-RS configuration type.
/// \param i_dmrs_port The DM-RS port index, indicating the specific configuration to retrieve.
/// \return The DM-RS parameters for the specified channel and configuration type.
dmrs_pxsch_parameters get_pxsch_dmrs_params(dmrs_type type, unsigned i_dmrs_port);

} // namespace srsran
