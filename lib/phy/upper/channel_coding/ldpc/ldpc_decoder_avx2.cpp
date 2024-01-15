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

/// \file
/// \brief LDPC decoder - definition of the AVX2-optimized implementation.

#include "ldpc_decoder_avx2.h"
#include "avx2_support.h"
#include "ldpc_graph_impl.h"
#include "srsran/srsvec/circ_shift.h"
#include "srsran/srsvec/copy.h"

using namespace srsran;
using namespace srsran::ldpc;

// AVX2 vectors filled with useful constants.
static __always_inline __m256i LLR_MAX_epi8()
{
  return _mm256_set1_epi8(LLR_MAX.to_value_type());
}

static __always_inline __m256i LLR_MIN_epi8()
{
  return _mm256_set1_epi8(LLR_MIN.to_value_type());
}

static __always_inline __m256i LLR_INFINITY_epi8()
{
  return _mm256_set1_epi8(LLR_INFINITY.to_value_type());
}

static __always_inline __m256i LLR_NEG_INFINITY_epi8()
{
  return _mm256_set1_epi8(-LLR_INFINITY.to_value_type());
}

/// Maximum number of AVX2 vectors needed to represent a BG node.
static constexpr unsigned MAX_NODE_SIZE_AVX2 = divide_ceil(ldpc::MAX_LIFTING_SIZE, AVX2_SIZE_BYTE);

void ldpc_decoder_avx2::specific_init()
{
  // Each BG node contains LS bits, which are stored in node_size_avx2 AVX2 vectors.
  node_size_avx2 = divide_ceil(lifting_size, AVX2_SIZE_BYTE);
  node_size_byte = node_size_avx2 * AVX2_SIZE_BYTE;
}

void ldpc_decoder_avx2::compute_var_to_check_msgs(span<log_likelihood_ratio>       this_var_to_check,
                                                  span<const log_likelihood_ratio> this_soft_bits,
                                                  span<const log_likelihood_ratio> this_check_to_var)
{
  srsran_srsvec_assert_size(this_check_to_var, this_soft_bits);
  srsran_srsvec_assert_size(this_check_to_var, this_var_to_check);

  // Compute the number of nodes of the lifted graph represented by the spans.
  unsigned avx2_blocks = this_var_to_check.size() / AVX2_SIZE_BYTE;

  srsran_assert(this_var_to_check.size() == avx2_blocks * AVX2_SIZE_BYTE,
                "The spans do not correspond to an integer number of AVX2 registers.");

  mm256::avx2_span       var_to_check_avx2(this_var_to_check, avx2_blocks);
  mm256::avx2_const_span soft_bits_avx2(this_soft_bits, avx2_blocks);
  mm256::avx2_const_span check_to_var_avx2(this_check_to_var, avx2_blocks);

  // By definition, the difference between two LLRs saturates at +/- LLR_MAX. Moreover, if either term is infinite, so
  // is the result, with proper sign.
  for (unsigned i_block = 0; i_block != avx2_blocks; ++i_block) {
    __m256i soft_epi8 = soft_bits_avx2.get_at(i_block);
    __m256i c2v_epi8  = check_to_var_avx2.get_at(i_block);

    // v2c = (soft - c2v > LLR_MAX) ? LLR_MAX : (soft - c2v)
    __m256i help_sub_epi8 = _mm256_subs_epi8(soft_epi8, c2v_epi8);
    __m256i v2c_epi8      = _mm256_min_epi8(help_sub_epi8, LLR_MAX_epi8());

    // v2c = (v2c < MIN_LLR) ? MIN_LLR : v2c
    v2c_epi8 = _mm256_max_epi8(v2c_epi8, LLR_MIN_epi8());

    // Ensure that soft = +/- infinity implies v2c = +/- infinity.
    // v2c = (soft < infinity) ? v2c : infinity
    __m256i mask_epi8 = _mm256_cmpgt_epi8(LLR_INFINITY_epi8(), soft_epi8);
    v2c_epi8          = _mm256_blendv_epi8(LLR_INFINITY_epi8(), v2c_epi8, mask_epi8);

    // v2c = (soft > - infinity) ? v2c : - infinity
    mask_epi8 = _mm256_cmpgt_epi8(soft_epi8, LLR_NEG_INFINITY_epi8());
    v2c_epi8  = _mm256_blendv_epi8(LLR_NEG_INFINITY_epi8(), v2c_epi8, mask_epi8);
    var_to_check_avx2.set_at(i_block, v2c_epi8);
  }
}

span<log_likelihood_ratio> ldpc_decoder_avx2::get_rotated_node(unsigned var_node)
{
  return span<log_likelihood_ratio>(rotated_var_to_check).subspan(var_node * node_size_byte, node_size_byte);
}

void ldpc_decoder_avx2::analyze_var_to_check_msgs(span<log_likelihood_ratio>       min_var_to_check,
                                                  span<log_likelihood_ratio>       second_min_var_to_check,
                                                  span<uint8_t>                    min_var_to_check_index,
                                                  span<uint8_t>                    sign_prod_var_to_check,
                                                  span<log_likelihood_ratio>       rotated_node,
                                                  span<const log_likelihood_ratio> this_var_to_check,
                                                  unsigned                         shift,
                                                  unsigned                         var_node)
{
  srsvec::circ_shift_backward(rotated_node.first(lifting_size), this_var_to_check.first(lifting_size), shift);

  mm256::avx2_span min_var_to_check_avx2(min_var_to_check, node_size_avx2);
  mm256::avx2_span second_min_var_to_check_avx2(second_min_var_to_check, node_size_avx2);
  mm256::avx2_span min_var_to_check_index_avx2(min_var_to_check_index, node_size_avx2);
  mm256::avx2_span sign_prod_var_to_check_avx2(sign_prod_var_to_check, node_size_avx2);
  mm256::avx2_span rotated_node_avx2(rotated_node, node_size_avx2);

  __m256i this_var_index_epi8 = _mm256_set1_epi8(static_cast<int8_t>(var_node));
  // For all AVX2 registers inside a node...
  for (unsigned i_block = 0; i_block != node_size_avx2; ++i_block) {
    // Store the sign of the cumulative product of all variable nodes.
    __m256i sign_prod_epi8 = sign_prod_var_to_check_avx2.get_at(i_block);
    sign_prod_epi8         = _mm256_xor_si256(sign_prod_epi8, rotated_node_avx2.get_at(i_block));
    sign_prod_var_to_check_avx2.set_at(i_block, sign_prod_epi8);

    // Find the minimum variable-to-check message (in absolute value).
    __m256i abs_var_to_check_epi8 = _mm256_abs_epi8(rotated_node_avx2.get_at(i_block));
    __m256i current_min_epi8      = min_var_to_check_avx2.get_at(i_block);
    __m256i mask_min_epi8         = _mm256_cmpgt_epi8(current_min_epi8, abs_var_to_check_epi8);
    // The following quantity is needed to find the second-smallest message.
    __m256i help_min_epi8 = _mm256_blendv_epi8(abs_var_to_check_epi8, current_min_epi8, mask_min_epi8);
    current_min_epi8      = _mm256_blendv_epi8(current_min_epi8, abs_var_to_check_epi8, mask_min_epi8);
    min_var_to_check_avx2.set_at(i_block, current_min_epi8);

    // Store the index corresponding to the minimum.
    __m256i min_var_to_check_index_epi8 = min_var_to_check_index_avx2.get_at(i_block);
    min_var_to_check_index_epi8 = _mm256_blendv_epi8(min_var_to_check_index_epi8, this_var_index_epi8, mask_min_epi8);
    min_var_to_check_index_avx2.set_at(i_block, min_var_to_check_index_epi8);

    // Find the second-minimum value.
    __m256i current_second_min_epi8 = second_min_var_to_check_avx2.get_at(i_block);
    mask_min_epi8                   = _mm256_cmpgt_epi8(current_second_min_epi8, abs_var_to_check_epi8);
    current_second_min_epi8         = _mm256_blendv_epi8(current_second_min_epi8, help_min_epi8, mask_min_epi8);
    second_min_var_to_check_avx2.set_at(i_block, current_second_min_epi8);
  }
}

void ldpc_decoder_avx2::compute_check_to_var_msgs(span<log_likelihood_ratio> this_check_to_var,
                                                  span<const log_likelihood_ratio> /*this_var_to_check*/,
                                                  span<const log_likelihood_ratio> rotated_node,
                                                  span<const log_likelihood_ratio> min_var_to_check,
                                                  span<const log_likelihood_ratio> second_min_var_to_check,
                                                  span<const uint8_t>              min_var_to_check_index,
                                                  span<const uint8_t>              sign_prod_var_to_check,
                                                  unsigned                         shift,
                                                  unsigned                         var_node)
{
  mm256::avx2_const_span min_var_to_check_avx2(min_var_to_check, node_size_avx2);
  mm256::avx2_const_span second_min_var_to_check_avx2(second_min_var_to_check, node_size_avx2);
  mm256::avx2_const_span min_var_to_check_index_avx2(min_var_to_check_index, node_size_avx2);
  mm256::avx2_const_span sign_prod_var_to_check_avx2(sign_prod_var_to_check, node_size_avx2);
  mm256::avx2_const_span rotated_node_avx2(rotated_node, node_size_avx2);

  std::array<log_likelihood_ratio, MAX_NODE_SIZE_AVX2 * AVX2_SIZE_BYTE> help_check_to_var;
  mm256::avx2_span help_check_to_var_avx2(help_check_to_var, node_size_avx2);

  __m256i this_var_index_epi8 = _mm256_set1_epi8(static_cast<int8_t>(var_node));
  for (unsigned i_block = 0; i_block != node_size_avx2; ++i_block) {
    // The check-to-variable message is the minimum of all variable-to-check messages but the current one: that is, if
    // i_block corresponds to the minimum message, take the second-minimum.
    __m256i mask_is_min_epi8  = _mm256_cmpeq_epi8(this_var_index_epi8, min_var_to_check_index_avx2.get_at(i_block));
    __m256i check_to_var_epi8 = _mm256_blendv_epi8(
        min_var_to_check_avx2.get_at(i_block), second_min_var_to_check_avx2.get_at(i_block), mask_is_min_epi8);
    // Scale the message to compensate for approximations.
    check_to_var_epi8 =
        mm256::scale_epi8(check_to_var_epi8, scaling_factor, log_likelihood_ratio::max().to_value_type());

    // Sign of the cumulative product of all variable-to-check messages but the current one (same as multiplying the
    // sign of all messages by the sign of the current one).
    __m256i final_sign_epi8 =
        _mm256_xor_si256(rotated_node_avx2.get_at(i_block), sign_prod_var_to_check_avx2.get_at(i_block));

    // Assign the sign to the message.
    __m256i help_epi8 = _mm256_sign_epi8(check_to_var_epi8, final_sign_epi8);
    help_check_to_var_avx2.set_at(i_block, _mm256_blendv_epi8(check_to_var_epi8, help_epi8, final_sign_epi8));
  }

  // Rotate the message back before storing it.
  srsvec::circ_shift_forward(
      this_check_to_var.first(lifting_size), span<log_likelihood_ratio>(help_check_to_var).first(lifting_size), shift);
}

void ldpc_decoder_avx2::compute_soft_bits(span<log_likelihood_ratio>       this_soft_bits,
                                          span<const log_likelihood_ratio> this_var_to_check,
                                          span<const log_likelihood_ratio> this_check_to_var)
{
  mm256::avx2_const_span this_check_to_var_avx2(this_check_to_var, node_size_avx2);
  mm256::avx2_const_span this_var_to_check_avx2(this_var_to_check, node_size_avx2);
  mm256::avx2_span       this_soft_bits_avx2(this_soft_bits, node_size_avx2);

  for (unsigned i_block = 0; i_block != node_size_avx2; ++i_block) {
    // Get a mask of the infinite check-to-var messages.
    __m256i check_to_var_epi8         = this_check_to_var_avx2.get_at(i_block);
    __m256i is_check_plus_infty_epi8  = _mm256_cmpgt_epi8(check_to_var_epi8, LLR_MAX_epi8());
    __m256i is_check_minus_infty_epi8 = _mm256_cmpgt_epi8(LLR_MIN_epi8(), check_to_var_epi8);
    // Get a mask of the infinite var-to_check messages.
    __m256i var_to_check_epi8       = this_var_to_check_avx2.get_at(i_block);
    __m256i is_var_plus_infty_epi8  = _mm256_cmpgt_epi8(var_to_check_epi8, LLR_MAX_epi8());
    __m256i is_var_minus_infty_epi8 = _mm256_cmpgt_epi8(LLR_MIN_epi8(), var_to_check_epi8);
    // Add check-to-variable and variable-to-check messages.
    __m256i soft_epi8 = _mm256_adds_epi8(check_to_var_epi8, this_var_to_check_avx2.get_at(i_block));

    // Soft bits are +INFINITY if they are larger than LLR_MAX or if at least one of the addends is (but not if the
    // two addends are both infinity but with opposite signs).
    __m256i mask_epi8        = _mm256_cmpgt_epi8(soft_epi8, LLR_MAX_epi8());
    __m256i mask_inputs_epi8 = _mm256_andnot_si256(is_var_minus_infty_epi8, is_check_plus_infty_epi8);
    __m256i mask_tmp_epi8    = _mm256_andnot_si256(is_check_minus_infty_epi8, is_var_plus_infty_epi8);
    mask_inputs_epi8         = _mm256_or_si256(mask_inputs_epi8, mask_tmp_epi8);
    mask_epi8                = _mm256_or_si256(mask_epi8, mask_inputs_epi8);
    soft_epi8                = _mm256_blendv_epi8(soft_epi8, LLR_INFINITY_epi8(), mask_epi8);

    // Soft bits are -INFINITY if they are smaller than LLR_MIN or if at least one of the addends is (but not if the
    // two addends are both infinity but with opposite signs).
    mask_epi8        = _mm256_cmpgt_epi8(LLR_MIN_epi8(), soft_epi8);
    mask_inputs_epi8 = _mm256_andnot_si256(is_var_plus_infty_epi8, is_check_minus_infty_epi8);
    mask_tmp_epi8    = _mm256_andnot_si256(is_check_plus_infty_epi8, is_var_minus_infty_epi8);
    mask_inputs_epi8 = _mm256_or_si256(mask_inputs_epi8, mask_tmp_epi8);
    mask_epi8        = _mm256_or_si256(mask_epi8, mask_inputs_epi8);
    this_soft_bits_avx2.set_at(i_block, _mm256_blendv_epi8(soft_epi8, LLR_NEG_INFINITY_epi8(), mask_epi8));
  }
}

void ldpc_decoder_avx2::get_hard_bits(bit_buffer& out)
{
  // Buffer to hold the soft bits.
  std::array<log_likelihood_ratio, MAX_LIFTING_SIZE * MAX_BG_K> temp_llr;
  span<log_likelihood_ratio>                                    llr_write_buffer(temp_llr);

  unsigned               max_node_lifted = bg_K * node_size_avx2;
  mm256::avx2_const_span soft_bits_avx2(soft_bits, max_node_lifted);

  // Copy the LLRs from the soft_bits AVX array into temp_llr without any padding. Recall that temp_llr is aligned to
  // the size of the AVX registers. This makes it possible to call the hard decision function only once, improving
  // efficiency.
  for (unsigned i_node_lifted = 0; i_node_lifted != max_node_lifted; i_node_lifted += node_size_avx2) {
    // View over the LLR.
    span<const log_likelihood_ratio> current_soft(
        reinterpret_cast<const log_likelihood_ratio*>(soft_bits_avx2.data_at(i_node_lifted, 0)), lifting_size);

    // Append LLRs to the buffer.
    srsvec::copy(llr_write_buffer.first(lifting_size), current_soft);

    // Advance buffer.
    llr_write_buffer = llr_write_buffer.last(llr_write_buffer.size() - lifting_size);
  }

  // View over the appended LLR.
  span<const log_likelihood_ratio> llr_read_buffer(temp_llr.begin(), llr_write_buffer.begin());

  // Convert to hard bits.
  hard_decision(out, llr_read_buffer);
}