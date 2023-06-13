/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ldpc_encoder_avx2.h"
#include "avx2_support.h"
#include "srsran/srsvec/binary.h"
#include "srsran/srsvec/circ_shift.h"
#include "srsran/srsvec/copy.h"
#include "srsran/srsvec/zero.h"

using namespace srsran;
using namespace srsran::ldpc;

/// Maximum number of AVX2 vectors needed to represent a BG node.
static constexpr unsigned MAX_NODE_SIZE_AVX2 = divide_ceil(ldpc::MAX_LIFTING_SIZE, AVX2_SIZE_BYTE);

// Recursively selects the proper strategy for the high-rate region by successively decreasing the value of the template
// parameter.
template <unsigned NODE_SIZE_AVX2_PH>
ldpc_encoder_avx2::strategy_method ldpc_encoder_avx2::select_hr_strategy(ldpc_base_graph_type current_bg,
                                                                         uint8_t              current_ls_index,
                                                                         unsigned             node_size_avx2)
{
  if (node_size_avx2 != NODE_SIZE_AVX2_PH) {
    return select_hr_strategy<NODE_SIZE_AVX2_PH - 1>(current_bg, current_ls_index, node_size_avx2);
  }

  if (current_bg == ldpc_base_graph_type::BG1) {
    if (current_ls_index == 6) {
      return &ldpc_encoder_avx2::high_rate_bg1_i6_inner<NODE_SIZE_AVX2_PH>;
    }
    // If current_lifting_index is not 6...
    return &ldpc_encoder_avx2::high_rate_bg1_other_inner<NODE_SIZE_AVX2_PH>;
  }
  // Else, if current_bg == BG2...
  if ((current_ls_index == 3) || (current_ls_index == 7)) {
    return &ldpc_encoder_avx2::high_rate_bg2_i3_7_inner<NODE_SIZE_AVX2_PH>;
  }
  // If current_lifting_index is neither 3 nor 7...
  return &ldpc_encoder_avx2::high_rate_bg2_other_inner<NODE_SIZE_AVX2_PH>;
}

// Ensures that the recursion stops when NODE_SIZE_AVX2_PH == 1.
template <>
ldpc_encoder_avx2::strategy_method ldpc_encoder_avx2::select_hr_strategy<1>(ldpc_base_graph_type current_bg,
                                                                            uint8_t              current_ls_index,
                                                                            unsigned /*node_size_avx2*/)
{
  if (current_bg == ldpc_base_graph_type::BG1) {
    if (current_ls_index == 6) {
      return &ldpc_encoder_avx2::high_rate_bg1_i6_inner<1>;
    }
    // If current_lifting_index is not 6...
    return &ldpc_encoder_avx2::high_rate_bg1_other_inner<1>;
  }
  // Else, if current_bg == BG2...
  if ((current_ls_index == 3) || (current_ls_index == 7)) {
    return &ldpc_encoder_avx2::high_rate_bg2_i3_7_inner<1>;
  }
  // If current_lifting_index is neither 3 nor 7...
  return &ldpc_encoder_avx2::high_rate_bg2_other_inner<1>;
}

static constexpr unsigned BG1_K = BG1_N_FULL - BG1_M;
static constexpr unsigned BG2_K = BG2_N_FULL - BG2_M;

// Recursively selects the proper strategy for the systematic bits by successively decreasing the value of the template
// parameter.
template <unsigned NODE_SIZE_AVX2_PH>
ldpc_encoder_avx2::strategy_method ldpc_encoder_avx2::select_sys_bits_strategy(ldpc_base_graph_type current_bg,
                                                                               unsigned             node_size_avx2)
{
  if (node_size_avx2 != NODE_SIZE_AVX2_PH) {
    return select_sys_bits_strategy<NODE_SIZE_AVX2_PH - 1>(current_bg, node_size_avx2);
  }
  if (current_bg == ldpc_base_graph_type::BG1) {
    return &ldpc_encoder_avx2::systematic_bits_inner<BG1_K, BG1_M, NODE_SIZE_AVX2_PH>;
  }
  return &ldpc_encoder_avx2::systematic_bits_inner<BG2_K, BG2_M, NODE_SIZE_AVX2_PH>;
}

// Ensures that the recursion stops when NODE_SIZE_AVX2_PH == 1.
template <>
ldpc_encoder_avx2::strategy_method ldpc_encoder_avx2::select_sys_bits_strategy<1>(ldpc_base_graph_type current_bg,
                                                                                  unsigned /*node_size_avx2*/)
{
  if (current_bg == ldpc_base_graph_type::BG1) {
    return &ldpc_encoder_avx2::systematic_bits_inner<BG1_K, BG1_M, 1>;
  }
  return &ldpc_encoder_avx2::systematic_bits_inner<BG2_K, BG2_M, 1>;
}

// Recursively selects the proper strategy for the extended region by successively decreasing the value of the template
// parameter.
template <unsigned NODE_SIZE_AVX2_PH>
ldpc_encoder_avx2::strategy_method ldpc_encoder_avx2::select_ext_strategy(unsigned node_size_avx2)
{
  if (node_size_avx2 == NODE_SIZE_AVX2_PH) {
    return &ldpc_encoder_avx2::ext_region_inner<NODE_SIZE_AVX2_PH>;
  }
  return select_ext_strategy<NODE_SIZE_AVX2_PH - 1>(node_size_avx2);
}

// Ensures that the recursion stops when NODE_SIZE_AVX2_PH == 1.
template <>
ldpc_encoder_avx2::strategy_method ldpc_encoder_avx2::select_ext_strategy<1>(unsigned /*node_size_avx2*/)
{
  return &ldpc_encoder_avx2::ext_region_inner<1>;
}

void ldpc_encoder_avx2::select_strategy()
{
  ldpc_base_graph_type current_bg       = current_graph->get_base_graph();
  uint8_t              current_ls_index = current_graph->get_lifting_index();

  // Each BG node contains LS bits, which are stored in node_size_avx2 AVX2 vectors.
  node_size_avx2 = divide_ceil(lifting_size, AVX2_SIZE_BYTE);

  systematic_bits = select_sys_bits_strategy<MAX_NODE_SIZE_AVX2>(current_bg, node_size_avx2);
  high_rate       = select_hr_strategy<MAX_NODE_SIZE_AVX2>(current_bg, current_ls_index, node_size_avx2);
  ext_region      = select_ext_strategy<MAX_NODE_SIZE_AVX2>(node_size_avx2);
}

void ldpc_encoder_avx2::load_input(span<const uint8_t> in)
{
  unsigned node_size_byte = node_size_avx2 * AVX2_SIZE_BYTE;
  unsigned tail_bytes     = node_size_byte - lifting_size;

  // Set state variables depending on the codeblock length.
  codeblock_used_size = codeblock_length / lifting_size * node_size_avx2;
  auxiliary_used_size = (codeblock_length / lifting_size - bg_K) * node_size_avx2;

  span<uint8_t>       codeblock(codeblock_buffer);
  span<const uint8_t> in_tmp = in;
  for (unsigned i_node = 0; i_node != bg_K; ++i_node) {
    srsvec::copy(codeblock.first(lifting_size), in_tmp.first(lifting_size));
    codeblock = codeblock.last(codeblock.size() - lifting_size);
    in_tmp    = in_tmp.last(in_tmp.size() - lifting_size);

    srsvec::zero(codeblock.first(tail_bytes));
    codeblock = codeblock.last(codeblock.size() - tail_bytes);
  }
}

void myxor(span<int8_t> out, span<const int8_t> in0, span<const int8_t> in1)
{
  unsigned i = 0;

  const __m256i* in0_ptr = reinterpret_cast<const __m256i*>(in0.data());
  const __m256i* in1_ptr = reinterpret_cast<const __m256i*>(in1.data());
  __m256i*       out_ptr = reinterpret_cast<__m256i*>(out.data());
  for (unsigned i_end = (out.size() / 32) * 32; i != i_end; i += 32) {
    __m256i in0_ = _mm256_loadu_si256(in0_ptr++);
    __m256i in1_ = _mm256_loadu_si256(in1_ptr++);

    _mm256_storeu_si256(out_ptr++, _mm256_xor_si256(in0_, in1_));
  }

  for (unsigned i_end = out.size(); i != i_end; ++i) {
    out[i] = in0[i] ^ in1[i];
  }
}

void myand_one(span<int8_t> out, span<const int8_t> in)
{
  unsigned i = 0;

  const __m256i* in0_ptr = reinterpret_cast<const __m256i*>(in.data());
  __m256i*       out_ptr = reinterpret_cast<__m256i*>(out.data());
  for (unsigned i_end = (out.size() / 32) * 32; i != i_end; i += 32) {
    __m256i in0_ = _mm256_loadu_si256(in0_ptr++);

    _mm256_storeu_si256(out_ptr++, _mm256_and_si256(in0_, _mm256_set1_epi8(1)));
  }

  for (unsigned i_end = out.size(); i != i_end; ++i) {
    out[i] = in[i] & 1;
  }
}

template <unsigned BG_K_PH, unsigned BG_M_PH, unsigned NODE_SIZE_AVX2_PH>
void ldpc_encoder_avx2::systematic_bits_inner()
{
  mm256::avx2_const_span codeblock(codeblock_buffer, codeblock_used_size);

  mm256::avx2_span auxiliary(auxiliary_buffer, auxiliary_used_size);

  span<uint8_t> aux_tmp(auxiliary_buffer);
  srsvec::zero(aux_tmp.first(auxiliary_used_size * AVX2_SIZE_BYTE));
  const auto& parity_check_sparse = current_graph->get_parity_check_sparse();

  std::array<int8_t, 2 * MAX_LIFTING_SIZE> tmp_blk;
  span<int8_t>                             blk           = span<int8_t>(tmp_blk).first(2 * lifting_size);
  unsigned                                 current_i_blk = std::numeric_limits<unsigned>::max();

  for (const auto& element : parity_check_sparse) {
    unsigned m          = std::get<0>(element);
    unsigned k          = std::get<1>(element);
    unsigned node_shift = std::get<2>(element);

    unsigned i_aux = m * NODE_SIZE_AVX2_PH;
    unsigned i_blk = k * NODE_SIZE_AVX2_PH;

    if (i_aux >= auxiliary_used_size) {
      continue;
    }

    if (i_blk != current_i_blk) {
      current_i_blk = i_blk;
      myand_one(blk.first(lifting_size), codeblock.plain_span(current_i_blk, lifting_size));
      myand_one(blk.last(lifting_size), codeblock.plain_span(current_i_blk, lifting_size));
    }

    myxor(auxiliary.plain_span(i_aux, lifting_size),
          blk.subspan(node_shift, lifting_size),
          auxiliary.plain_span(i_aux, lifting_size));
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

  mm256::avx2_span codeblock(codeblock_buffer, codeblock_used_size);

  mm256::avx2_const_span auxiliary(auxiliary_buffer, auxiliary_used_size);

  mm256::avx2_span rotated_node(rotated_node_buffer, NODE_SIZE_AVX2_PH);

  // First chunk of parity bits.
  for (unsigned j = 0; j != NODE_SIZE_AVX2_PH; ++j) {
    __m256i tmp_epi8 = _mm256_xor_si256(auxiliary.get_at(j), auxiliary.get_at(NODE_SIZE_AVX2_PH + j));
    tmp_epi8         = _mm256_xor_si256(tmp_epi8, auxiliary.get_at(node_size_x_2 + j));
    tmp_epi8         = _mm256_xor_si256(tmp_epi8, auxiliary.get_at(node_size_x_3 + j));
    rotated_node.set_at(j, tmp_epi8);
  }

  srsvec::circ_shift_forward(
      codeblock.plain_span(skip0, lifting_size), rotated_node.plain_span(0, lifting_size), 105 % lifting_size);

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

  mm256::avx2_span codeblock(codeblock_buffer, codeblock_used_size);

  mm256::avx2_const_span auxiliary(auxiliary_buffer, auxiliary_used_size);

  mm256::avx2_span rotated_node(rotated_node_buffer, NODE_SIZE_AVX2_PH);

  // First chunk of parity bits.
  for (unsigned j = 0; j != NODE_SIZE_AVX2_PH; ++j) {
    __m256i block0 = _mm256_xor_si256(auxiliary.get_at(j), auxiliary.get_at(NODE_SIZE_AVX2_PH + j));
    block0         = _mm256_xor_si256(block0, auxiliary.get_at(node_size_x_2 + j));
    block0         = _mm256_xor_si256(block0, auxiliary.get_at(node_size_x_3 + j));
    codeblock.set_at(skip0 + j, block0);
  }

  srsvec::circ_shift_backward(rotated_node.plain_span(0, lifting_size), codeblock.plain_span(skip0, lifting_size), 1);

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

  mm256::avx2_span codeblock(codeblock_buffer, codeblock_used_size);

  mm256::avx2_const_span auxiliary(auxiliary_buffer, auxiliary_used_size);

  mm256::avx2_span rotated_node(rotated_node_buffer, NODE_SIZE_AVX2_PH);

  // First chunk of parity bits.
  for (unsigned j = 0; j != NODE_SIZE_AVX2_PH; ++j) {
    __m256i block0 = _mm256_xor_si256(auxiliary.get_at(j), auxiliary.get_at(NODE_SIZE_AVX2_PH + j));
    block0         = _mm256_xor_si256(block0, auxiliary.get_at(node_size_x_2 + j));
    block0         = _mm256_xor_si256(block0, auxiliary.get_at(node_size_x_3 + j));
    codeblock.set_at(skip0 + j, block0);
  }

  srsvec::circ_shift_backward(rotated_node.plain_span(0, lifting_size), codeblock.plain_span(skip0, lifting_size), 1);

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

  mm256::avx2_span codeblock(codeblock_buffer, codeblock_used_size);

  mm256::avx2_const_span auxiliary(auxiliary_buffer, auxiliary_used_size);

  mm256::avx2_span rotated_node(rotated_node_buffer, NODE_SIZE_AVX2_PH);

  // First chunk of parity bits.
  for (unsigned j = 0; j != NODE_SIZE_AVX2_PH; ++j) {
    __m256i rotated_j = _mm256_xor_si256(auxiliary.get_at(j), auxiliary.get_at(NODE_SIZE_AVX2_PH + j));
    rotated_j         = _mm256_xor_si256(rotated_j, auxiliary.get_at(node_size_x_2 + j));
    rotated_j         = _mm256_xor_si256(rotated_j, auxiliary.get_at(node_size_x_3 + j));
    rotated_node.set_at(j, rotated_j);
  }

  srsvec::circ_shift_forward(codeblock.plain_span(skip0, lifting_size), rotated_node.plain_span(0, lifting_size), 1);

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

  mm256::avx2_span codeblock(codeblock_buffer, codeblock_used_size);

  mm256::avx2_const_span auxiliary(auxiliary_buffer, auxiliary_used_size);

  mm256::avx2_span rotated_node(rotated_node_buffer, NODE_SIZE_AVX2_PH);

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
      srsvec::circ_shift_backward(rotated_node.plain_span(0, lifting_size),
                                  codeblock.plain_span((bg_K + k) * NODE_SIZE_AVX2_PH, lifting_size),
                                  node_shift);
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
  unsigned            node_size_byte = node_size_avx2 * AVX2_SIZE_BYTE;
  span<uint8_t>       out_tmp        = out;
  span<const uint8_t> codeblock(codeblock_buffer);
  codeblock = codeblock.last(codeblock.size() - 2 * node_size_byte);
  for (unsigned i_node = 2, max_i_node = nof_nodes - 1; i_node != max_i_node; ++i_node) {
    srsvec::copy(out_tmp.first(lifting_size), codeblock.first(lifting_size));
    out_tmp   = out_tmp.last(out_tmp.size() - lifting_size);
    codeblock = codeblock.last(codeblock.size() - node_size_byte);
  }

  // Take care of the last node.
  unsigned remainder = out_tmp.size();
  srsvec::copy(out_tmp, codeblock.first(remainder));
}
