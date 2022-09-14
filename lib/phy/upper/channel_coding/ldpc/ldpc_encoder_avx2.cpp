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
#include "srsgnb/srsvec/copy.h"

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

ldpc_encoder_avx2::strategy_method ldpc_encoder_avx2::select_hr_strategy_short(const ldpc_base_graph_type current_bg,
                                                                               const uint8_t current_ls_index)
{
  if (current_bg == ldpc_base_graph_type::BG1) {
    if (current_ls_index == 6) {
      return &ldpc_encoder_avx2::high_rate_bg1_i6_short;
    }
    // If current_lifting_index is not 6...
    return &ldpc_encoder_avx2::high_rate_bg1_other_short;
  }
  // Else, if current_bg == BG2...
  if ((current_ls_index == 3) || (current_ls_index == 7)) {
    return &ldpc_encoder_avx2::high_rate_bg2_i3_7_short;
  }
  // If current_lifting_index is neither 3 nor 7...
  return &ldpc_encoder_avx2::high_rate_bg2_other_short;
}

ldpc_encoder_avx2::strategy_method ldpc_encoder_avx2::select_hr_strategy_long(const ldpc_base_graph_type current_bg,
                                                                              const uint8_t current_ls_index)
{
  if (current_bg == ldpc_base_graph_type::BG1) {
    if (current_ls_index == 6) {
      return &ldpc_encoder_avx2::high_rate_bg1_i6_long;
    }
    // If current_lifting_index is not 6...
    return &ldpc_encoder_avx2::high_rate_bg1_other_long;
  }
  // Else, if current_bg == BG2...
  if ((current_ls_index == 3) || (current_ls_index == 7)) {
    return &ldpc_encoder_avx2::high_rate_bg2_i3_7_long;
  }
  // If current_lifting_index is neither 3 nor 7...
  return &ldpc_encoder_avx2::high_rate_bg2_other_long;
}

void ldpc_encoder_avx2::select_strategy()
{
  ldpc_base_graph_type current_bg       = current_graph->get_base_graph();
  uint8_t              current_ls_index = current_graph->get_lifting_index();

  if (lifting_size <= AVX2_SIZE_BYTE) {
    systematic_bits = &ldpc_encoder_avx2::systematic_bits_short;
    high_rate       = select_hr_strategy_short(current_bg, current_ls_index);
    ext_region      = &ldpc_encoder_avx2::ext_region_short;

  } else { // if lifting_size > AVX2_SIZE_BYTE
    systematic_bits = &ldpc_encoder_avx2::systematic_bits_long;
    high_rate       = select_hr_strategy_long(current_bg, current_ls_index);
    ext_region      = &ldpc_encoder_avx2::ext_region_long;
  }
}

void ldpc_encoder_avx2::load_input(span<const uint8_t> in)
{
  // Each BG node contains LS bits, which are stored in node_size_avx2 AVX2 vectors.
  node_size_avx2 = divide_ceil(lifting_size, AVX2_SIZE_BYTE);

  // Resize internal buffer.
  codeblock_used_size = codeblock_length / lifting_size * node_size_avx2;

  unsigned i_input = 0;
  unsigned skip    = 0;
  for (unsigned i_node = 0; i_node != bg_K; ++i_node) {
    unsigned i_avx2 = 0;
    // First deal with the part of the node that fills AVX2 vectors completely.
    for (unsigned max_i_avx2 = lifting_size / AVX2_SIZE_BYTE; i_avx2 != max_i_avx2; ++i_avx2) {
      codeblock.set_at(skip++, _mm256_lddqu_si256(reinterpret_cast<const __m256i*>(&in[i_input])));
      i_input += AVX2_SIZE_BYTE;
    }
    // Now deal with the remaining part of the node.
    unsigned left_over = lifting_size % AVX2_SIZE_BYTE;
    if (left_over > 0) {
      // First reset AX2 register.
      codeblock.set_at(skip, _mm256_setzero_si256());
      std::memcpy(codeblock.data_at(skip), &in[i_input], left_over);
      ++skip;
      i_input += left_over;
    }
  }
}

void ldpc_encoder_avx2::systematic_bits_short()
{
  // Resize auxiliary buffer.
  auxiliary_used_size = (codeblock_length / lifting_size - bg_K) * node_size_avx2;

  for (unsigned i_avx2 = 0; i_avx2 != auxiliary_used_size; ++i_avx2) {
    auxiliary.set_at(i_avx2, _mm256_setzero_si256());
  }

  // For each BG information node...
  for (unsigned k = 0; k != bg_K; ++k) {
    // and for each BG check node...
    for (unsigned m = 0; m != bg_M; ++m) {
      unsigned node_shift = current_graph->get_lifted_node(m, k);
      if (node_shift == NO_EDGE) {
        continue;
      }
      __m256i tmp;
      rotate_node_right(&tmp, codeblock.data_at(k), node_shift, lifting_size);
      tmp = _mm256_and_si256(tmp, _mm256_set1_epi8(1));
      auxiliary.set_at(m, _mm256_xor_si256(auxiliary.get_at(m), tmp));
    }
  }
}

void ldpc_encoder_avx2::systematic_bits_long()
{
  // Resize auxiliary buffer.
  auxiliary_used_size = (codeblock_length / lifting_size - bg_K) * node_size_avx2;

  for (unsigned i_avx2 = 0; i_avx2 != auxiliary_used_size; ++i_avx2) {
    auxiliary.set_at(i_avx2, _mm256_setzero_si256());
  }

  // For each BG information node...
  for (unsigned k = 0, i_blk = 0; k != bg_K; ++k, i_blk += node_size_avx2) {
    // and for each BG check node...
    for (unsigned m = 0, i_aux = 0; m != bg_M; ++m) {
      unsigned node_shift = current_graph->get_lifted_node(m, k);
      if (node_shift == NO_EDGE) {
        i_aux += node_size_avx2;
        continue;
      }
      rotate_node_right(rotated_node.data_at(0), codeblock.data_at(i_blk), node_shift, lifting_size);
      for (unsigned j = 0; j != node_size_avx2; ++j) {
        __m256i tmp_epi8 = _mm256_and_si256(rotated_node.get_at(j), _mm256_set1_epi8(1));
        auxiliary.set_at(i_aux, _mm256_xor_si256(auxiliary.get_at(i_aux), tmp_epi8));
        ++i_aux;
      }
    }
  }
}

void ldpc_encoder_avx2::high_rate_bg1_i6_short()
{
  unsigned skip0 = bg_K;
  unsigned skip1 = bg_K + 1;
  unsigned skip2 = bg_K + 2;
  unsigned skip3 = bg_K + 3;

  // First chunk of parity bits.
  __m256i tmp_epi8 = _mm256_xor_si256(auxiliary.get_at(0), auxiliary.get_at(1));
  tmp_epi8         = _mm256_xor_si256(tmp_epi8, auxiliary.get_at(2));
  tmp_epi8         = _mm256_xor_si256(tmp_epi8, auxiliary.get_at(3));
  __m256i rotated_tmp;
  rotate_node_left(&rotated_tmp, &tmp_epi8, 105 % lifting_size, lifting_size);
  codeblock.set_at(skip0, rotated_tmp);

  // Second chunk of parity bits.
  codeblock.set_at(skip1, _mm256_xor_si256(auxiliary.get_at(0), codeblock.get_at(skip0)));
  // Fourth chunk of parity bits.
  codeblock.set_at(skip3, _mm256_xor_si256(auxiliary.get_at(3), codeblock.get_at(skip0)));
  // Third chunk of parity bits.
  codeblock.set_at(skip2, _mm256_xor_si256(auxiliary.get_at(2), codeblock.get_at(skip3)));
}

void ldpc_encoder_avx2::high_rate_bg1_other_short()
{
  unsigned skip0 = bg_K;
  unsigned skip1 = bg_K + 1;
  unsigned skip2 = bg_K + 2;
  unsigned skip3 = bg_K + 3;

  // First chunk of parity bits.
  codeblock.set_at(skip0, _mm256_xor_si256(auxiliary.get_at(0), auxiliary.get_at(1)));
  codeblock.set_at(skip0, _mm256_xor_si256(codeblock.get_at(skip0), auxiliary.get_at(2)));
  codeblock.set_at(skip0, _mm256_xor_si256(codeblock.get_at(skip0), auxiliary.get_at(3)));

  __m256i tmp_epi8;
  rotate_node_right(&tmp_epi8, codeblock.data_at(skip0), 1, lifting_size);

  // Second chunk of parity bits.
  codeblock.set_at(skip1, _mm256_xor_si256(auxiliary.get_at(0), tmp_epi8));
  // Fourth chunk of parity bits.
  codeblock.set_at(skip3, _mm256_xor_si256(auxiliary.get_at(3), tmp_epi8));
  // Third chunk of parity bits.
  codeblock.set_at(skip2, _mm256_xor_si256(auxiliary.get_at(2), codeblock.get_at(skip3)));
}

void ldpc_encoder_avx2::high_rate_bg2_i3_7_short()
{
  unsigned skip0 = bg_K;
  unsigned skip1 = bg_K + 1;
  unsigned skip2 = bg_K + 2;
  unsigned skip3 = bg_K + 3;

  // First chunk of parity bits.
  codeblock.set_at(skip0, _mm256_xor_si256(auxiliary.get_at(0), auxiliary.get_at(1)));
  codeblock.set_at(skip0, _mm256_xor_si256(codeblock.get_at(skip0), auxiliary.get_at(2)));
  codeblock.set_at(skip0, _mm256_xor_si256(codeblock.get_at(skip0), auxiliary.get_at(3)));

  __m256i tmp_epi8;
  rotate_node_right(&tmp_epi8, codeblock.data_at(skip0), 1, lifting_size);
  // Second chunk of parity bits.
  codeblock.set_at(skip1, _mm256_xor_si256(auxiliary.get_at(0), tmp_epi8));
  // Third chunk of parity bits.
  codeblock.set_at(skip2, _mm256_xor_si256(auxiliary.get_at(1), codeblock.get_at(skip1)));
  // Fourth chunk of parity bits.
  codeblock.set_at(skip3, _mm256_xor_si256(auxiliary.get_at(3), tmp_epi8));
}

void ldpc_encoder_avx2::high_rate_bg2_other_short()
{
  unsigned skip0 = bg_K;
  unsigned skip1 = bg_K + 1;
  unsigned skip2 = bg_K + 2;
  unsigned skip3 = bg_K + 3;

  // First chunk of parity bits.
  __m256i tmp_epi8 = _mm256_xor_si256(auxiliary.get_at(0), auxiliary.get_at(1));
  tmp_epi8         = _mm256_xor_si256(tmp_epi8, auxiliary.get_at(2));
  tmp_epi8         = _mm256_xor_si256(tmp_epi8, auxiliary.get_at(3));
  rotate_node_left(codeblock.data_at(skip0), &tmp_epi8, 1, lifting_size);

  // Second chunk of parity bits.
  codeblock.set_at(skip1, _mm256_xor_si256(auxiliary.get_at(0), codeblock.get_at(skip0)));
  // Third chunk of parity bits.
  codeblock.set_at(skip2, _mm256_xor_si256(auxiliary.get_at(1), codeblock.get_at(skip1)));
  // Fourth chunk of parity bits.
  codeblock.set_at(skip3, _mm256_xor_si256(auxiliary.get_at(3), codeblock.get_at(skip0)));
}

void ldpc_encoder_avx2::high_rate_bg1_i6_long()
{
  unsigned skip0         = bg_K * node_size_avx2;
  unsigned skip1         = (bg_K + 1) * node_size_avx2;
  unsigned skip2         = (bg_K + 2) * node_size_avx2;
  unsigned skip3         = (bg_K + 3) * node_size_avx2;
  unsigned node_size_x_2 = 2 * node_size_avx2;
  unsigned node_size_x_3 = 3 * node_size_avx2;

  // First chunk of parity bits.
  for (unsigned j = 0; j != node_size_avx2; ++j) {
    rotated_node.set_at(j, _mm256_xor_si256(auxiliary.get_at(j), auxiliary.get_at(node_size_avx2 + j)));
    rotated_node.set_at(j, _mm256_xor_si256(rotated_node.get_at(j), auxiliary.get_at(node_size_x_2 + j)));
    rotated_node.set_at(j, _mm256_xor_si256(rotated_node.get_at(j), auxiliary.get_at(node_size_x_3 + j)));
  }

  rotate_node_right(codeblock.data_at(skip0), rotated_node.data_at(0), lifting_size - 105 % lifting_size, lifting_size);

  for (unsigned j = 0; j != node_size_avx2; ++j) {
    // Second chunk of parity bits.
    codeblock.set_at(skip1 + j, _mm256_xor_si256(auxiliary.get_at(j), codeblock.get_at(skip0 + j)));
    // Fourth chunk of parity bits.
    codeblock.set_at(skip3 + j, _mm256_xor_si256(auxiliary.get_at(node_size_x_3 + j), codeblock.get_at(skip0 + j)));
    // Third chunk of parity bits.
    codeblock.set_at(skip2 + j, _mm256_xor_si256(auxiliary.get_at(node_size_x_2 + j), codeblock.get_at(skip3 + j)));
  }
}

void ldpc_encoder_avx2::high_rate_bg1_other_long()
{
  unsigned skip0         = bg_K * node_size_avx2;
  unsigned skip1         = (bg_K + 1) * node_size_avx2;
  unsigned skip2         = (bg_K + 2) * node_size_avx2;
  unsigned skip3         = (bg_K + 3) * node_size_avx2;
  unsigned node_size_x_2 = 2 * node_size_avx2;
  unsigned node_size_x_3 = 3 * node_size_avx2;

  // First chunk of parity bits.
  for (unsigned j = 0; j != node_size_avx2; ++j) {
    codeblock.set_at(skip0 + j, _mm256_xor_si256(auxiliary.get_at(j), auxiliary.get_at(node_size_avx2 + j)));
    codeblock.set_at(skip0 + j, _mm256_xor_si256(codeblock.get_at(skip0 + j), auxiliary.get_at(node_size_x_2 + j)));
    codeblock.set_at(skip0 + j, _mm256_xor_si256(codeblock.get_at(skip0 + j), auxiliary.get_at(node_size_x_3 + j)));
  }

  rotate_node_right(rotated_node.data_at(0), codeblock.data_at(skip0), 1, lifting_size);

  for (unsigned j = 0; j != node_size_avx2; ++j) {
    // Second chunk of parity bits.
    codeblock.set_at(skip1 + j, _mm256_xor_si256(auxiliary.get_at(j), rotated_node.get_at(j)));
    // Fourth chunk of parity bits.
    codeblock.set_at(skip3 + j, _mm256_xor_si256(auxiliary.get_at(node_size_x_3 + j), rotated_node.get_at(j)));
    // Third chunk of parity bits.
    codeblock.set_at(skip2 + j, _mm256_xor_si256(auxiliary.get_at(node_size_x_2 + j), codeblock.get_at(skip3 + j)));
  }
}

void ldpc_encoder_avx2::high_rate_bg2_i3_7_long()
{
  unsigned skip0         = bg_K * node_size_avx2;
  unsigned skip1         = (bg_K + 1) * node_size_avx2;
  unsigned skip2         = (bg_K + 2) * node_size_avx2;
  unsigned skip3         = (bg_K + 3) * node_size_avx2;
  unsigned node_size_x_2 = 2 * node_size_avx2;
  unsigned node_size_x_3 = 3 * node_size_avx2;

  // First chunk of parity bits.
  for (unsigned j = 0; j != node_size_avx2; ++j) {
    codeblock.set_at(skip0 + j, _mm256_xor_si256(auxiliary.get_at(j), auxiliary.get_at(node_size_avx2 + j)));
    codeblock.set_at(skip0 + j, _mm256_xor_si256(codeblock.get_at(skip0 + j), auxiliary.get_at(node_size_x_2 + j)));
    codeblock.set_at(skip0 + j, _mm256_xor_si256(codeblock.get_at(skip0 + j), auxiliary.get_at(node_size_x_3 + j)));
  }

  rotate_node_right(rotated_node.data_at(0), codeblock.data_at(skip0), 1, lifting_size);

  for (unsigned j = 0; j != node_size_avx2; ++j) {
    // Second chunk of parity bits.
    codeblock.set_at(skip1 + j, _mm256_xor_si256(auxiliary.get_at(j), rotated_node.get_at(j)));
    // third chunk of parity bits
    codeblock.set_at(skip2 + j, _mm256_xor_si256(auxiliary.get_at(node_size_avx2 + j), codeblock.get_at(skip1 + j)));
    // fourth chunk of parity bits
    codeblock.set_at(skip3 + j, _mm256_xor_si256(auxiliary.get_at(node_size_x_3 + j), rotated_node.get_at(j)));
  }
}

void ldpc_encoder_avx2::high_rate_bg2_other_long()
{
  unsigned skip0         = bg_K * node_size_avx2;
  unsigned skip1         = (bg_K + 1) * node_size_avx2;
  unsigned skip2         = (bg_K + 2) * node_size_avx2;
  unsigned skip3         = (bg_K + 3) * node_size_avx2;
  unsigned node_size_x_2 = 2 * node_size_avx2;
  unsigned node_size_x_3 = 3 * node_size_avx2;

  // First chunk of parity bits.
  for (unsigned j = 0; j != node_size_avx2; ++j) {
    rotated_node.set_at(j, _mm256_xor_si256(auxiliary.get_at(j), auxiliary.get_at(node_size_avx2 + j)));
    rotated_node.set_at(j, _mm256_xor_si256(rotated_node.get_at(j), auxiliary.get_at(node_size_x_2 + j)));
    rotated_node.set_at(j, _mm256_xor_si256(rotated_node.get_at(j), auxiliary.get_at(node_size_x_3 + j)));
  }

  rotate_node_right(codeblock.data_at(skip0), rotated_node.data_at(0), lifting_size - 1, lifting_size);

  for (unsigned j = 0; j != node_size_avx2; ++j) {
    // Second chunk of parity bits.
    codeblock.set_at(skip1 + j, _mm256_xor_si256(auxiliary.get_at(j), codeblock.get_at(skip0 + j)));
    // Third chunk of parity bits.
    codeblock.set_at(skip2 + j, _mm256_xor_si256(auxiliary.get_at(node_size_avx2 + j), codeblock.get_at(skip1 + j)));
    // Fourth chunk of parity bits.
    codeblock.set_at(skip3 + j, _mm256_xor_si256(auxiliary.get_at(node_size_x_3 + j), codeblock.get_at(skip0 + j)));
  }
}

void ldpc_encoder_avx2::ext_region_short()
{
  // We only compute the variable nodes needed to fill the codeword.
  // Also, recall the high-rate region has length (bg_K + 4) * lifting_size.
  unsigned nof_layers = codeblock_length / lifting_size - bg_K;

  // Encode the extended region.
  for (unsigned m = 4; m != nof_layers; ++m) {
    unsigned skip = bg_K + m;

    // The systematic part has already been computed.
    codeblock.set_at(skip, auxiliary.get_at(m));

    // Sum the contribution due to the high-rate region, with the proper circular shifts.
    for (unsigned k = 0; k != 4; ++k) {
      unsigned node_shift = current_graph->get_lifted_node(m, bg_K + k);
      if (node_shift == ldpc::NO_EDGE) {
        continue;
      }
      __m256i tmp_epi8;
      rotate_node_right(&tmp_epi8, codeblock.data_at(bg_K + k), node_shift, lifting_size);
      codeblock.set_at(skip, _mm256_xor_si256(codeblock.get_at(skip), tmp_epi8));
    }
  }
}

void ldpc_encoder_avx2::ext_region_long()
{
  // We only compute the variable nodes needed to fill the codeword.
  // Also, recall the high-rate region has length (bg_K + 4) * lifting_size.
  unsigned nof_layers = codeblock_length / lifting_size - bg_K;

  // Encode the extended region.
  unsigned skip     = (bg_K + 4) * node_size_avx2;
  unsigned skip_aux = 4 * node_size_avx2;
  for (unsigned m = 4; m != nof_layers; ++m) {
    // The systematic part has already been computed.
    for (unsigned j = 0; j != node_size_avx2; ++j) {
      codeblock.set_at(skip + j, auxiliary.get_at(skip_aux + j));
    }

    // Sum the contribution due to the high-rate region, with the proper circular shifts.
    for (unsigned k = 0; k < 4; ++k) {
      unsigned node_shift = current_graph->get_lifted_node(m, bg_K + k);

      if (node_shift == NO_EDGE) {
        continue;
      }
      rotate_node_right(
          rotated_node.data_at(0), codeblock.data_at((bg_K + k) * node_size_avx2), node_shift, lifting_size);
      for (unsigned j = 0; j != node_size_avx2; ++j) {
        codeblock.set_at(skip + j, _mm256_xor_si256(codeblock.get_at(skip + j), rotated_node.get_at(j)));
      }
    }
    skip += node_size_avx2;
    skip_aux += node_size_avx2;
  }
}

void ldpc_encoder_avx2::write_codeblock(span<uint8_t> out)
{
  unsigned nof_nodes = codeblock_length / lifting_size;

  unsigned node_size_byte = node_size_avx2 * AVX2_SIZE_BYTE;

  span<uint8_t> tmp =
      span<uint8_t>(reinterpret_cast<uint8_t*>(codeblock.data_at(0)), codeblock_used_size * AVX2_SIZE_BYTE);
  // The first two blocks are shortened and the last node is not considered, since it can be incomplete.
  unsigned i_out = 0;
  unsigned i_tmp = 2 * node_size_byte;
  for (unsigned i_node = 2, max_i_node = nof_nodes - 1; i_node != max_i_node; ++i_node) {
    srsvec::copy(out.subspan(i_out, lifting_size), tmp.subspan(i_tmp, lifting_size));
    i_out += lifting_size;
    i_tmp += node_size_byte;
  }

  // Take care of the last node.
  unsigned remainder = out.size() - i_out;
  srsvec::copy(out.subspan(i_out, remainder), tmp.subspan(i_tmp, remainder));
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
