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

#include "srsran/adt/bounded_bitset.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/dmrs.h"
#include "srsran/ran/sch/ldpc_base_graph.h"
#include "srsran/ran/sch/sch_mcs.h"
#include "srsran/ran/sch/sch_segmentation.h"
#include "srsran/support/units.h"

namespace srsran {

/// Collects the necessary parameters to calculate the Downlink Shared Channel (DL-SCH) information.
struct dlsch_configuration {
  /// Transport block size.
  units::bits tbs;
  /// Modulation and coding scheme.
  sch_mcs_description mcs_descr;
  /// Transmission bandwidth in resource blocks.
  unsigned nof_rb;
  /// First OFDM index for the transmission within the slot.
  unsigned start_symbol_index;
  /// Number of OFDM symbols used for the transmission.
  unsigned nof_symbols;
  /// DM-RS Type.
  dmrs_config_type dmrs_type;
  /// Boolean mask indicating which OFDM symbols in the slot contain DM-RS.
  bounded_bitset<MAX_NSYMB_PER_SLOT> dmrs_symbol_mask;
  /// Number of CDM groups without data.
  unsigned nof_cdm_groups_without_data;
  /// Number of transmission layers.
  unsigned nof_layers;
  /// Set to true if the transmission overlaps with the Direct Current (DC).
  bool contains_dc;
};

/// Collects Downlink Shared Channel (DL-SCH) derived parameters.
struct dlsch_information {
  /// Shared channel (SCH) parameters.
  sch_information sch;
  /// Number of encoded and rate-matched DL-SCH data bits.
  units::bits nof_dl_sch_bits;
  /// Number of bits that are affected by overlapping with the direct current.
  units::bits nof_dc_overlap_bits;

  /// \brief Calculates the effective code rate normalized between 0 and 1.
  ///
  /// The effective code rate is determined as the quotient of the number of information bits plus CRCs and the total
  /// number of channel bits.
  float get_effective_code_rate() const
  {
    srsran_assert(nof_dl_sch_bits.value() != 0, "DL-SCH number of bits must not be zero.");
    srsran_assert(sch.nof_bits_per_cb.value() > sch.nof_filler_bits_per_cb.value(),
                  "The number of bits per CB must be greater than the number of filler bits.");
    return static_cast<float>((sch.nof_bits_per_cb.value() - sch.nof_filler_bits_per_cb.value()) * sch.nof_cb) /
           static_cast<float>(nof_dl_sch_bits.value() - nof_dc_overlap_bits.value());
  }
};

/// Gets the DL-SCH information for a given PDSCH configuration.
dlsch_information get_dlsch_information(const dlsch_configuration& config);

} // namespace srsran
