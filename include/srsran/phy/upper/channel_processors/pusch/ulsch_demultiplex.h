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

#include "srsran/phy/upper/dmrs_mapping.h"
#include "srsran/ran/modulation_scheme.h"

/// \file
/// \brief Uplink Shared Channel demultiplexer interface declaration.

namespace srsran {

class pusch_codeword_buffer;
class pusch_decoder_buffer;

/// \brief User interface of the Uplink Shared Channel (UL-SCH) demultiplexer.
///
/// Note that, for calculating the CSI Part 2 report size, it is necessary to decode the CSI Part 1. Consequently,
/// \ref ulsch_demultiplex::set_csi_part2 must be called with the size information obtained from CSI Part 1 before the
/// CSI Part 2 report can be demultiplexed.
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
    /// Number of HARQ-ACK information bits multiplexed in the PUSCH message. Parameter \f$O_\textup{HARQ-ACK}\f$.
    unsigned nof_harq_ack_bits;
    /// Number of encoded and rate-matched HARQ-ACK data bits. Parameter \f$G^\textup{HARQ-ACK}\f$.
    unsigned nof_enc_harq_ack_bits;
    /// Number of CSI Part 1 information bits multiplexed in the PUSCH message. Parameter \f$O_\textup{CSI-1}\f$.
    unsigned nof_csi_part1_bits;
    /// Number of encoded and rate-matched CSI Part 1 data bits. Parameter \f$G^\textup{CSI-1}\f$.
    unsigned nof_enc_csi_part1_bits;
  };

  /// Default destructor.
  virtual ~ulsch_demultiplex() = default;

  /// \brief Configures the CSI Part 2.
  /// \param[in] csi_part2              CSI Part 2 decoder buffer.
  /// \param[in] nof_csi_part2_bits     Number of CSI Part 2 information bits. Parameter \f$O_\textup{CSI-2}\f$.
  /// \param[in] nof_csi_part2_enc_bits Number of encoded and rate-matched CSI Part 2 data bits. Parameter
  /// \f$G^\textup{CSI-2}\f$.
  virtual void
  set_csi_part2(pusch_decoder_buffer& csi_part2, unsigned nof_csi_part2_bits, unsigned nof_csi_part2_enc_bits) = 0;

  /// \brief Demultiplexes Uplink Shared Channel (UL-SCH).
  ///
  /// Demultiplexes the different information fields from the UL-SCH transmission.
  ///
  /// \param[out] sch_data   Shared channel data soft bits.
  /// \param[out] harq_ack   HARQ-ACK information soft bits.
  /// \param[out] csi_part1  CSI Part 1 report soft bits.
  /// \param[in] config      UL-SCH demultiplexing parameters.
  /// \return A PUSCH codeword buffer, used to feed the PUSCH payload to the demultiplexer.
  virtual pusch_codeword_buffer& demultiplex(pusch_decoder_buffer& sch_data,
                                             pusch_decoder_buffer& harq_ack,
                                             pusch_decoder_buffer& csi_part1,
                                             const configuration&  config) = 0;
};

} // namespace srsran
