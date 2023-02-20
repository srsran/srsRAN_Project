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

#include "srsgnb/adt/bounded_bitset.h"
#include "srsgnb/ran/cyclic_prefix.h"
#include "srsgnb/ran/dmrs.h"
#include "srsgnb/ran/ldpc_base_graph.h"
#include "srsgnb/ran/sch/sch_segmentation.h"
#include "srsgnb/ran/sch_mcs.h"
#include "srsgnb/support/units.h"

namespace srsgnb {

/// \brief Collects the necessary parameters to calculate the Uplink Shared Channel (UL-SCH) information.
///
/// The parameters are described in TS38.212 Section 6.3.2.4.1.
struct ulsch_configuration {
  /// Transport block size. Set to zero if no shared channel is multiplexed.
  units::bits tbs;
  /// Modulation and coding scheme.
  sch_mcs_description mcs_descr;
  /// Number of HARQ-ACK bits to multiplex in the transmission. Parameter \f$O_\textup{ACK}\f$.
  units::bits nof_harq_ack_bits;
  /// Number of CSI Part 1 bits to multiplex in the transmission. Parameter \f$O_\textup{CSI-1}\f$.
  units::bits nof_csi_part1_bits;
  /// Number of CSI Part 2 bits to multiplex in the transmission. Parameter \f$O_\textup{CSI-2}\f$.
  units::bits nof_csi_part2_bits;
  /// \brief Parameter \f$\alpha\f$ as per TS38.212 Section 6.3.2.4.
  ///
  /// Provided by the higher layer parameter \e scaling in TS38.331 Section 6.3.2, Information Element \e UCI-OnPUSCH.
  float alpha_scaling;
  /// Parameter \f$\beta _\textup{offset} ^\textup{HARQ-ACK}\f$.
  float beta_offset_harq_ack;
  /// Parameter \f$\beta _\textup{offset} ^\textup{CSI-1}\f$.
  float beta_offset_csi_part1;
  /// Parameter \f$\beta _\textup{offset} ^\textup{CSI-2}\f$.
  float beta_offset_csi_part2;
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

/// \brief Collects Uplink Shared Channel (UL-SCH) derived parameters.
///
/// The parameters are described in TS38.212 Section 6.3.2.4.1.
struct ulsch_information {
  /// Shared channel (SCH) parameters.
  sch_information sch;
  /// Number of encoded and rate-matched UL-SCH data bits. Parameter \f$G^\textup{UL-SCH}\f$.
  units::bits nof_ul_sch_bits;
  /// Number of encoded and rate-matched HARQ-ACK data bits. Parameter \f$G^\textup{HARQ-ACK}\f$.
  units::bits nof_harq_ack_bits;
  /// Number of reserved bits for HARQ-ACK. Parameter \f$G^\textup{HARQ-ACK}_\textup{rvd}\f$.
  units::bits nof_harq_ack_rvd;
  /// Number of encoded and rate-matched CSI Part 1 data bits. Parameter \f$G^\textup{CSI-part1}\f$.
  units::bits nof_csi_part1_bits;
  /// Number of encoded and rate-matched CSI Part 2 data bits. Parameter \f$G^\textup{CSI-part2}\f$.
  units::bits nof_csi_part2_bits;
  /// Number of resource elements occupied by HARQ-ACK information. Parameter \f$Q'_\textup{ACK}\f$.
  unsigned nof_harq_ack_re;
  /// Number of resource elements occupied by CSI Part 1 information. Parameter \f$Q'_\textup{CSI-1}\f$.
  unsigned nof_csi_part1_re;
  /// Number of resource elements occupied by CSI Part 1 information. Parameter \f$Q'_\textup{CSI-2}\f$.
  unsigned nof_csi_part2_re;

  /// \brief Calculates the effective code rate normalized between 0 and 1.
  ///
  /// The effective code rate is determined as the quotient of the number of information bits plus CRCs and the total
  /// number of channel bits.
  float get_effective_code_rate() const
  {
    srsgnb_assert(nof_ul_sch_bits.value() != 0, "UL-SCH number of bits must not be zero.");
    srsgnb_assert(sch.nof_bits_per_cb.value() > sch.nof_filler_bits_per_cb.value(),
                  "The number of bits per CB must be greater than the number of filler bits.");
    return static_cast<float>((sch.nof_bits_per_cb.value() - sch.nof_filler_bits_per_cb.value()) * sch.nof_cb) /
           static_cast<float>(nof_ul_sch_bits.value());
  }
};

/// Gets the UL-SCH information for a given PUSCH configuration.
ulsch_information get_ulsch_information(const ulsch_configuration& config);

} // namespace srsgnb
