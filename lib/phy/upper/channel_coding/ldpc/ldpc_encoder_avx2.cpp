/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ldpc_encoder_avx2.h"
#include "avx2_support.h"

using namespace srsgnb;
using namespace srsgnb::ldpc;

/// Rotates the contents of a node towards the left by \c steps chars, that is the \c steps * 8 least significant bits
/// become the most significant ones - for long lifting sizes.
/// \param[out] out       Pointer to the first AVX2 block of the output rotated node.
/// \param[in]  in        Pointer to the first AVX2 block of the input node to rotate.
/// \param[in]  steps     The order of the rotation as a number of chars.
/// \param[in]  ls        The size of the node (lifting size).
static void rotate_node_left(__m256i* out, const __m256i* in, unsigned steps, unsigned ls);

/// Rotates the contents of a node towards the right by \c steps chars, that is the \c steps * 8 most significant bits
/// become the least significant ones - for long lifting sizes.
/// \param[out] out       Pointer to the first AVX2 block of the output rotated node.
/// \param[in]  in        Pointer to the first AVX2 block of the input node to rotate.
/// \param[in]  steps     The order of the rotation as a number of chars.
/// \param[in]  ls        The size of the node (lifting size).
static void rotate_node_right(__m256i* out, const __m256i* in, unsigned steps, unsigned ls);

ldpc_encoder_avx2::strategy_method ldpc_encoder_avx2::select_hr_strategy(const ldpc_base_graph_type current_bg,
                                                                         const uint8_t              current_ls_index,
                                                                         const unsigned             node_size_avx2)
{
  if (current_bg == ldpc_base_graph_type::BG1) {
    if (current_ls_index == 6) {
      switch (node_size_avx2) {
        case 1:
          return &ldpc_encoder_avx2::high_rate_bg1_i6_inner<1>;
        case 2:
          return &ldpc_encoder_avx2::high_rate_bg1_i6_inner<2>;
        case 3:
          return &ldpc_encoder_avx2::high_rate_bg1_i6_inner<3>;
        case 4:
          return &ldpc_encoder_avx2::high_rate_bg1_i6_inner<4>;
        case 5:
          return &ldpc_encoder_avx2::high_rate_bg1_i6_inner<5>;
        case 6:
          return &ldpc_encoder_avx2::high_rate_bg1_i6_inner<6>;
        case 7:
          return &ldpc_encoder_avx2::high_rate_bg1_i6_inner<7>;
        case 8:
          return &ldpc_encoder_avx2::high_rate_bg1_i6_inner<8>;
        case 9:
          return &ldpc_encoder_avx2::high_rate_bg1_i6_inner<9>;
        case 10:
          return &ldpc_encoder_avx2::high_rate_bg1_i6_inner<10>;
        case 11:
          return &ldpc_encoder_avx2::high_rate_bg1_i6_inner<11>;
        case 12:
          return &ldpc_encoder_avx2::high_rate_bg1_i6_inner<12>;
        default:
          srsgnb_assert(false, "The node size in AVX2 registers should be between 1 and 12.");
      }
    }
    // If current_lifting_index is not 6...
    switch (node_size_avx2) {
      case 1:
        return &ldpc_encoder_avx2::high_rate_bg1_other_inner<1>;
      case 2:
        return &ldpc_encoder_avx2::high_rate_bg1_other_inner<2>;
      case 3:
        return &ldpc_encoder_avx2::high_rate_bg1_other_inner<3>;
      case 4:
        return &ldpc_encoder_avx2::high_rate_bg1_other_inner<4>;
      case 5:
        return &ldpc_encoder_avx2::high_rate_bg1_other_inner<5>;
      case 6:
        return &ldpc_encoder_avx2::high_rate_bg1_other_inner<6>;
      case 7:
        return &ldpc_encoder_avx2::high_rate_bg1_other_inner<7>;
      case 8:
        return &ldpc_encoder_avx2::high_rate_bg1_other_inner<8>;
      case 9:
        return &ldpc_encoder_avx2::high_rate_bg1_other_inner<9>;
      case 10:
        return &ldpc_encoder_avx2::high_rate_bg1_other_inner<10>;
      case 11:
        return &ldpc_encoder_avx2::high_rate_bg1_other_inner<11>;
      case 12:
        return &ldpc_encoder_avx2::high_rate_bg1_other_inner<12>;
      default:
        srsgnb_assert(false, "The node size in AVX2 registers should be between 1 and 12.");
    }
  }
  // Else, if current_bg == BG2...
  if ((current_ls_index == 3) || (current_ls_index == 7)) {
    switch (node_size_avx2) {
      case 1:
        return &ldpc_encoder_avx2::high_rate_bg2_i3_7_inner<1>;
      case 2:
        return &ldpc_encoder_avx2::high_rate_bg2_i3_7_inner<2>;
      case 3:
        return &ldpc_encoder_avx2::high_rate_bg2_i3_7_inner<3>;
      case 4:
        return &ldpc_encoder_avx2::high_rate_bg2_i3_7_inner<4>;
      case 5:
        return &ldpc_encoder_avx2::high_rate_bg2_i3_7_inner<5>;
      case 6:
        return &ldpc_encoder_avx2::high_rate_bg2_i3_7_inner<6>;
      case 7:
        return &ldpc_encoder_avx2::high_rate_bg2_i3_7_inner<7>;
      case 8:
        return &ldpc_encoder_avx2::high_rate_bg2_i3_7_inner<8>;
      case 9:
        return &ldpc_encoder_avx2::high_rate_bg2_i3_7_inner<9>;
      case 10:
        return &ldpc_encoder_avx2::high_rate_bg2_i3_7_inner<10>;
      case 11:
        return &ldpc_encoder_avx2::high_rate_bg2_i3_7_inner<11>;
      case 12:
        return &ldpc_encoder_avx2::high_rate_bg2_i3_7_inner<12>;
      default:
        srsgnb_assert(false, "The node size in AVX2 registers should be between 1 and 12.");
    }
  }
  // If current_lifting_index is neither 3 nor 7...
  switch (node_size_avx2) {
    case 1:
      return &ldpc_encoder_avx2::high_rate_bg2_other_inner<1>;
    case 2:
      return &ldpc_encoder_avx2::high_rate_bg2_other_inner<2>;
    case 3:
      return &ldpc_encoder_avx2::high_rate_bg2_other_inner<3>;
    case 4:
      return &ldpc_encoder_avx2::high_rate_bg2_other_inner<4>;
    case 5:
      return &ldpc_encoder_avx2::high_rate_bg2_other_inner<5>;
    case 6:
      return &ldpc_encoder_avx2::high_rate_bg2_other_inner<6>;
    case 7:
      return &ldpc_encoder_avx2::high_rate_bg2_other_inner<7>;
    case 8:
      return &ldpc_encoder_avx2::high_rate_bg2_other_inner<8>;
    case 9:
      return &ldpc_encoder_avx2::high_rate_bg2_other_inner<9>;
    case 10:
      return &ldpc_encoder_avx2::high_rate_bg2_other_inner<10>;
    case 11:
      return &ldpc_encoder_avx2::high_rate_bg2_other_inner<11>;
    case 12:
      return &ldpc_encoder_avx2::high_rate_bg2_other_inner<12>;
    default:
      srsgnb_assert(false, "The node size in AVX2 registers should be between 1 and 12.");
  }
  return nullptr;
}

ldpc_encoder_avx2::strategy_method ldpc_encoder_avx2::select_sys_bits_strategy(ldpc_base_graph_type current_bg,
                                                                               unsigned             node_size_avx2)
{
  if (current_bg == ldpc_base_graph_type::BG1) {
    constexpr unsigned BG1_K = BG1_N_FULL - BG1_M;
    switch (node_size_avx2) {
      case 1:
        return &ldpc_encoder_avx2::systematic_bits_inner<BG1_K, BG1_M, 1>;
      case 2:
        return &ldpc_encoder_avx2::systematic_bits_inner<BG1_K, BG1_M, 2>;
      case 3:
        return &ldpc_encoder_avx2::systematic_bits_inner<BG1_K, BG1_M, 3>;
      case 4:
        return &ldpc_encoder_avx2::systematic_bits_inner<BG1_K, BG1_M, 4>;
      case 5:
        return &ldpc_encoder_avx2::systematic_bits_inner<BG1_K, BG1_M, 5>;
      case 6:
        return &ldpc_encoder_avx2::systematic_bits_inner<BG1_K, BG1_M, 6>;
      case 7:
        return &ldpc_encoder_avx2::systematic_bits_inner<BG1_K, BG1_M, 7>;
      case 8:
        return &ldpc_encoder_avx2::systematic_bits_inner<BG1_K, BG1_M, 8>;
      case 9:
        return &ldpc_encoder_avx2::systematic_bits_inner<BG1_K, BG1_M, 9>;
      case 10:
        return &ldpc_encoder_avx2::systematic_bits_inner<BG1_K, BG1_M, 10>;
      case 11:
        return &ldpc_encoder_avx2::systematic_bits_inner<BG1_K, BG1_M, 11>;
      case 12:
        return &ldpc_encoder_avx2::systematic_bits_inner<BG1_K, BG1_M, 12>;
      default:
        srsgnb_assert(false, "The node size in AVX2 registers should be between 1 and 12.");
    }
  }
  constexpr unsigned BG2_K = BG2_N_FULL - BG2_M;
  switch (node_size_avx2) {
    case 1:
      return &ldpc_encoder_avx2::systematic_bits_inner<BG2_K, BG2_M, 1>;
    case 2:
      return &ldpc_encoder_avx2::systematic_bits_inner<BG2_K, BG2_M, 2>;
    case 3:
      return &ldpc_encoder_avx2::systematic_bits_inner<BG2_K, BG2_M, 3>;
    case 4:
      return &ldpc_encoder_avx2::systematic_bits_inner<BG2_K, BG2_M, 4>;
    case 5:
      return &ldpc_encoder_avx2::systematic_bits_inner<BG2_K, BG2_M, 5>;
    case 6:
      return &ldpc_encoder_avx2::systematic_bits_inner<BG2_K, BG2_M, 6>;
    case 7:
      return &ldpc_encoder_avx2::systematic_bits_inner<BG2_K, BG2_M, 7>;
    case 8:
      return &ldpc_encoder_avx2::systematic_bits_inner<BG2_K, BG2_M, 8>;
    case 9:
      return &ldpc_encoder_avx2::systematic_bits_inner<BG2_K, BG2_M, 9>;
    case 10:
      return &ldpc_encoder_avx2::systematic_bits_inner<BG2_K, BG2_M, 10>;
    case 11:
      return &ldpc_encoder_avx2::systematic_bits_inner<BG2_K, BG2_M, 11>;
    case 12:
      return &ldpc_encoder_avx2::systematic_bits_inner<BG2_K, BG2_M, 12>;
    default:
      srsgnb_assert(false, "The node size in AVX2 registers should be between 1 and 12.");
  }
  return nullptr;
}

ldpc_encoder_avx2::strategy_method ldpc_encoder_avx2::select_ext_strategy(const unsigned node_size_avx2)
{
  switch (node_size_avx2) {
    case 1:
      return &ldpc_encoder_avx2::ext_region_inner<1>;
    case 2:
      return &ldpc_encoder_avx2::ext_region_inner<2>;
    case 3:
      return &ldpc_encoder_avx2::ext_region_inner<3>;
    case 4:
      return &ldpc_encoder_avx2::ext_region_inner<4>;
    case 5:
      return &ldpc_encoder_avx2::ext_region_inner<5>;
    case 6:
      return &ldpc_encoder_avx2::ext_region_inner<6>;
    case 7:
      return &ldpc_encoder_avx2::ext_region_inner<7>;
    case 8:
      return &ldpc_encoder_avx2::ext_region_inner<8>;
    case 9:
      return &ldpc_encoder_avx2::ext_region_inner<9>;
    case 10:
      return &ldpc_encoder_avx2::ext_region_inner<10>;
    case 11:
      return &ldpc_encoder_avx2::ext_region_inner<11>;
    case 12:
      return &ldpc_encoder_avx2::ext_region_inner<12>;
    default:
      srsgnb_assert(false, "The node size in AVX2 registers should be between 1 and 12.");
  }
  return nullptr;
}

void ldpc_encoder_avx2::select_strategy()
{
  ldpc_base_graph_type current_bg       = current_graph->get_base_graph();
  uint8_t              current_ls_index = current_graph->get_lifting_index();

  // Each BG node contains LS bits, which are stored in node_size_avx2 AVX2 vectors.
  node_size_avx2 = divide_ceil(lifting_size, AVX2_SIZE_BYTE);

  systematic_bits = select_sys_bits_strategy(current_bg, node_size_avx2);
  high_rate       = select_hr_strategy(current_bg, current_ls_index, node_size_avx2);
  ext_region      = select_ext_strategy(node_size_avx2);
}

void ldpc_encoder_avx2::load_input(span<const uint8_t> in)
{
  // Resize internal buffer.
  codeblock_used_size = codeblock_length / lifting_size * node_size_avx2;

  unsigned i_input        = 0;
  unsigned i_avx2         = 0;
  unsigned node_size_byte = node_size_avx2 * AVX2_SIZE_BYTE;
  for (unsigned i_node = 0; i_node != bg_K; ++i_node) {
    std::memset(codeblock.data_at(i_avx2), 0, node_size_byte);
    std::memcpy(codeblock.data_at(i_avx2), in.data() + i_input, lifting_size);
    i_avx2 += node_size_avx2;
    i_input += lifting_size;
  }
}

template <unsigned BG_K_PH, unsigned BG_M_PH, unsigned NODE_SIZE_AVX2_PH>
void ldpc_encoder_avx2::systematic_bits_inner()
{
  // Resize auxiliary buffer.
  auxiliary_used_size = (codeblock_length / lifting_size - BG_K_PH) * NODE_SIZE_AVX2_PH;

  std::memset(auxiliary.data_at(0), 0, auxiliary_used_size * AVX2_SIZE_BYTE);

  // For each BG information node...
  for (unsigned k = 0, i_blk = 0; k != BG_K_PH; ++k, i_blk += NODE_SIZE_AVX2_PH) {
    // and for each BG check node...
    for (unsigned m = 0, i_aux = 0; m != BG_M_PH; ++m) {
      unsigned node_shift = current_graph->get_lifted_node(m, k);
      if (node_shift == NO_EDGE) {
        i_aux += NODE_SIZE_AVX2_PH;
        continue;
      }
      rotate_node_right(rotated_node.data_at(0), codeblock.data_at(i_blk), node_shift, lifting_size);
      for (unsigned j = 0; j != NODE_SIZE_AVX2_PH; ++j) {
        __m256i tmp_epi8 = _mm256_and_si256(rotated_node.get_at(j), _mm256_set1_epi8(1));
        auxiliary.set_at(i_aux, _mm256_xor_si256(auxiliary.get_at(i_aux), tmp_epi8));
        ++i_aux;
      }
    }
  }
}

template <unsigned NODE_SIZE_AVX2_PH>
void ldpc_encoder_avx2::high_rate_bg1_i6_inner()
{
  unsigned skip0         = bg_K * NODE_SIZE_AVX2_PH;
  unsigned skip1         = (bg_K + 1) * NODE_SIZE_AVX2_PH;
  unsigned skip2         = (bg_K + 2) * NODE_SIZE_AVX2_PH;
  unsigned skip3         = (bg_K + 3) * NODE_SIZE_AVX2_PH;
  unsigned node_size_x_2 = 2 * NODE_SIZE_AVX2_PH;
  unsigned node_size_x_3 = 3 * NODE_SIZE_AVX2_PH;

  // First chunk of parity bits.
  for (unsigned j = 0; j != NODE_SIZE_AVX2_PH; ++j) {
    __m256i tmp_epi8 = _mm256_xor_si256(auxiliary.get_at(j), auxiliary.get_at(NODE_SIZE_AVX2_PH + j));
    tmp_epi8         = _mm256_xor_si256(tmp_epi8, auxiliary.get_at(node_size_x_2 + j));
    tmp_epi8         = _mm256_xor_si256(tmp_epi8, auxiliary.get_at(node_size_x_3 + j));
    rotated_node.set_at(j, tmp_epi8);
  }

  rotate_node_left(codeblock.data_at(skip0), rotated_node.data_at(0), 105 % lifting_size, lifting_size);

  for (unsigned j = 0; j != NODE_SIZE_AVX2_PH; ++j) {
    __m256i block0 = codeblock.get_at(skip0 + j);
    // Second chunk of parity bits.
    codeblock.set_at(skip1 + j, _mm256_xor_si256(auxiliary.get_at(j), block0));
    // Fourth chunk of parity bits.
    __m256i block3 = _mm256_xor_si256(auxiliary.get_at(node_size_x_3 + j), block0);
    codeblock.set_at(skip3 + j, block3);
    // Third chunk of parity bits.
    codeblock.set_at(skip2 + j, _mm256_xor_si256(auxiliary.get_at(node_size_x_2 + j), block3));
  }
}

template <unsigned NODE_SIZE_AVX2_PH>
void ldpc_encoder_avx2::high_rate_bg1_other_inner()
{
  unsigned skip0         = bg_K * NODE_SIZE_AVX2_PH;
  unsigned skip1         = (bg_K + 1) * NODE_SIZE_AVX2_PH;
  unsigned skip2         = (bg_K + 2) * NODE_SIZE_AVX2_PH;
  unsigned skip3         = (bg_K + 3) * NODE_SIZE_AVX2_PH;
  unsigned node_size_x_2 = 2 * NODE_SIZE_AVX2_PH;
  unsigned node_size_x_3 = 3 * NODE_SIZE_AVX2_PH;

  // First chunk of parity bits.
  for (unsigned j = 0; j != NODE_SIZE_AVX2_PH; ++j) {
    __m256i block0 = _mm256_xor_si256(auxiliary.get_at(j), auxiliary.get_at(NODE_SIZE_AVX2_PH + j));
    block0         = _mm256_xor_si256(block0, auxiliary.get_at(node_size_x_2 + j));
    block0         = _mm256_xor_si256(block0, auxiliary.get_at(node_size_x_3 + j));
    codeblock.set_at(skip0 + j, block0);
  }

  rotate_node_right(rotated_node.data_at(0), codeblock.data_at(skip0), 1, lifting_size);

  for (unsigned j = 0; j != NODE_SIZE_AVX2_PH; ++j) {
    __m256i rotated_j = rotated_node.get_at(j);
    // Second chunk of parity bits.
    codeblock.set_at(skip1 + j, _mm256_xor_si256(auxiliary.get_at(j), rotated_j));
    // Fourth chunk of parity bits.
    __m256i block3 = _mm256_xor_si256(auxiliary.get_at(node_size_x_3 + j), rotated_j);
    codeblock.set_at(skip3 + j, block3);
    // Third chunk of parity bits.
    codeblock.set_at(skip2 + j, _mm256_xor_si256(auxiliary.get_at(node_size_x_2 + j), block3));
  }
}

template <unsigned NODE_SIZE_AVX2_PH>
void ldpc_encoder_avx2::high_rate_bg2_i3_7_inner()
{
  unsigned skip0         = bg_K * NODE_SIZE_AVX2_PH;
  unsigned skip1         = (bg_K + 1) * NODE_SIZE_AVX2_PH;
  unsigned skip2         = (bg_K + 2) * NODE_SIZE_AVX2_PH;
  unsigned skip3         = (bg_K + 3) * NODE_SIZE_AVX2_PH;
  unsigned node_size_x_2 = 2 * NODE_SIZE_AVX2_PH;
  unsigned node_size_x_3 = 3 * NODE_SIZE_AVX2_PH;

  // First chunk of parity bits.
  for (unsigned j = 0; j != NODE_SIZE_AVX2_PH; ++j) {
    __m256i block0 = _mm256_xor_si256(auxiliary.get_at(j), auxiliary.get_at(NODE_SIZE_AVX2_PH + j));
    block0         = _mm256_xor_si256(block0, auxiliary.get_at(node_size_x_2 + j));
    block0         = _mm256_xor_si256(block0, auxiliary.get_at(node_size_x_3 + j));
    codeblock.set_at(skip0 + j, block0);
  }

  rotate_node_right(rotated_node.data_at(0), codeblock.data_at(skip0), 1, lifting_size);

  for (unsigned j = 0; j != NODE_SIZE_AVX2_PH; ++j) {
    __m256i rotated_j = rotated_node.get_at(j);
    // Second chunk of parity bits.
    __m256i block_1 = _mm256_xor_si256(auxiliary.get_at(j), rotated_j);
    codeblock.set_at(skip1 + j, block_1);
    // third chunk of parity bits
    codeblock.set_at(skip2 + j, _mm256_xor_si256(auxiliary.get_at(NODE_SIZE_AVX2_PH + j), block_1));
    // fourth chunk of parity bits
    codeblock.set_at(skip3 + j, _mm256_xor_si256(auxiliary.get_at(node_size_x_3 + j), rotated_j));
  }
}

template <unsigned NODE_SIZE_AVX2_PH>
void ldpc_encoder_avx2::high_rate_bg2_other_inner()
{
  unsigned skip0         = bg_K * NODE_SIZE_AVX2_PH;
  unsigned skip1         = (bg_K + 1) * NODE_SIZE_AVX2_PH;
  unsigned skip2         = (bg_K + 2) * NODE_SIZE_AVX2_PH;
  unsigned skip3         = (bg_K + 3) * NODE_SIZE_AVX2_PH;
  unsigned node_size_x_2 = 2 * NODE_SIZE_AVX2_PH;
  unsigned node_size_x_3 = 3 * NODE_SIZE_AVX2_PH;

  // First chunk of parity bits.
  for (unsigned j = 0; j != NODE_SIZE_AVX2_PH; ++j) {
    __m256i rotated_j = _mm256_xor_si256(auxiliary.get_at(j), auxiliary.get_at(NODE_SIZE_AVX2_PH + j));
    rotated_j         = _mm256_xor_si256(rotated_j, auxiliary.get_at(node_size_x_2 + j));
    rotated_j         = _mm256_xor_si256(rotated_j, auxiliary.get_at(node_size_x_3 + j));
    rotated_node.set_at(j, rotated_j);
  }

  rotate_node_left(codeblock.data_at(skip0), rotated_node.data_at(0), 1, lifting_size);

  for (unsigned j = 0; j != NODE_SIZE_AVX2_PH; ++j) {
    __m256i block_0 = codeblock.get_at(skip0 + j);
    // Second chunk of parity bits.
    __m256i block_1 = _mm256_xor_si256(auxiliary.get_at(j), block_0);
    codeblock.set_at(skip1 + j, block_1);
    // Third chunk of parity bits.
    codeblock.set_at(skip2 + j, _mm256_xor_si256(auxiliary.get_at(NODE_SIZE_AVX2_PH + j), block_1));
    // Fourth chunk of parity bits.
    codeblock.set_at(skip3 + j, _mm256_xor_si256(auxiliary.get_at(node_size_x_3 + j), block_0));
  }
}

template <unsigned NODE_SIZE_AVX2_PH>
void ldpc_encoder_avx2::ext_region_inner()
{
  // We only compute the variable nodes needed to fill the codeword.
  // Also, recall the high-rate region has length (bg_K + 4) * lifting_size.
  unsigned nof_layers = codeblock_length / lifting_size - bg_K;

  // Encode the extended region.
  unsigned skip     = (bg_K + 4) * NODE_SIZE_AVX2_PH;
  unsigned skip_aux = 4 * NODE_SIZE_AVX2_PH;
  for (unsigned m = 4; m != nof_layers; ++m) {
    // The systematic part has already been computed.
    for (unsigned j = 0; j != NODE_SIZE_AVX2_PH; ++j) {
      codeblock.set_at(skip + j, auxiliary.get_at(skip_aux + j));
    }

    // Sum the contribution due to the high-rate region, with the proper circular shifts.
    for (unsigned k = 0; k != 4; ++k) {
      unsigned node_shift = current_graph->get_lifted_node(m, bg_K + k);

      if (node_shift == NO_EDGE) {
        continue;
      }
      rotate_node_right(
          rotated_node.data_at(0), codeblock.data_at((bg_K + k) * NODE_SIZE_AVX2_PH), node_shift, lifting_size);
      for (unsigned j = 0; j != NODE_SIZE_AVX2_PH; ++j) {
        codeblock.set_at(skip + j, _mm256_xor_si256(codeblock.get_at(skip + j), rotated_node.get_at(j)));
      }
    }
    skip += NODE_SIZE_AVX2_PH;
    skip_aux += NODE_SIZE_AVX2_PH;
  }
}

void ldpc_encoder_avx2::write_codeblock(span<uint8_t> out)
{
  unsigned nof_nodes = codeblock_length / lifting_size;

  // The first two blocks are shortened and the last node is not considered, since it can be incomplete.
  unsigned i_out = 0;
  unsigned i_in  = 2 * node_size_avx2;
  for (unsigned i_node = 2, max_i_node = nof_nodes - 1; i_node != max_i_node; ++i_node) {
    std::memcpy(out.data() + i_out, codeblock.data_at(i_in), lifting_size);
    i_out += lifting_size;
    i_in += node_size_avx2;
  }

  // Take care of the last node.
  unsigned remainder = out.size() - i_out;
  std::memcpy(out.data() + i_out, codeblock.data_at(i_in), remainder);
}

static void rotate_node_left(__m256i* out, const __m256i* in, unsigned steps, unsigned ls)
{
  std::memcpy(out, reinterpret_cast<const int8_t*>(in) + ls - steps, steps);
  std::memcpy(reinterpret_cast<int8_t*>(out) + steps, in, ls - steps);
}

static void rotate_node_right(__m256i* out, const __m256i* in, unsigned steps, unsigned ls)
{
  std::memcpy(out, reinterpret_cast<const int8_t*>(in) + steps, ls - steps);
  std::memcpy(reinterpret_cast<int8_t*>(out) + ls - steps, in, steps);
}
