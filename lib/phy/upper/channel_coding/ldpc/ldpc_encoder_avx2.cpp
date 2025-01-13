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

#include "ldpc_encoder_avx2.h"
#include "avx2_support.h"
#include "srsran/srsvec/binary.h"
#include "srsran/srsvec/bit.h"
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

void ldpc_encoder_avx2::select_strategy()
{
  ldpc_base_graph_type current_bg       = current_graph->get_base_graph();
  uint8_t              current_ls_index = current_graph->get_lifting_index();

  // Each BG node contains LS bits, which are stored in node_size_avx2 AVX2 vectors.
  node_size_avx2 = divide_ceil(lifting_size, AVX2_SIZE_BYTE);

  systematic_bits = select_sys_bits_strategy<MAX_NODE_SIZE_AVX2>(current_bg, node_size_avx2);
  high_rate       = select_hr_strategy<MAX_NODE_SIZE_AVX2>(current_bg, current_ls_index, node_size_avx2);
}

void ldpc_encoder_avx2::load_input(const bit_buffer& in)
{
  unsigned node_size_byte = node_size_avx2 * AVX2_SIZE_BYTE;
  unsigned tail_bytes     = node_size_byte - lifting_size;

  // Set state variables depending on the codeblock length.
  codeblock_used_size = codeblock_length / lifting_size * node_size_avx2;
  length_extended     = (codeblock_length / lifting_size - bg_K) * node_size_avx2;

  span<uint8_t> codeblock = span<uint8_t>(codeblock_buffer).first((bg_K + bg_hr_parity_nodes) * node_size_byte);
  // Unpack the input bits into the CB buffer.
  for (unsigned i_node = 0; i_node != bg_K; ++i_node) {
    srsvec::bit_unpack(codeblock.first(lifting_size), in, i_node * lifting_size);
    codeblock = codeblock.last(codeblock.size() - lifting_size);

    srsvec::zero(codeblock.first(tail_bytes));
    codeblock = codeblock.last(codeblock.size() - tail_bytes);
  }
  srsvec::zero(codeblock);
}

// Computes the XOR logical operation (modulo-2 sum) between the contents of "in0" and "in1". The result is stored in
// "out". The representation is unpacked (1 byte represents 1 bit).
template <typename Type>
static void fast_xor(span<Type> out, span<const Type> in0, span<const Type> in1)
{
  static_assert(sizeof(Type) == 1, "Type size must be one byte.");
#if defined(__AVX512F__) && defined(__AVX512BW__)
  // Upgrades the XOR to AVX512 if it is possible.
  unsigned       nof_vectors = in0.size() / AVX512_SIZE_BYTE;
  const __m512i* in0_local   = reinterpret_cast<const __m512i*>(in0.data());
  const __m512i* in1_local   = reinterpret_cast<const __m512i*>(in1.data());
  __m512i*       out_local   = reinterpret_cast<__m512i*>(out.data());

  unsigned i = 0;
  for (unsigned i_vector = 0; i_vector != nof_vectors; ++i_vector, i += AVX512_SIZE_BYTE) {
    _mm512_storeu_si512(out_local++,
                        _mm512_xor_si512(_mm512_loadu_si512(in0_local++), _mm512_loadu_si512(in1_local++)));
  }

  uint64_t  remainder = in0.size() - i;
  __mmask64 mask      = (1UL << remainder) - 1UL;
  _mm512_mask_storeu_epi8(
      out_local,
      mask,
      _mm512_xor_si512(_mm512_maskz_loadu_epi8(mask, in0_local), _mm512_maskz_loadu_epi8(mask, in1_local)));
#else  // defined(__AVX512F__) && defined(__AVX512BW__)
  unsigned               nof_vectors = in0.size() / AVX2_SIZE_BYTE;
  mm256::avx2_const_span in0_local(in0, nof_vectors);
  mm256::avx2_const_span in1_local(in1, nof_vectors);
  mm256::avx2_span       out_local(out, nof_vectors);

  unsigned i = 0;
  for (unsigned i_vector = 0; i_vector != nof_vectors; ++i_vector, i += AVX2_SIZE_BYTE) {
    _mm256_storeu_si256(out_local.data_at(i_vector),
                        _mm256_xor_si256(in0_local.get_at(i_vector), in1_local.get_at(i_vector)));
  }

  for (unsigned i_end = out.size(); i != i_end; ++i) {
    out[i] = in0[i] ^ in1[i];
  }
#endif // defined(__AVX512F__) && defined(__AVX512BW__)
}

template <unsigned BG_K_PH, unsigned BG_M_PH, unsigned NODE_SIZE_AVX2_PH>
void ldpc_encoder_avx2::systematic_bits_inner()
{
  static constexpr unsigned node_size_byte = NODE_SIZE_AVX2_PH * AVX2_SIZE_BYTE;
  mm256::avx2_const_span    codeblock(codeblock_buffer, codeblock_used_size);
  mm256::avx2_span          auxiliary(auxiliary_buffer, bg_hr_parity_nodes * NODE_SIZE_AVX2_PH);

  std::array<int8_t, node_size_byte> tmp_blk = {};
  span<int8_t>                       blk     = span<int8_t>(tmp_blk);

  for (unsigned m = 0; m != bg_hr_parity_nodes; ++m) {
    const ldpc::BG_adjacency_row_t& row = current_graph->get_adjacency_row(m);

    for (uint16_t k : row) {
      if ((k >= bg_K) || (k == NO_EDGE)) {
        break;
      }

      unsigned node_shift = current_graph->get_lifted_node(m, k);
      if (node_shift == NO_EDGE) {
        break;
      }

      unsigned i_aux = m * NODE_SIZE_AVX2_PH;
      unsigned i_blk = k * NODE_SIZE_AVX2_PH;

      span<int8_t> plain_auxiliary = auxiliary.plain_span(i_aux, node_size_byte);

      if (k == row[0]) {
        srsvec::circ_shift_backward(
            plain_auxiliary.first(lifting_size), codeblock.plain_span(i_blk, lifting_size), node_shift);
      } else {
        srsvec::circ_shift_backward(blk.first(lifting_size), codeblock.plain_span(i_blk, lifting_size), node_shift);
        fast_xor<int8_t>(plain_auxiliary, blk, plain_auxiliary);
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

  mm256::avx2_span codeblock(codeblock_buffer, codeblock_used_size);

  mm256::avx2_const_span auxiliary(auxiliary_buffer, bg_hr_parity_nodes * NODE_SIZE_AVX2_PH);

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

  mm256::avx2_const_span auxiliary(auxiliary_buffer, bg_hr_parity_nodes * NODE_SIZE_AVX2_PH);

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

  mm256::avx2_const_span auxiliary(auxiliary_buffer, bg_hr_parity_nodes * NODE_SIZE_AVX2_PH);

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
    // Third chunk of parity bits.
    codeblock.set_at(skip2 + j, _mm256_xor_si256(auxiliary.get_at(NODE_SIZE_AVX2_PH + j), block_1));
    // Fourth chunk of parity bits.
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

  mm256::avx2_const_span auxiliary(auxiliary_buffer, bg_hr_parity_nodes * NODE_SIZE_AVX2_PH);

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

void ldpc_encoder_avx2::ext_region_inner(span<uint8_t> out_node, unsigned m) const
{
  unsigned            node_size_byte = node_size_avx2 * AVX2_SIZE_BYTE;
  span<const uint8_t> codeblock(codeblock_buffer);

  // Get the adjacency row for the node m.
  const ldpc::BG_adjacency_row_t& row = current_graph->get_adjacency_row(m);

  // Zero node.
  srsvec::zero(out_node);

  // Process each of the nodes.
  for (uint16_t k : row) {
    // Check if the node is in codeblock range.
    if ((k >= bg_K) || (k == NO_EDGE)) {
      break;
    }

    // Obtain the node cyclic shift.
    unsigned node_shift = current_graph->get_lifted_node(m, k);
    if (node_shift == NO_EDGE) {
      break;
    }

    // Get the view of the codeblock node.
    span<const uint8_t> codeblock_node = codeblock.subspan(k * node_size_byte, lifting_size);

    // The first time, the node is copied; afterwards combined.
    if (k == row[0]) {
      srsvec::circ_shift_backward(out_node, codeblock_node, node_shift);
    } else {
      fast_xor<uint8_t>(out_node.first(lifting_size - node_shift),
                        out_node.first(lifting_size - node_shift),
                        codeblock_node.last(lifting_size - node_shift));
      fast_xor<uint8_t>(out_node.last(node_shift), out_node.last(node_shift), codeblock_node.first(node_shift));
    }
  }

  // Process extended region.
  for (unsigned k = 0; k != bg_hr_parity_nodes; ++k) {
    unsigned node_shift = current_graph->get_lifted_node(m, bg_K + k);

    if (node_shift == NO_EDGE) {
      continue;
    }

    // At this point, the codeblock nodes in the extended region already contain the contribution from the systematic
    // bits (they were computed in systematic_bits_inner). All is left to do is to sum the contribution due to the
    // high-rate region (also stored in codeblock), with the proper circular shifts.
    span<const uint8_t> in_node = codeblock.subspan((bg_K + k) * node_size_byte, lifting_size);
    fast_xor<uint8_t>(out_node.first(lifting_size - node_shift),
                      out_node.first(lifting_size - node_shift),
                      in_node.last(lifting_size - node_shift));
    fast_xor<uint8_t>(out_node.last(node_shift), out_node.last(node_shift), in_node.first(node_shift));
  }
}

void ldpc_encoder_avx2::write_codeblock(span<uint8_t> out, unsigned offset) const
{
  srsran_assert(out.size() + offset <= bg_N_short * lifting_size,
                "The output size (i.e., {}) plus the offset (i.e., {}) exceeds the codeblock length (i.e., {}).",
                out.size(),
                offset,
                bg_N_short * lifting_size);

  // Calculate the node size in bytes, considering SIMD alignment.
  unsigned            node_size_byte = node_size_avx2 * AVX2_SIZE_BYTE;
  span<const uint8_t> codeblock(codeblock_buffer);

  // Select the initial node. The first two blocks are shortened and the last node is not considered, since it can be
  // incomplete.
  unsigned i_node_begin = 2 + offset / lifting_size;

  // Advance the codeblock to the initial node.
  codeblock = codeblock.last(codeblock.size() - i_node_begin * node_size_byte);

  // End node.
  unsigned i_node_end = i_node_begin + divide_ceil(out.size(), lifting_size);

  // Calculate the offset within the first node.
  offset = offset % lifting_size;

  for (unsigned i_node = i_node_begin; i_node != i_node_end; ++i_node) {
    // Determine the number of bits to read from this node.
    unsigned count = std::min(lifting_size - offset, static_cast<unsigned>(out.size()));

    // Check if it is operating in the extended region.
    if (i_node < bg_hr_parity_nodes + bg_K) {
      srsvec::copy(out.first(count), codeblock.subspan(offset, count));
    } else {
      // Detect if the extended region can be done directly on the output.
      bool inplace = ((offset == 0) && (count == lifting_size));

      // Create temporary data.
      static_vector<uint8_t, MAX_LIFTING_SIZE> temp(lifting_size);
      span<uint8_t>                            node_view = temp;

      // Use direct output for the extended node.
      if (inplace) {
        node_view = out.first(lifting_size);
      }

      // Process extended region for the given node.
      ext_region_inner(node_view, i_node - bg_K);

      // Copy data if temporal buffer was used.
      if (!inplace) {
        srsvec::copy(out.first(count), node_view.subspan(offset, count));
      }
    }

    // Advance codeblock.
    codeblock = codeblock.last(codeblock.size() - node_size_byte);

    // Advance output.
    out = out.last(out.size() - count);

    // The offset is no longer required after the first node.
    offset = 0;
  }
}
