/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/bounded_bitset.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/dmrs.h"
#include "srsran/ran/ldpc_base_graph.h"
#include "srsran/ran/sch/sch_segmentation.h"
#include "srsran/ran/sch_mcs.h"
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
};

/// Collects Downlink Shared Channel (DL-SCH) derived parameters.
struct dlsch_information {
  /// Shared channel (SCH) parameters.
  sch_information sch;
  /// Number of encoded and rate-matched DL-SCH data bits.
  units::bits nof_dl_sch_bits;

  /// \brief Calculates the effective code rate normalized between 0 and 1.
  ///
  /// The effective code rate is determined as the quotient of the number of information bits plus CRCs and the total
  /// number of channel bits.
  float get_effective_code_rate() const
  {
    srsgnb_assert(nof_dl_sch_bits.value() != 0, "DL-SCH number of bits must not be zero.");
    srsgnb_assert(sch.nof_bits_per_cb.value() > sch.nof_filler_bits_per_cb.value(),
                  "The number of bits per CB must be greater than the number of filler bits.");
    return static_cast<float>((sch.nof_bits_per_cb.value() - sch.nof_filler_bits_per_cb.value()) * sch.nof_cb) /
           static_cast<float>(nof_dl_sch_bits.value());
  }
};

/// Gets the DL-SCH information for a given PDSCH configuration.
dlsch_information get_dlsch_information(const dlsch_configuration& config);

} // namespace srsran
