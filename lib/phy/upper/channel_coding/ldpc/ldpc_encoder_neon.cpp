/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ldpc_encoder_neon.h"
#include "neon_support.h"

using namespace srsgnb;
using namespace srsgnb::ldpc;

// Recursively selects the proper strategy for the high-rate region by successively decreasing the value of the template
// parameter.
template <unsigned NODE_SIZE_NEON_PH>
ldpc_encoder_neon::strategy_method ldpc_encoder_neon::select_hr_strategy(ldpc_base_graph_type current_bg,
                                                                         uint8_t              current_ls_index,
                                                                         unsigned int         node_size_neon)
{
  if (node_size_neon != NODE_SIZE_NEON_PH) {
    return select_hr_strategy<NODE_SIZE_NEON_PH - 1>(current_bg, current_ls_index, node_size_neon);
  }

  if (current_bg == ldpc_base_graph_type::BG1) {
    if (current_ls_index == 6) {
      return &ldpc_encoder_neon::high_rate_bg1_i6_inner<NODE_SIZE_NEON_PH>;
    }
    // If current_lifting_index is not 6...
    return &ldpc_encoder_neon::high_rate_bg1_other_inner<NODE_SIZE_NEON_PH>;
  }
  // Else, if current_bg == BG2...
  if ((current_ls_index == 3) || (current_ls_index == 7)) {
    return &ldpc_encoder_neon::high_rate_bg2_i3_7_inner<NODE_SIZE_NEON_PH>;
  }
  // If current_lifting_index is neither 3 nor 7...
  return &ldpc_encoder_neon::high_rate_bg2_other_inner<NODE_SIZE_NEON_PH>;
}

// Ensures that the recursion stops when NODE_SIZE_NEON_PH == 1.
template <>
ldpc_encoder_neon::strategy_method ldpc_encoder_neon::select_hr_strategy<1>(ldpc_base_graph_type current_bg,
                                                                            uint8_t              current_ls_index,
                                                                            unsigned /*node_size_neon*/)
{
  if (current_bg == ldpc_base_graph_type::BG1) {
    if (current_ls_index == 6) {
      return &ldpc_encoder_neon::high_rate_bg1_i6_inner<1>;
    }
    // If current_lifting_index is not 6...
    return &ldpc_encoder_neon::high_rate_bg1_other_inner<1>;
  }
  // Else, if current_bg == BG2...
  if ((current_ls_index == 3) || (current_ls_index == 7)) {
    return &ldpc_encoder_neon::high_rate_bg2_i3_7_inner<1>;
  }
  // If current_lifting_index is neither 3 nor 7...
  return &ldpc_encoder_neon::high_rate_bg2_other_inner<1>;
}

static constexpr unsigned BG1_K = BG1_N_FULL - BG1_M;
static constexpr unsigned BG2_K = BG2_N_FULL - BG2_M;

// Recursively selects the proper strategy for the systematic bits by successively decreasing the value of the template
// parameter.
template <unsigned NODE_SIZE_NEON_PH>
ldpc_encoder_neon::strategy_method ldpc_encoder_neon::select_sys_bits_strategy(ldpc_base_graph_type current_bg,
                                                                               unsigned             node_size_neon)
{
  if (node_size_neon != NODE_SIZE_NEON_PH) {
    return select_sys_bits_strategy<NODE_SIZE_NEON_PH - 1>(current_bg, node_size_neon);
  }
  if (current_bg == ldpc_base_graph_type::BG1) {
    return &ldpc_encoder_neon::systematic_bits_inner<BG1_K, BG1_M, NODE_SIZE_NEON_PH>;
  }
  return &ldpc_encoder_neon::systematic_bits_inner<BG2_K, BG2_M, NODE_SIZE_NEON_PH>;
}

// Ensures that the recursion stops when NODE_SIZE_NEON_PH == 1.
template <>
ldpc_encoder_neon::strategy_method ldpc_encoder_neon::select_sys_bits_strategy<1>(ldpc_base_graph_type current_bg,
                                                                                  unsigned /*node_size_neon*/)
{
  if (current_bg == ldpc_base_graph_type::BG1) {
    return &ldpc_encoder_neon::systematic_bits_inner<BG1_K, BG1_M, 1>;
  }
  return &ldpc_encoder_neon::systematic_bits_inner<BG2_K, BG2_M, 1>;
}

// Recursively selects the proper strategy for the extended region by successively decreasing the value of the template
// parameter.
template <unsigned NODE_SIZE_NEON_PH>
ldpc_encoder_neon::strategy_method ldpc_encoder_neon::select_ext_strategy(unsigned node_size_neon)
{
  if (node_size_neon == NODE_SIZE_NEON_PH) {
    return &ldpc_encoder_neon::ext_region_inner<NODE_SIZE_NEON_PH>;
  }
  return select_ext_strategy<NODE_SIZE_NEON_PH - 1>(node_size_neon);
}

// Ensures that the recursion stops when NODE_SIZE_NEON_PH == 1.
template <>
ldpc_encoder_neon::strategy_method ldpc_encoder_neon::select_ext_strategy<1>(unsigned /*node_size_neon*/)
{
  return &ldpc_encoder_neon::ext_region_inner<1>;
}

void ldpc_encoder_neon::select_strategy()
{
  ldpc_base_graph_type current_bg       = current_graph->get_base_graph();
  uint8_t              current_ls_index = current_graph->get_lifting_index();

  // Each BG node contains LS bits, which are stored in node_size_neon NEON vectors.
  node_size_neon = divide_ceil(lifting_size, NEON_SIZE_BYTE);

  systematic_bits = select_sys_bits_strategy<MAX_NODE_SIZE_NEON>(current_bg, node_size_neon);
  high_rate       = select_hr_strategy<MAX_NODE_SIZE_NEON>(current_bg, current_ls_index, node_size_neon);
  ext_region      = select_ext_strategy<MAX_NODE_SIZE_NEON>(node_size_neon);
}

void ldpc_encoder_neon::load_input(span<const uint8_t> in)
{
  unsigned i_input        = 0;
  unsigned i_neon         = 0;
  unsigned node_size_byte = node_size_neon * NEON_SIZE_BYTE;
  unsigned tail_bytes     = node_size_byte - lifting_size;
  for (unsigned i_node = 0; i_node != bg_K; ++i_node) {
    std::memcpy(codeblock.data_at(i_neon), in.data() + i_input, lifting_size);
    std::memset(codeblock.data_at(i_neon, lifting_size), 0, tail_bytes);
    i_neon += node_size_neon;
    i_input += lifting_size;
  }
}

template <unsigned BG_K_PH, unsigned BG_M_PH, unsigned NODE_SIZE_NEON_PH>
void ldpc_encoder_neon::systematic_bits_inner()
{
  // Resize auxiliary buffer.
  auxiliary_used_size = (codeblock_length / lifting_size - BG_K_PH) * NODE_SIZE_NEON_PH;

  std::memset(auxiliary.data_at(0), 0, auxiliary_used_size * NEON_SIZE_BYTE);

  // For each BG information node...
  for (unsigned k = 0, i_blk = 0; k != BG_K_PH; ++k, i_blk += NODE_SIZE_NEON_PH) {
    // and for each BG check node...
    for (unsigned m = 0, i_aux = 0; m != BG_M_PH; ++m) {
      unsigned node_shift = current_graph->get_lifted_node(m, k);
      if (node_shift == NO_EDGE) {
        i_aux += NODE_SIZE_NEON_PH;
        continue;
      }
      rotate_node_right(rotated_node.data_at(0, 0), codeblock.data_at(i_blk, 0), node_shift, lifting_size);
      for (unsigned j = 0; j != NODE_SIZE_NEON_PH; ++j) {
        int8x16_t tmp = vandq_s8(rotated_node.get_at(j), vdupq_n_s8(1));
        auxiliary.set_at(i_aux, veorq_s8(auxiliary.get_at(i_aux), tmp));
        ++i_aux;
      }
    }
  }
}

template <unsigned NODE_SIZE_NEON_PH>
void ldpc_encoder_neon::high_rate_bg1_i6_inner()
{
  unsigned skip0         = bg_K * NODE_SIZE_NEON_PH;
  unsigned skip1         = (bg_K + 1) * NODE_SIZE_NEON_PH;
  unsigned skip2         = (bg_K + 2) * NODE_SIZE_NEON_PH;
  unsigned skip3         = (bg_K + 3) * NODE_SIZE_NEON_PH;
  unsigned node_size_x_2 = 2 * NODE_SIZE_NEON_PH;
  unsigned node_size_x_3 = 3 * NODE_SIZE_NEON_PH;

  // First chunk of parity bits.
  for (unsigned j = 0; j != NODE_SIZE_NEON_PH; ++j) {
    int8x16_t tmp = veorq_s8(auxiliary.get_at(j), auxiliary.get_at(NODE_SIZE_NEON_PH + j));
    tmp           = veorq_s8(tmp, auxiliary.get_at(node_size_x_2 + j));
    tmp           = veorq_s8(tmp, auxiliary.get_at(node_size_x_3 + j));
    rotated_node.set_at(j, tmp);
  }

  rotate_node_left(codeblock.data_at(skip0, 0), rotated_node.data_at(0, 0), 105 % lifting_size, lifting_size);

  for (unsigned j = 0; j != NODE_SIZE_NEON_PH; ++j) {
    int8x16_t block0 = codeblock.get_at(skip0 + j);
    // Second chunk of parity bits.
    codeblock.set_at(skip1 + j, veorq_s8(auxiliary.get_at(j), block0));
    // Fourth chunk of parity bits.
    int8x16_t block3 = veorq_s8(auxiliary.get_at(node_size_x_3 + j), block0);
    codeblock.set_at(skip3 + j, block3);
    // Third chunk of parity bits.
    codeblock.set_at(skip2 + j, veorq_s8(auxiliary.get_at(node_size_x_2 + j), block3));
  }
}

template <unsigned NODE_SIZE_NEON_PH>
void ldpc_encoder_neon::high_rate_bg1_other_inner()
{
  unsigned skip0         = bg_K * NODE_SIZE_NEON_PH;
  unsigned skip1         = (bg_K + 1) * NODE_SIZE_NEON_PH;
  unsigned skip2         = (bg_K + 2) * NODE_SIZE_NEON_PH;
  unsigned skip3         = (bg_K + 3) * NODE_SIZE_NEON_PH;
  unsigned node_size_x_2 = 2 * NODE_SIZE_NEON_PH;
  unsigned node_size_x_3 = 3 * NODE_SIZE_NEON_PH;

  // First chunk of parity bits.
  for (unsigned j = 0; j != NODE_SIZE_NEON_PH; ++j) {
    int8x16_t block0 = veorq_s8(auxiliary.get_at(j), auxiliary.get_at(NODE_SIZE_NEON_PH + j));
    block0           = veorq_s8(block0, auxiliary.get_at(node_size_x_2 + j));
    block0           = veorq_s8(block0, auxiliary.get_at(node_size_x_3 + j));
    codeblock.set_at(skip0 + j, block0);
  }

  rotate_node_right(rotated_node.data_at(0, 0), codeblock.data_at(skip0, 0), 1, lifting_size);

  for (unsigned j = 0; j != NODE_SIZE_NEON_PH; ++j) {
    int8x16_t rotated_j = rotated_node.get_at(j);
    // Second chunk of parity bits.
    codeblock.set_at(skip1 + j, veorq_s8(auxiliary.get_at(j), rotated_j));
    // Fourth chunk of parity bits.
    int8x16_t block3 = veorq_s8(auxiliary.get_at(node_size_x_3 + j), rotated_j);
    codeblock.set_at(skip3 + j, block3);
    // Third chunk of parity bits.
    codeblock.set_at(skip2 + j, veorq_s8(auxiliary.get_at(node_size_x_2 + j), block3));
  }
}

template <unsigned NODE_SIZE_NEON_PH>
void ldpc_encoder_neon::high_rate_bg2_i3_7_inner()
{
  unsigned skip0         = bg_K * NODE_SIZE_NEON_PH;
  unsigned skip1         = (bg_K + 1) * NODE_SIZE_NEON_PH;
  unsigned skip2         = (bg_K + 2) * NODE_SIZE_NEON_PH;
  unsigned skip3         = (bg_K + 3) * NODE_SIZE_NEON_PH;
  unsigned node_size_x_2 = 2 * NODE_SIZE_NEON_PH;
  unsigned node_size_x_3 = 3 * NODE_SIZE_NEON_PH;

  // First chunk of parity bits.
  for (unsigned j = 0; j != NODE_SIZE_NEON_PH; ++j) {
    int8x16_t block0 = veorq_s8(auxiliary.get_at(j), auxiliary.get_at(NODE_SIZE_NEON_PH + j));
    block0           = veorq_s8(block0, auxiliary.get_at(node_size_x_2 + j));
    block0           = veorq_s8(block0, auxiliary.get_at(node_size_x_3 + j));
    codeblock.set_at(skip0 + j, block0);
  }

  rotate_node_right(rotated_node.data_at(0, 0), codeblock.data_at(skip0, 0), 1, lifting_size);

  for (unsigned j = 0; j != NODE_SIZE_NEON_PH; ++j) {
    int8x16_t rotated_j = rotated_node.get_at(j);
    // Second chunk of parity bits.
    int8x16_t block_1 = veorq_s8(auxiliary.get_at(j), rotated_j);
    codeblock.set_at(skip1 + j, block_1);
    // third chunk of parity bits
    codeblock.set_at(skip2 + j, veorq_s8(auxiliary.get_at(NODE_SIZE_NEON_PH + j), block_1));
    // fourth chunk of parity bits
    codeblock.set_at(skip3 + j, veorq_s8(auxiliary.get_at(node_size_x_3 + j), rotated_j));
  }
}

template <unsigned NODE_SIZE_NEON_PH>
void ldpc_encoder_neon::high_rate_bg2_other_inner()
{
  unsigned skip0         = bg_K * NODE_SIZE_NEON_PH;
  unsigned skip1         = (bg_K + 1) * NODE_SIZE_NEON_PH;
  unsigned skip2         = (bg_K + 2) * NODE_SIZE_NEON_PH;
  unsigned skip3         = (bg_K + 3) * NODE_SIZE_NEON_PH;
  unsigned node_size_x_2 = 2 * NODE_SIZE_NEON_PH;
  unsigned node_size_x_3 = 3 * NODE_SIZE_NEON_PH;

  // First chunk of parity bits.
  for (unsigned j = 0; j != NODE_SIZE_NEON_PH; ++j) {
    int8x16_t rotated_j = veorq_s8(auxiliary.get_at(j), auxiliary.get_at(NODE_SIZE_NEON_PH + j));
    rotated_j           = veorq_s8(rotated_j, auxiliary.get_at(node_size_x_2 + j));
    rotated_j           = veorq_s8(rotated_j, auxiliary.get_at(node_size_x_3 + j));
    rotated_node.set_at(j, rotated_j);
  }

  rotate_node_left(codeblock.data_at(skip0, 0), rotated_node.data_at(0, 0), 1, lifting_size);

  for (unsigned j = 0; j != NODE_SIZE_NEON_PH; ++j) {
    int8x16_t block_0 = codeblock.get_at(skip0 + j);
    // Second chunk of parity bits.
    int8x16_t block_1 = veorq_s8(auxiliary.get_at(j), block_0);
    codeblock.set_at(skip1 + j, block_1);
    // Third chunk of parity bits.
    codeblock.set_at(skip2 + j, veorq_s8(auxiliary.get_at(NODE_SIZE_NEON_PH + j), block_1));
    // Fourth chunk of parity bits.
    codeblock.set_at(skip3 + j, veorq_s8(auxiliary.get_at(node_size_x_3 + j), block_0));
  }
}

template <unsigned NODE_SIZE_NEON_PH>
void ldpc_encoder_neon::ext_region_inner()
{
  // We only compute the variable nodes needed to fill the codeword.
  // Also, recall the high-rate region has length (bg_K + 4) * lifting_size.
  unsigned nof_layers = codeblock_length / lifting_size - bg_K;

  // Encode the extended region.
  unsigned skip     = (bg_K + 4) * NODE_SIZE_NEON_PH;
  unsigned skip_aux = 4 * NODE_SIZE_NEON_PH;
  for (unsigned m = 4; m != nof_layers; ++m) {
    // The systematic part has already been computed.
    for (unsigned j = 0; j != NODE_SIZE_NEON_PH; ++j) {
      codeblock.set_at(skip + j, auxiliary.get_at(skip_aux + j));
    }

    // Sum the contribution due to the high-rate region, with the proper circular shifts.
    for (unsigned k = 0; k != 4; ++k) {
      unsigned node_shift = current_graph->get_lifted_node(m, bg_K + k);

      if (node_shift == NO_EDGE) {
        continue;
      }
      rotate_node_right(
          rotated_node.data_at(0, 0), codeblock.data_at((bg_K + k) * NODE_SIZE_NEON_PH, 0), node_shift, lifting_size);
      for (unsigned j = 0; j != NODE_SIZE_NEON_PH; ++j) {
        codeblock.set_at(skip + j, veorq_s8(codeblock.get_at(skip + j), rotated_node.get_at(j)));
      }
    }
    skip += NODE_SIZE_NEON_PH;
    skip_aux += NODE_SIZE_NEON_PH;
  }
}

void ldpc_encoder_neon::write_codeblock(span<uint8_t> out)
{
  unsigned nof_nodes = codeblock_length / lifting_size;

  // The first two blocks are shortened and the last node is not considered, since it can be incomplete.
  unsigned i_out = 0;
  unsigned i_in  = 2 * node_size_neon;
  for (unsigned i_node = 2, max_i_node = nof_nodes - 1; i_node != max_i_node; ++i_node) {
    std::memcpy(out.data() + i_out, codeblock.data_at(i_in), lifting_size);
    i_out += lifting_size;
    i_in += node_size_neon;
  }

  // Take care of the last node.
  unsigned remainder = out.size() - i_out;
  std::memcpy(out.data() + i_out, codeblock.data_at(i_in), remainder);
}
