/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/phy/upper/channel_processors/ulsch_placeholder_list.h"
#include "srsran/phy/upper/dmrs_mapping.h"
#include "srsran/phy/upper/log_likelihood_ratio.h"

/// \file
/// \brief Uplink Shared Channel demultiplexer interface declaration.

namespace srsran {

/// \brief User interface of the Uplink Shared Channel (UL-SCH) demultiplexer.
///
/// Note that, for calculating the CSI Part 2 report size, it is necessary to decode the CSI Part 1. Consequently, the
/// CSI Part 1 report soft bits can be demultiplexed with a dedicated method (see \ref
/// ulsch_demultiplex::demultiplex_csi_part1). After decoding the CSI Part 1 report, all the remaining UL-SCH fields can
/// be demultiplexed at once (see \ref ulsch_demultiplex::demultiplex_sch_harq_ack_and_csi_part2).

///
/// All fields can be demultiplexed simultaneously if CSI Part 2 is not present (see \ref
/// ulsch_demultiplex::demultiplex).
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

  /// \brief Demultiplexes the CSI Part 1 report from the UL-SCH transmission.
  /// \param[out] csi_part1             CSI Part 1 report soft bits.
  /// \param[in]  input                 Input soft bits to demultiplex.
  /// \param[in]  nof_enc_harq_ack_bits Number of HARQ-ACK information bits multiplexed in the PUSCH message. Parameter
  ///                                   \f$O_\textup{HARQ-ACK}\f$.
  /// \param[in]  config                UL-SCH demultiplexing parameters.
  virtual void demultiplex_csi_part1(span<log_likelihood_ratio>       csi_part1,
                                     span<const log_likelihood_ratio> input,
                                     unsigned                         nof_enc_harq_ack_bits,
                                     const configuration&             config) = 0;

  /// \brief Demultiplexes the Shared Channel (SCH) data, the HARQ-ACK information bits and the CSI Part 2 report from
  ///        the UL-SCH transmission.
  /// \param[out] sch_data               Shared channel data soft bits.
  /// \param[out] harq_ack               HARQ-ACK information soft bits.
  /// \param[out] csi_part2              CSI Part 2 report soft bits.
  /// \param[in]  input                  Input soft bits to demultiplex.
  /// \param[in]  nof_enc_csi_part1_bits Number of HARQ-ACK information bits multiplexed in the PUSCH message. Parameter
  ///                                    \f$O_\textup{CSI-1}\f$.
  /// \param[in]  config                 UL-SCH demultiplexing parameters.
  virtual void demultiplex_sch_harq_ack_and_csi_part2(span<log_likelihood_ratio>       sch_data,
                                                      span<log_likelihood_ratio>       harq_ack,
                                                      span<log_likelihood_ratio>       csi_part2,
                                                      span<const log_likelihood_ratio> input,
                                                      unsigned                         nof_enc_csi_part1_bits,
                                                      const configuration&             config) = 0;

  /// \brief Demultiplexes Uplink Shared Channel (UL-SCH).
  ///
  /// Demultiplexes the different information fields from the UL-SCH transmission.
  ///
  /// \param[out] sch_data   Shared channel data soft bits.
  /// \param[out] harq_ack   HARQ-ACK information soft bits.
  /// \param[out] csi_part1  CSI Part 1 report soft bits.
  /// \param[out] csi_part2  CSI Part 2 report soft bits.
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

} // namespace srsran
