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

#include "srsgnb/adt/bounded_bitset.h"
#include "srsgnb/ran/cyclic_prefix.h"
#include "srsgnb/ran/dmrs.h"
#include "srsgnb/ran/ldpc_base_graph.h"
#include "srsgnb/ran/modulation_scheme.h"
#include "srsgnb/ran/sch/sch_segmentation.h"
#include <array>

namespace srsgnb {

/// \brief Collects the necessary parameters to calculate the Uplink Shared Channel (UL-SCH) information.
///
/// The parameters are described in TS38.212 Section 6.3.2.4.1.
struct ulsch_configuration {
  /// Transport block size. Set to zero if no shared channel is multiplexed.
  unsigned tbs;
  /// Modulation scheme.
  modulation_scheme modulation;
  /// Target code rate, parameter \f$R\f$. Determined according to TS38.214 Section 6.1.4.1.
  float target_code_rate;
  /// Number of HARQ-ACK bits to multiplex in the transmission. Parameter \f$O_\textup{ACK}\f$.
  unsigned nof_harq_ack_bits;
  /// Number of CSI Part 1 bits to multiplex in the transmission. Parameter \f$O_\textup{CSI-1}\f$.
  unsigned nof_csi_part1_bits;
  /// Number of CSI Part 2 bits to multiplex in the transmission. Parameter \f$O_\textup{CSI-2}\f$.
  unsigned nof_csi_part2_bits;
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
  unsigned nof_ul_sch_bits;
  /// Number of encoded and rate-matched HARQ-ACK data bits. Parameter \f$G^\textup{HARQ-ACK}\f$.
  unsigned nof_harq_ack_bits;
  /// Number of reserved bits for HARQ-ACK. Parameter \f$G^\textup{HARQ-ACK}_\textup{rvd}\f$.
  unsigned nof_harq_ack_rvd;
  /// Number of encoded and rate-matched CSI Part 1 data bits. Parameter \f$G^\textup{CSI-part1}\f$.
  unsigned nof_csi_part1_bits;
  /// Number of encoded and rate-matched CSI Part 2 data bits. Parameter \f$G^\textup{CSI-part2}\f$.
  unsigned nof_csi_part2_bits;
  /// Number of resource elements occupied by HARQ-ACK information. Parameter \f$Q'_\textup{ACK}\f$.
  unsigned nof_harq_ack_re;
  /// Number of resource elements occupied by CSI Part 1 information. Parameter \f$Q'_\textup{CSI-1}\f$.
  unsigned nof_csi_part1_re;
  /// Number of resource elements occupied by CSI Part 1 information. Parameter \f$Q'_\textup{CSI-2}\f$.
  unsigned nof_csi_part2_re;
};

ulsch_information get_ulsch_information(const ulsch_configuration& config);

} // namespace srsgnb
