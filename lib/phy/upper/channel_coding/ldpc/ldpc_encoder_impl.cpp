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

#include "ldpc_encoder_impl.h"
#include "ldpc_luts_impl.h"

using namespace srsran;
using namespace srsran::ldpc;

void ldpc_encoder_impl::init(const codeblock_metadata::tb_common_metadata& cfg)
{
  uint8_t  pos  = get_lifting_size_position(cfg.lifting_size);
  unsigned skip = (cfg.base_graph == ldpc_base_graph_type::BG2) ? NOF_LIFTING_SIZES : 0;
  current_graph = &graph_array[skip + pos];
  bg_N_full     = current_graph->get_nof_BG_var_nodes_full();
  bg_N_short    = current_graph->get_nof_BG_var_nodes_short();
  bg_M          = current_graph->get_nof_BG_check_nodes();
  bg_K          = current_graph->get_nof_BG_info_nodes();
  assert(bg_K == bg_N_full - bg_M);
  lifting_size = static_cast<uint16_t>(cfg.lifting_size);

  select_strategy();
}

void ldpc_encoder_impl::encode(bit_buffer&                                   output,
                               const bit_buffer&                             input,
                               const codeblock_metadata::tb_common_metadata& cfg)
{
  init(cfg);

  uint16_t message_length    = bg_K * lifting_size;
  uint16_t max_output_length = bg_N_short * lifting_size;
  srsran_assert(input.size() == message_length,
                "Input size ({}) and message length ({}) must be equal",
                input.size(),
                message_length);
  srsran_assert(output.size() <= max_output_length,
                "Output size ({}) must be equal to or greater than {}",
                output.size(),
                max_output_length);

  // The minimum codeblock length is message_length + four times the lifting size
  // (that is, the length of the high-rate region).
  uint16_t min_codeblock_length = message_length + 4 * lifting_size;
  // The encoder works with at least min_codeblock_length bits. Recall that the encoder also shortens
  // the codeblock by 2 * lifting size before returning it as output.
  codeblock_length = std::max(output.size() + 2UL * lifting_size, static_cast<size_t>(min_codeblock_length));
  // The encoder works with a codeblock length that is a multiple of the lifting size.
  if (codeblock_length % lifting_size != 0) {
    codeblock_length = (codeblock_length / lifting_size + 1) * lifting_size;
  }

  load_input(input);

  preprocess_systematic_bits();

  encode_high_rate();

  encode_ext_region();

  write_codeblock(output);
}
