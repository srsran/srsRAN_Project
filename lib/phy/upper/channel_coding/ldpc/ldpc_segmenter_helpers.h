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
/// \brief Helper functions for LDPC segmenter implementations.

#pragma once

#include "srsran/phy/upper/channel_coding/ldpc/ldpc.h"
#include "srsran/phy/upper/codeblock_metadata.h"

namespace srsran {

struct segment_parameters {
  /// \name Attributes relative to TS38.212 Section 5.2.2.
  ///@{
  /// Final length of a segment (corresponds to \f$K\f$).
  units::bits segment_length{0};
  /// Number of bits in the transport block (corresponds to \f$B\f$).
  units::bits nof_tb_bits_in{0};
  /// Augmented number of bits in the transport block, including new CRCs (corresponds to \f$B'\f$).
  units::bits nof_tb_bits_out{0};
  /// Number of segments resulting from the transport block (corresponds to \f$C\f$).
  unsigned nof_segments = 0;
  ///@}
  /// \name Attributes relative to TS38.212 Section 5.4.2.1.
  ///@{
  /// Number of symbols per transmission layer (corresponds to \f$G / (N_L Q_m)\f$).
  unsigned nof_symbols_per_layer = 0;
  /// \brief Number of segments of short rate-matched length (corresponds to \f$C - \bigr(\bigl(G / (N_L Q_m)\bigr)
  /// \bmod C\bigr)\f$).
  unsigned nof_short_segments = 0;
  ///@}
  /// Base graph used for encoding/decoding the current transport block.
  ldpc_base_graph_type base_graph = ldpc_base_graph_type::BG1;
  /// Lifting size used for encoding/decoding the current transport block.
  unsigned lifting_size = 0;
  /// Total codeword length.
  units::bits cw_length;
  /// Number of filler bits.
  units::bits nof_filler_bits;
  /// Number of segment-specific CRC bits.
  units::bits nof_crc_bits;
  // Number of information bits that is assigned to a segment.
  units::bits cb_info_bits;
  // Number of information bits that is assigned to the last segment.
  units::bits cb_info_bits_last;
  // Number of zero-pad bits.
  units::bits zero_pad;
  /// Number of TB-specific CRC bits.
  units::bits nof_tb_crc_bits;
  /// Array of codeblock starting offset within the codeword.
  std::array<unsigned, MAX_NOF_SEGMENTS> cw_offset;
  /// Array of segment starting offset within the transport block.
  std::array<unsigned, MAX_NOF_SEGMENTS> tb_offset;
  /// Array of codeblock metadata structure.
  std::array<codeblock_metadata, MAX_NOF_SEGMENTS> cb_metadata;
};

/// Computes the length of the rate-matched codeblock corresponding to each segment, as per TS38.212
/// Section 5.4.2.1.
inline unsigned
compute_rm_length(const segment_parameters& seg_param, modulation_scheme mod, unsigned nof_layers, unsigned i_seg)
{
  unsigned tmp = 0;
  if (i_seg < seg_param.nof_short_segments) {
    // For unsigned, division then floor is the same as integer division.
    tmp = seg_param.nof_symbols_per_layer / seg_param.nof_segments;
  } else {
    tmp = divide_ceil(seg_param.nof_symbols_per_layer, seg_param.nof_segments);
  }
  return tmp * nof_layers * get_bits_per_symbol(mod);
}

/// Generates a codeblock metadata structure for the current segment configuration.
inline codeblock_metadata
generate_cb_metadata(const segment_parameters& seg_param, const segmenter_config& cfg, unsigned i_seg)
{
  codeblock_metadata tmp_description;

  tmp_description.tb_common.base_graph   = seg_param.base_graph;
  tmp_description.tb_common.lifting_size = static_cast<ldpc::lifting_size_t>(seg_param.lifting_size);
  tmp_description.tb_common.rv           = cfg.rv;
  tmp_description.tb_common.mod          = cfg.mod;
  tmp_description.tb_common.Nref         = cfg.Nref;
  tmp_description.tb_common.cw_length    = seg_param.cw_length.value();

  unsigned rm_length = compute_rm_length(seg_param, cfg.mod, cfg.nof_layers, i_seg);

  tmp_description.cb_specific.full_length =
      ldpc::compute_full_codeblock_size(seg_param.base_graph, seg_param.segment_length).value();
  tmp_description.cb_specific.nof_filler_bits = seg_param.nof_filler_bits.value();
  tmp_description.cb_specific.rm_length       = rm_length;
  tmp_description.cb_specific.cw_offset       = seg_param.cw_offset[i_seg];
  tmp_description.cb_specific.nof_crc_bits =
      (seg_param.nof_segments == 1) ? seg_param.nof_tb_crc_bits.value() : seg_param.nof_crc_bits.value();

  return tmp_description;
}

} // namespace srsran
