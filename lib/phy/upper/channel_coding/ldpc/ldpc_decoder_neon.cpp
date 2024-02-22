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
/// \brief LDPC decoder - definition of the NEON-optimized implementation.

#include "ldpc_decoder_neon.h"
#include "ldpc_graph_impl.h"
#include "neon_support.h"
#include "srsran/srsvec/circ_shift.h"
#include "srsran/srsvec/copy.h"

using namespace srsran;
using namespace srsran::ldpc;

// NEON vectors filled with useful constants.
static __always_inline int8x16_t LLR_MAX_s8()
{
  return vdupq_n_s8(LLR_MAX.to_value_type());
}

static __always_inline int8x16_t LLR_MIN_s8()
{
  return vdupq_n_s8(LLR_MIN.to_value_type());
}

static __always_inline int8x16_t LLR_INFINITY_s8()
{
  return vdupq_n_s8(LLR_INFINITY.to_value_type());
}

static __always_inline int8x16_t LLR_NEG_INFINITY_s8()
{
  return vdupq_n_s8(-LLR_INFINITY.to_value_type());
}

/// Maximum number of NEON vectors needed to represent a BG node.
static constexpr unsigned MAX_NODE_SIZE_NEON = divide_ceil(ldpc::MAX_LIFTING_SIZE, NEON_SIZE_BYTE);

void ldpc_decoder_neon::specific_init()
{
  // Each BG node contains LS bits, which are stored in node_size_neon NEON vectors.
  node_size_neon = divide_ceil(lifting_size, NEON_SIZE_BYTE);
  node_size_byte = node_size_neon * NEON_SIZE_BYTE;
}

void ldpc_decoder_neon::compute_var_to_check_msgs(span<log_likelihood_ratio>       this_var_to_check,
                                                  span<const log_likelihood_ratio> this_soft_bits,
                                                  span<const log_likelihood_ratio> this_check_to_var)
{
  srsran_srsvec_assert_size(this_check_to_var, this_soft_bits);
  srsran_srsvec_assert_size(this_check_to_var, this_var_to_check);

  // Compute the number of nodes of the lifted graph represented by the spans.
  unsigned neon_blocks = this_var_to_check.size() / NEON_SIZE_BYTE;

  srsran_assert(this_var_to_check.size() == neon_blocks * NEON_SIZE_BYTE,
                "The spans do not correspond to an integer number of AVX2 registers.");
  neon::neon_span       var_to_check_neon(this_var_to_check, neon_blocks);
  neon::neon_const_span soft_bits_neon(this_soft_bits, neon_blocks);
  neon::neon_const_span check_to_var_neon(this_check_to_var, neon_blocks);

  // By definition, the difference between two LLRs saturates at +/- LLR_MAX. Moreover, if either term is infinite, so
  // is the result, with proper sign.
  for (unsigned i_block = 0; i_block != neon_blocks; ++i_block) {
    int8x16_t soft_s8 = soft_bits_neon.get_at(i_block);
    int8x16_t c2v_s8  = check_to_var_neon.get_at(i_block);

    // v2c = (soft - c2v > LLR_MAX) ? LLR_MAX : (soft - c2v)
    int8x16_t help_sub_s8 = vsubq_s8(soft_s8, c2v_s8);
    int8x16_t v2c_s8      = vminq_s8(help_sub_s8, LLR_MAX_s8());

    // v2c = (v2c < MIN_LLR) ? MIN_LLR : v2c
    v2c_s8 = vmaxq_s8(v2c_s8, LLR_MIN_s8());

    // Ensure that soft = +/- infinity implies v2c = +/- infinity.
    // v2c = (soft < infinity) ? v2c : infinity
    uint8x16_t mask_u8 = vcgtq_s8(LLR_INFINITY_s8(), soft_s8);
    v2c_s8             = vbslq_s8(mask_u8, v2c_s8, LLR_INFINITY_s8());

    // v2c = (soft > - infinity) ? v2c : - infinity
    mask_u8 = vcgtq_s8(soft_s8, LLR_NEG_INFINITY_s8());
    v2c_s8  = vbslq_s8(mask_u8, v2c_s8, LLR_NEG_INFINITY_s8());
    var_to_check_neon.set_at(i_block, v2c_s8);
  }
}

void ldpc_decoder_neon::analyze_var_to_check_msgs(span<log_likelihood_ratio>       min_var_to_check,
                                                  span<log_likelihood_ratio>       second_min_var_to_check,
                                                  span<uint8_t>                    min_var_to_check_index,
                                                  span<uint8_t>                    sign_prod_var_to_check,
                                                  span<const log_likelihood_ratio> rotated_node,
                                                  unsigned                         var_node)
{
  neon::neon_span       min_var_to_check_neon(min_var_to_check, node_size_neon);
  neon::neon_span       second_min_var_to_check_neon(second_min_var_to_check, node_size_neon);
  neon::neon_span       min_var_to_check_index_neon(min_var_to_check_index, node_size_neon);
  neon::neon_span       sign_prod_var_to_check_neon(sign_prod_var_to_check, node_size_neon);
  neon::neon_const_span rotated_node_neon(rotated_node, node_size_neon);

  int8x16_t this_var_index_s8 = vdupq_n_s8(static_cast<int8_t>(var_node));
  // For all NEON registers inside a node...
  for (unsigned i_block = 0; i_block != node_size_neon; ++i_block) {
    // Store the sign of the cumulative product of all variable nodes.
    int8x16_t sign_prod_s8 = sign_prod_var_to_check_neon.get_at(i_block);
    sign_prod_s8           = veorq_s8(sign_prod_s8, rotated_node_neon.get_at(i_block));
    sign_prod_var_to_check_neon.set_at(i_block, sign_prod_s8);

    // Find the minimum variable-to-check message (in absolute value).
    int8x16_t  abs_var_to_check_s8 = vabsq_s8(rotated_node_neon.get_at(i_block));
    int8x16_t  current_min_s8      = min_var_to_check_neon.get_at(i_block);
    uint8x16_t mask_min_u8         = vcgtq_s8(current_min_s8, abs_var_to_check_s8);
    // The following quantity is needed to find the second-smallest message.
    int8x16_t help_min_s8 = vbslq_s8(mask_min_u8, current_min_s8, abs_var_to_check_s8);
    current_min_s8        = vbslq_s8(mask_min_u8, abs_var_to_check_s8, current_min_s8);
    min_var_to_check_neon.set_at(i_block, current_min_s8);

    // Store the index corresponding to the minimum.
    int8x16_t min_var_to_check_index_s8 = min_var_to_check_index_neon.get_at(i_block);
    min_var_to_check_index_s8           = vbslq_s8(mask_min_u8, this_var_index_s8, min_var_to_check_index_s8);
    min_var_to_check_index_neon.set_at(i_block, min_var_to_check_index_s8);

    // Find the second-minimum value.
    int8x16_t current_second_min_s8 = second_min_var_to_check_neon.get_at(i_block);
    mask_min_u8                     = vcgtq_s8(current_second_min_s8, abs_var_to_check_s8);
    current_second_min_s8           = vbslq_s8(mask_min_u8, help_min_s8, current_second_min_s8);
    second_min_var_to_check_neon.set_at(i_block, current_second_min_s8);
  }
}

void ldpc_decoder_neon::compute_check_to_var_msgs(span<log_likelihood_ratio> this_check_to_var,
                                                  span<const log_likelihood_ratio> /*this_var_to_check*/,
                                                  span<const log_likelihood_ratio> rotated_node,
                                                  span<const log_likelihood_ratio> min_var_to_check,
                                                  span<const log_likelihood_ratio> second_min_var_to_check,
                                                  span<const uint8_t>              min_var_to_check_index,
                                                  span<const uint8_t>              sign_prod_var_to_check,
                                                  unsigned                         shift,
                                                  unsigned                         var_node)
{
  neon::neon_const_span min_var_to_check_neon(min_var_to_check, node_size_neon);
  neon::neon_const_span second_min_var_to_check_neon(second_min_var_to_check, node_size_neon);
  neon::neon_const_span min_var_to_check_index_neon(min_var_to_check_index, node_size_neon);
  neon::neon_const_span sign_prod_var_to_check_neon(sign_prod_var_to_check, node_size_neon);
  neon::neon_const_span rotated_node_neon(rotated_node, node_size_neon);

  std::array<log_likelihood_ratio, MAX_NODE_SIZE_NEON * NEON_SIZE_BYTE> help_check_to_var;
  neon::neon_span help_check_to_var_neon(help_check_to_var, node_size_neon);

  int8x16_t this_var_index_s8 = vdupq_n_s8(static_cast<int8_t>(var_node));
  for (unsigned i_block = 0; i_block != node_size_neon; ++i_block) {
    // The check-to-variable message is the minimum of all variable-to-check messages but the current one: that is, if
    // i_block corresponds to the minimum message, take the second-minimum.
    uint8x16_t mask_is_min_u8 = vceqq_s8(this_var_index_s8, min_var_to_check_index_neon.get_at(i_block));
    int8x16_t  check_to_var_s8 =
        vbslq_s8(mask_is_min_u8, second_min_var_to_check_neon.get_at(i_block), min_var_to_check_neon.get_at(i_block));
    // Scale the message to compensate for approximations.
    check_to_var_s8 = neon::scale_s8(check_to_var_s8, scaling_factor, log_likelihood_ratio::max().to_value_type());

    // Sign of the cumulative product of all variable-to-check messages but the current one (same as multiplying the
    // sign of all messages by the sign of the current one).
    int8x16_t  final_sign_s8 = veorq_s8(rotated_node_neon.get_at(i_block), sign_prod_var_to_check_neon.get_at(i_block));
    uint8x16_t sign_mask     = vreinterpretq_u8_s8(vshrq_n_s8(final_sign_s8, 7));

    // Assign the sign to the message.
    int8x16_t check_to_var_neg_s8 = vnegq_s8(check_to_var_s8);
    help_check_to_var_neon.set_at(i_block, vbslq_s8(sign_mask, check_to_var_neg_s8, check_to_var_s8));
  }

  // Rotate the message back before storing it.
  srsvec::circ_shift_forward(
      this_check_to_var.first(lifting_size), span<log_likelihood_ratio>(help_check_to_var).first(lifting_size), shift);
}

void ldpc_decoder_neon::compute_soft_bits(span<log_likelihood_ratio>       this_soft_bits,
                                          span<const log_likelihood_ratio> this_var_to_check,
                                          span<const log_likelihood_ratio> this_check_to_var)
{
  neon::neon_const_span this_check_to_var_neon(this_check_to_var, node_size_neon);
  neon::neon_const_span this_var_to_check_neon(this_var_to_check, node_size_neon);
  neon::neon_span       this_soft_bits_neon(this_soft_bits, node_size_neon);

  for (unsigned i_block = 0; i_block != node_size_neon; ++i_block) {
    // Get a mask of the infinite check-to-var messages.
    int8x16_t  check_to_var_s8              = this_check_to_var_neon.get_at(i_block);
    uint8x16_t is_check_plus_infty_mask_u8  = vcgtq_s8(check_to_var_s8, LLR_MAX_s8());
    uint8x16_t is_check_minus_infty_mask_u8 = vcgtq_s8(LLR_MIN_s8(), check_to_var_s8);
    // Get a mask of the infinite var-to_check messages.
    int8x16_t  var_to_check_s8            = this_var_to_check_neon.get_at(i_block);
    uint8x16_t is_var_plus_infty_mask_u8  = vcgtq_s8(var_to_check_s8, LLR_MAX_s8());
    uint8x16_t is_var_minus_infty_mask_u8 = vcgtq_s8(LLR_MIN_s8(), var_to_check_s8);
    // Add check-to-variable and variable-to-check messages (note: we use saturated addition).
    int8x16_t soft_s8 = vqaddq_s8(check_to_var_s8, var_to_check_s8);

    // Soft bits are +INFINITY if they are larger than LLR_MAX or if at least one of the addends is (but not if the two
    // addends are both infinity but with opposite signs).
    uint8x16_t mask_u8        = vcgtq_s8(soft_s8, LLR_MAX_s8());
    uint8x16_t mask_inputs_u8 = vandq_u8(vmvnq_u8(is_var_minus_infty_mask_u8), is_check_plus_infty_mask_u8);
    uint8x16_t mask_tmp_u8    = vandq_u8(vmvnq_u8(is_check_minus_infty_mask_u8), is_var_plus_infty_mask_u8);
    mask_inputs_u8            = vorrq_u8(mask_inputs_u8, mask_tmp_u8);
    mask_u8                   = vorrq_u8(mask_u8, mask_inputs_u8);
    soft_s8                   = vbslq_s8(mask_u8, LLR_INFINITY_s8(), soft_s8);

    // Soft bits are -INFINITY if they are smaller than LLR_MIN or if at least one of the addends is (but not if the two
    // addends are both infinity but with opposite signs).
    mask_u8        = vcgtq_s8(LLR_MIN_s8(), soft_s8);
    mask_inputs_u8 = vandq_u8(vmvnq_u8(is_var_plus_infty_mask_u8), is_check_minus_infty_mask_u8);
    mask_tmp_u8    = vandq_u8(vmvnq_u8(is_check_plus_infty_mask_u8), is_var_minus_infty_mask_u8);
    mask_inputs_u8 = vorrq_u8(mask_inputs_u8, mask_tmp_u8);
    mask_u8        = vorrq_u8(mask_u8, mask_inputs_u8);
    this_soft_bits_neon.set_at(i_block, vbslq_s8(mask_u8, LLR_NEG_INFINITY_s8(), soft_s8));
  }
}

bool ldpc_decoder_neon::get_hard_bits(bit_buffer& out)
{
  // Buffer to hold the soft bits.
  std::array<log_likelihood_ratio, MAX_LIFTING_SIZE * MAX_BG_K> temp_llr;
  span<log_likelihood_ratio>                                    llr_write_buffer(temp_llr);

  unsigned              max_node_lifted = bg_K * node_size_neon;
  neon::neon_const_span soft_bits_neon(soft_bits, max_node_lifted);

  // Copy the LLRs from the soft_bits AVX array into temp_llr without any padding. Recall that temp_llr is aligned to
  // the size of the AVX registers. This makes it possible to call the hard decision function only once, improving
  // efficiency.
  for (unsigned i_node_lifted = 0; i_node_lifted != max_node_lifted; i_node_lifted += node_size_neon) {
    // View over the LLR.
    span<const log_likelihood_ratio> current_soft(
        reinterpret_cast<const log_likelihood_ratio*>(soft_bits_neon.data_at(i_node_lifted, 0)), lifting_size);

    // Append LLRs to the buffer.
    srsvec::copy(llr_write_buffer.first(lifting_size), current_soft);

    // Advance buffer.
    llr_write_buffer = llr_write_buffer.last(llr_write_buffer.size() - lifting_size);
  }

  // View over the appended LLR.
  span<const log_likelihood_ratio> llr_read_buffer(temp_llr.begin(), llr_write_buffer.begin());

  // Convert to hard bits.
  return hard_decision(out, llr_read_buffer);
}