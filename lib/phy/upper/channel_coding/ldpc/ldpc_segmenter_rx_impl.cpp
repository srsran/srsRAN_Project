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

#include "ldpc_segmenter_rx_impl.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc.h"
#include "srsran/phy/upper/codeblock_metadata.h"
#include "srsran/srsvec/bit.h"
#include "srsran/srsvec/copy.h"
#include "srsran/support/math/math_utils.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;
using namespace srsran::ldpc;

/// Length of the CRC checksum added to the segments.
static constexpr units::bits SEG_CRC_LENGTH{24};

static void check_inputs_rx(span<const log_likelihood_ratio> codeword_llrs, const segmenter_config& cfg)
{
  srsran_assert(!codeword_llrs.empty(), "Argument transport_block should not be empty.");
  srsran_assert(codeword_llrs.size() == cfg.nof_ch_symbols * get_bits_per_symbol(cfg.mod),
                "Wrong number of LLRs {} (expected {}).",
                codeword_llrs.size(),
                cfg.nof_ch_symbols * get_bits_per_symbol(cfg.mod));

  srsran_assert((cfg.rv >= 0) && (cfg.rv <= 3), "Invalid redundancy version.");

  srsran_assert((cfg.nof_layers >= 1) && (cfg.nof_layers <= 4), "Invalid number of layers.");

  srsran_assert(cfg.nof_ch_symbols % (cfg.nof_layers) == 0,
                "The number of channel symbols should be a multiple of the product between the number of layers.");
}

void ldpc_segmenter_rx_impl::segment(static_vector<described_rx_codeblock, MAX_NOF_SEGMENTS>& described_codeblocks,
                                     span<const log_likelihood_ratio>                         codeword_llrs,
                                     unsigned                                                 tbs,
                                     const segmenter_config&                                  cfg)
{
  check_inputs_rx(codeword_llrs, cfg);

  using namespace units::literals;

  params.base_graph = cfg.base_graph;

  units::bits tbs_bits(tbs);
  params.nof_tb_crc_bits = compute_tb_crc_size(tbs_bits);

  params.nof_tb_bits_in = tbs_bits + params.nof_tb_crc_bits;

  params.nof_segments    = ldpc::compute_nof_codeblocks(tbs_bits, params.base_graph);
  params.nof_tb_bits_out = params.nof_tb_bits_in;
  if (params.nof_segments > 1) {
    params.nof_tb_bits_out += units::bits(params.nof_segments * SEG_CRC_LENGTH.value());
  }
  params.lifting_size   = compute_lifting_size(tbs_bits, params.base_graph, params.nof_segments);
  params.segment_length = compute_codeblock_size(params.base_graph, params.lifting_size);

  params.nof_crc_bits = (params.nof_segments > 1) ? SEG_CRC_LENGTH : 0_bits;

  // Compute the maximum number of information bits that can be assigned to a segment.
  units::bits max_info_bits =
      units::bits(divide_ceil(params.nof_tb_bits_out.value(), params.nof_segments)) - params.nof_crc_bits;

  // Number of channel symbols assigned to a transmission layer.
  params.nof_symbols_per_layer = cfg.nof_ch_symbols / cfg.nof_layers;
  // Number of segments that will have a short rate-matched length. In TS38.212 Section 5.4.2.1, these correspond to
  // codeblocks whose length E_r is computed by rounding down - floor. For the remaining codewords, the length is
  // rounded up.
  params.nof_short_segments = params.nof_segments - (params.nof_symbols_per_layer % params.nof_segments);

  // Codeword length (after concatenation of codeblocks).
  params.cw_length = static_cast<units::bits>(codeword_llrs.size());

  unsigned cw_offset = 0;
  for (unsigned i_segment = 0; i_segment != params.nof_segments; ++i_segment) {
    params.cw_offset[i_segment] = cw_offset;
    params.nof_filler_bits      = params.segment_length - (max_info_bits + params.nof_crc_bits);

    codeblock_metadata tmp_description = generate_cb_metadata(params, cfg, i_segment);

    unsigned rm_length = tmp_description.cb_specific.rm_length;
    described_codeblocks.push_back({codeword_llrs.subspan(cw_offset, rm_length), tmp_description});
    cw_offset += rm_length;
  }
  // After accumulating all codeblock rate-matched lengths, cw_offset should be the same as cw_length.
  srsran_assert(params.cw_length.value() == cw_offset, "Cw offset must be equal to the cw length");
}
