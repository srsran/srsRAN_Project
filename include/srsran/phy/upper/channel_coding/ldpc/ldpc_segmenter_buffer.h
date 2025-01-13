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

#include "srsran/adt/span.h"
#include "srsran/adt/static_vector.h"
#include "srsran/phy/upper/codeblock_metadata.h"

namespace srsran {

/// \brief LDPC segmenter output buffer interface.
///
/// This interface allows the LDPC segmenter to provide the CBs to the LDPC encoder without using an intermediate
/// buffer.
class ldpc_segmenter_buffer
{
public:
  /// Default destructor.
  virtual ~ldpc_segmenter_buffer() = default;

  /// \brief  Gets the codeblock metadata structure for a given codeblock.
  ///
  /// \param[in] cb_index        Codeblock index (within the TB).
  /// \return Codeblock metadata structure for the codeblock with index \c cb_index.
  virtual codeblock_metadata get_cb_metadata(unsigned cb_index) const = 0;

  /// Gets the number of codeblocks resulting from TB segmentation.
  virtual unsigned get_nof_codeblocks() const = 0;

  /// Gets the number of of segments that will have a short rate-matched length.
  virtual unsigned get_nof_short_segments() const = 0;

  /// \brief  Gets the number of information bits for a given segment.
  ///
  /// \param[in] cb_index        Segment index.
  /// \return Number of information bits for the segment with index \c cb_index.
  virtual units::bits get_cb_info_bits(unsigned cb_index) const = 0;

  /// Gets the segment length resulting from TB segmentation.
  virtual units::bits get_segment_length() const = 0;

  /// Gets the codeword length.
  virtual units::bits get_cw_length() const = 0;

  /// Gets the zero padding length.
  virtual units::bits get_zero_pad() const = 0;

  /// Gets the number of TB CRC bits.
  virtual units::bits get_tb_crc_bits() const = 0;

  /// Gets the number of filler bits.
  virtual units::bits get_nof_filler_bits() const = 0;

  /// Gets the codeblock length.
  virtual unsigned get_rm_length(unsigned cb_index) const = 0;

  /// \brief Reads the segmented codeblock data, including the CB CRC, for the CB at index \c cb_index.
  ///
  /// \param[out] codeblock       Read destination.
  /// \param[in]  transport_block Read source.
  /// \param[in]  cb_index        Codeblock index (within the TB).
  virtual void read_codeblock(bit_buffer codeblock, span<const uint8_t> transport_block, unsigned cb_index) const = 0;
};

} // namespace srsran
