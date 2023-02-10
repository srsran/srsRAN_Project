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

#include "srsgnb/phy/upper/channel_processors/ulsch_placeholder_list.h"
#include "srsgnb/phy/upper/dmrs_mapping.h"
#include "srsgnb/phy/upper/log_likelihood_ratio.h"

/// \file
/// \brief Uplink Shared Channel demultiplexer interface declaration.

namespace srsgnb {

/// \brief Uplink Shared Channel demultiplexer interface.
///
/// User interface of the Uplink Shared Channel (UL-SCH) demultiplexer.
class ulsch_demultiplex
{
public:
  /// Collects the necessary parameters for performing the UL-SCH demultiplex according to TS38.212 Section 6.2.7.
  struct configuration {
    /// Modulation scheme.
    modulation_scheme modulation;
    /// Number of layers {1, ..., 4}
    unsigned nof_layers;
    /// Number of Physical Resource Blocks allocated to the UL-SCH transmission.
    unsigned nof_prb;
    /// Index of the first allocated OFDM symbol within the slot.
    unsigned start_symbol_index;
    /// Number of allocated OFDM symbols.
    unsigned nof_symbols;
    /// Number of reserved bits for HARQ-ACK. Parameter \f$G^\textup{HARQ-ACK}_\textup{rvd}\f$.
    unsigned nof_harq_ack_rvd;
    /// Transmission DM-RS Type.
    dmrs_type dmrs;
    /// Mask indicating which OFDM symbols in the slot contain DM-RS.
    symbol_slot_mask dmrs_symbol_mask;
    /// Number of CDM groups without data.
    unsigned nof_cdm_groups_without_data;
  };

  /// Collects message payload information parameters.
  struct message_information {
    /// Number of HARQ-ACK information bits multiplexed in the PUSCH message. Parameter \f$O_\textup{HARQ-ACK}\f$.
    unsigned nof_harq_ack_bits;
    /// Number of encoded and rate-matched HARQ-ACK data bits. Parameter \f$G^\textup{HARQ-ACK}\f$.
    unsigned nof_enc_harq_ack_bits;
    /// Number of CSI Part 1 information bits multiplexed in the PUSCH message. Parameter \f$O_\textup{CSI-1}\f$.
    unsigned nof_csi_part1_bits;
    /// Number of encoded and rate-matched CSI Part 1 data bits. Parameter \f$G^\textup{HARQ-ACK}\f$.
    unsigned nof_enc_csi_part1_bits;
    /// Number of CSI Part 2 information bits multiplexed in the PUSCH message. Parameter \f$O_\textup{CSI-1}\f$.
    unsigned nof_csi_part2_bits;
    /// Number of encoded and rate-matched CSI Part 2 data bits. Parameter \f$G^\textup{HARQ-ACK}\f$.
    unsigned nof_enc_csi_part2_bits;
  };

  /// Default destructor.
  virtual ~ulsch_demultiplex() = default;

  /// \brief Demultiplexes Uplink Shared Channel (UL-SCH).
  ///
  /// Demultiplexes the different information fields from the UL-SCH transmission.
  ///
  /// \param[out] sch_data   Destination of the resultant shared channel data soft bits.
  /// \param[out] harq_ack   Destination of HARQ-ACK information soft bits.
  /// \param[out] csi_part1  Destination CSI Part 1 report soft bits.
  /// \param[out] csi_part2  Destination CSI Part 2 report soft bits.
  /// \param[in] input       Input soft bits to demultiplex.
  /// \param[in] config      UL-SCH demultiplexing parameters.
  virtual void demultiplex(span<log_likelihood_ratio>       sch_data,
                           span<log_likelihood_ratio>       harq_ack,
                           span<log_likelihood_ratio>       csi_part1,
                           span<log_likelihood_ratio>       csi_part2,
                           span<const log_likelihood_ratio> input,
                           const configuration&             config) = 0;

  /// \brief Gets the list of indexes where the repetition placeholders \f$y\f$ are.
  /// \param[in] uci_message_info UCI message information.
  /// \param[in] config           UL-SCH multiplex configuration.
  /// \return A UL-SCH placeholder list (see \ref ulsch_placeholder_list for more information).
  virtual ulsch_placeholder_list get_placeholders(const message_information& uci_message_info,
                                                  const configuration&       config) = 0;
};

} // namespace srsgnb
