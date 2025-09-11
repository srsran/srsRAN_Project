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

#pragma once

#include "srsran/phy/support/resource_grid_mapper.h"
#include "srsran/phy/upper/channel_processors/pdsch/pdsch_processor.h"

namespace srsran {

class ldpc_segmenter_buffer;

/// \brief Describes the PDSCH block processor interface.
///
/// The PDSCH block processor carries out CRC attachment, LDPC encoding, rate matching, scrambling, and modulation of a
/// contiguous number of codeblocks within a transmission.
class pdsch_block_processor
{
public:
  /// Default destructor.
  virtual ~pdsch_block_processor() = default;

  /// \brief Configures a new transmission.
  ///
  /// configure_new_transmission() configures the processor for a new transmission and returns a reference to a resource
  /// grid mapper buffer interface \ref resource_grid_mapper::symbol_buffer. After that, the processor will process
  /// codeblocks as the resource grid mapper requests resource elements to map into the resource grid.
  ///
  /// \param[in] data           Transport block data.
  /// \param[in] i_cw           Codeword index.
  /// \param[in] pdu            PDSCH transmission parameters.
  /// \param[in] segment_buffer LDPC segmenter output buffer interface.
  /// \param[in] start_i_cb     Index of the first CB in the batch.
  /// \param[in] cb_batch_len   Length of the CB batch.
  /// \return A reference to the complex symbol buffer of the resource element mapping interface.
  virtual resource_grid_mapper::symbol_buffer& configure_new_transmission(span<const uint8_t>           data,
                                                                          unsigned                      i_cw,
                                                                          const pdsch_processor::pdu_t& pdu,
                                                                          const ldpc_segmenter_buffer&  segment_buffer,
                                                                          unsigned                      start_i_cb,
                                                                          unsigned cb_batch_len) = 0;
};

} // namespace srsran
