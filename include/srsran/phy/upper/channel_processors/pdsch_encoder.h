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
/// \brief PDSCH encoder interface.
///
/// PDSCH encoder black-box interface, with a single method that takes a transport block as input and returns a
/// codeword.

#pragma once

#include "srsran/adt/span.h"
#include "srsran/phy/upper/codeblock_metadata.h"

namespace srsran {

class tx_buffer;

/// \brief PDSCH encoder, as per TS38.212 Section 7.2.
///
/// For a PHY Downlink Shared Data CHannel (PDSCH) transport block, it returns the single codeword obtained after CRC
/// attachment, segmentation, codeblock encoding, rate matching and, finally, codeblock concatenation.
class pdsch_encoder
{
public:
  /// Default destructor.
  virtual ~pdsch_encoder() = default;

  /// Collects PDSCH encoder parameters.
  struct configuration {
    /// Code base graph.
    ldpc_base_graph_type base_graph;
    /// Redundancy version, values in {0, 1, 2, 3}.
    unsigned rv;
    /// Modulation scheme.
    modulation_scheme mod;
    /// \brief Limited buffer rate matching length in bits, as per TS38.212 Section 5.4.2.
    /// \note Set to zero for unlimited buffer length.
    unsigned Nref;
    /// Number of transmission layers the transport block is mapped onto.
    unsigned nof_layers;
    /// Number of channel symbols (i.e., REs) the transport block is mapped to.
    unsigned nof_ch_symbols;
  };

  /// \brief Full PDSCH encoding.
  ///
  /// General encoding method that includes the four steps of CRC attachment (see TS38.212 Section 7.2.1), transport
  /// block segmentation (see TS38.212 Section 7.2.3), codeblock encoding (see TS38.212 Section 7.2.4) and rate matching
  /// (see TS38.212 Section 7.2.5), and codeblock concatenation (see TS38.212 Section 7.2.6) into the PDSCH codeword.
  /// \param[out]    codeword         Final PDSCH codeword.
  /// \param[in]     transport_block  Transport block to be transmitted.
  /// \param[in]     cfg              PDSCH configuration parameters.
  virtual void encode(span<uint8_t> codeword, span<const uint8_t> transport_block, const configuration& cfg) = 0;
};

} // namespace srsran
