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

/// \file
/// \brief LDPC codeblock segmentation (transmit side).
#pragma once

#include "srsran/adt/span.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc.h"
#include "srsran/phy/upper/codeblock_metadata.h"

namespace srsran {

class ldpc_segmenter_buffer;

/// Carries out the segmentation of a transport block into a number of codeblocks.
class ldpc_segmenter_tx
{
public:
  /// Default destructor.
  virtual ~ldpc_segmenter_tx() = default;

  /// \brief Performs transport block segmentation on a codeblock basis without using an intermediate buffer.
  ///
  /// If needed, a CRC is attached to the generated segment, according to TS38.212 Section 5.2.2. Moreover, the
  /// transport block CRC is attached to the last segment, as per TS38.212 Section 7.2.1. The function also computes
  /// relevant segment metadata (e.g., coded and rate-matched length) according to TS38.212 Section 5.4.2.1.
  ///
  /// \param[in]  transport_block       The transport block to segment_tx (packed, one byte per entry).
  /// \param[in]  cfg                   Parameters affecting splitting and codeblock metadata.
  /// \return A reference to the LDPC segmenter buffer.
  virtual const ldpc_segmenter_buffer& new_transmission(span<const uint8_t>     transport_block,
                                                        const segmenter_config& cfg) = 0;
};

} // namespace srsran
