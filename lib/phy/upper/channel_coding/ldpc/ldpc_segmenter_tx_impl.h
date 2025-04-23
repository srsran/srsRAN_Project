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
/// \brief Tx-chain bufferless LDPC codeblock segmentation.

#pragma once

#include "ldpc_graph_impl.h"
#include "ldpc_segmenter_helpers.h"
#include "srsran/phy/upper/channel_coding/crc_calculator.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc_segmenter_buffer.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc_segmenter_tx.h"

namespace srsran {

/// \brief Bufferless implementation of Tx-chain LDPC segmentation.
class ldpc_segmenter_tx_impl : public ldpc_segmenter_tx, private ldpc_segmenter_buffer
{
public:
  /// CRC calculators used in shared channels.
  struct sch_crc {
    /// For short TB checksums.
    std::unique_ptr<crc_calculator> crc16;
    /// For long TB checksums.
    std::unique_ptr<crc_calculator> crc24A;
    /// For segment-specific checksums.
    std::unique_ptr<crc_calculator> crc24B;
  };

  /// \brief Initializes the internal Tx-chain LDPC segmenter blocks.
  /// \param[in] crc Unique pointers to CRC calculators.
  ldpc_segmenter_tx_impl(sch_crc& c) : crc_set({std::move(c.crc16), std::move(c.crc24A), std::move(c.crc24B)})
  {
    srsran_assert(crc_set.crc16, "Invalid CRC16 calculator.");
    srsran_assert(crc_set.crc24A, "Invalid CRC24A calculator.");
    srsran_assert(crc_set.crc24B, "Invalid CRC24B calculator.");
    srsran_assert(crc_set.crc16->get_generator_poly() == crc_generator_poly::CRC16,
                  "Not a CRC generator of type CRC16.");
    srsran_assert(crc_set.crc24A->get_generator_poly() == crc_generator_poly::CRC24A,
                  "Not a CRC generator of type CRC24A.");
    srsran_assert(crc_set.crc24B->get_generator_poly() == crc_generator_poly::CRC24B,
                  "Not a CRC generator of type CRC24B.");
  }

  // See interface for the documentation.
  ldpc_segmenter_buffer& new_transmission(span<const uint8_t> transport_block, const segmenter_config& cfg) override;

private:
  /// Segmentation parameters.
  segment_parameters params;

  /// CRC calculators for transport-block and segment-specific checksums.
  sch_crc crc_set;

  // See ldpc_segmenter_buffer interface for documentation.
  void read_codeblock(bit_buffer codeblock, span<const uint8_t> transport_block, unsigned cb_index) const override;

  // See ldpc_segmenter_buffer interface for documentation.
  codeblock_metadata get_cb_metadata(unsigned cb_index) const override;

  // See ldpc_segmenter_buffer interface for documentation.
  unsigned get_nof_codeblocks() const override { return params.nof_segments; }

  // See ldpc_segmenter_buffer interface for documentation.
  units::bits get_cb_info_bits(unsigned cb_index) const override;

  // See ldpc_segmenter_buffer interface for documentation.
  units::bits get_segment_length() const override { return params.segment_length; }

  // See ldpc_segmenter_buffer interface for documentation.
  units::bits get_cw_length() const override { return params.cw_length; }

  // See ldpc_segmenter_buffer interface for documentation.
  units::bits get_zero_pad() const override { return params.zero_pad; }

  // See ldpc_segmenter_buffer interface for documentation.
  units::bits get_tb_crc_bits() const override { return params.nof_tb_crc_bits; }

  // See ldpc_segmenter_buffer interface for documentation.
  units::bits get_nof_filler_bits() const override { return params.nof_filler_bits; }

  // See ldpc_segmenter_buffer interface for documentation.
  unsigned get_nof_short_segments() const override { return params.nof_short_segments; }

  // See ldpc_segmenter_buffer interface for documentation.
  unsigned get_rm_length(unsigned cb_index) const override;
};

} // namespace srsran
