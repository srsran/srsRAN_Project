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

/// \file
/// \brief PUSCH decoder interface declaration.

#pragma once

#include "srsran/adt/span.h"
#include "srsran/phy/upper/log_likelihood_ratio.h"
#include "srsran/ran/sch/ldpc_base_graph.h"
#include "srsran/ran/sch/modulation_scheme.h"

namespace srsran {

class pusch_decoder_buffer;
class pusch_decoder_notifier;
class rx_buffer;
class unique_rx_buffer;
struct pusch_decoder_result;

/// \brief PUSCH decoder interface.
///
/// Recovers a UL-SCH transport block from a PUSCH codeword. Reverting the encoding operations described in TS38.212
/// Sections 6.2.1-6.2.6, the codeword is first split into rate-matched codeblocks. Then, each codeblock is restored
/// to its base rate, combined with previous retransmissions, and decoded. Finally, if all blocks pass the CRC check,
/// the data bits from all codeblocks are concatenated to form the UL-SCH transport block. If applicable, a last
/// transport-block CRC is computed and verified.
///
/// The PUSCH decoder is configured using the method \ref new_data. This method returns a \ref pusch_decoder_buffer
/// which is used to write softbits into the decoder.
///
/// The decoder could potentially start the code block processing if a codeword length is provided (see
/// \ref set_codeword_length) prior \ref pusch_decoder_buffer::on_end_softbits.
class pusch_decoder
{
public:
  /// Collects the parameters necessary for decoding a PUSCH transport block.
  struct configuration {
    /// Code base graph.
    ldpc_base_graph_type base_graph = ldpc_base_graph_type::BG1;
    /// Redundancy version, values in {0, 1, 2, 3}.
    unsigned rv = 0;
    /// Modulation scheme.
    modulation_scheme mod = modulation_scheme::BPSK;
    /// \brief Limited buffer rate matching length in bits, as per TS38.212 Section 5.4.2.
    /// \note Set to zero for unlimited buffer length.
    unsigned Nref = 0;
    /// Number of transmission layers the transport block is mapped onto.
    unsigned nof_layers = 0;
    /// Maximum number of iterations of the LDPC decoder.
    unsigned nof_ldpc_iterations = 6;
    /// Flag for LDPC decoder early stopping: \c true to activate.
    bool use_early_stop = true;
    /// Flag to denote new data (first HARQ transmission).
    bool new_data = true;
  };

  /// Default destructor.
  virtual ~pusch_decoder() = default;

  /// \brief Decodes a PUSCH codeword.
  /// \param[out]    transport_block The decoded transport block, with packed (8 bits per entry) representation.
  /// \param[in,out] rm_buffer       A buffer for combining log-likelihood ratios from different retransmissions.
  /// \param[in]     notifier        Interface for notifying the completion of the TB processing.
  /// \param[in]     cfg             Decoder configuration parameters.
  /// \return  A \ref pusch_decoder_buffer, used to write softbits into the decoder.
  virtual pusch_decoder_buffer& new_data(span<uint8_t>           transport_block,
                                         unique_rx_buffer        rm_buffer,
                                         pusch_decoder_notifier& notifier,
                                         const configuration&    cfg) = 0;

  /// \brief Sets the number of UL-SCH codeword softbits expected by the PUSCH decoder.
  ///
  /// It allows the decoder to start decoding codeblocks before receiving the entire codeword. If it is called before
  /// \ref new_data or after decoding has started, it has no effect.
  ///
  /// \param[in] nof_softbits Number of codeword softbits, parameter \f$G^\textup{UL-SCH}\f$ in TS38.212 Section 6.2.7.
  virtual void set_nof_softbits(units::bits nof_softbits) = 0;
};

} // namespace srsran
