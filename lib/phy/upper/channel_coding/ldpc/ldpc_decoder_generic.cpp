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

#include "ldpc_decoder_generic.h"
#include "srsran/srsvec/copy.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;
using namespace srsran::ldpc;

void ldpc_decoder_generic::compute_var_to_check_msgs(span<log_likelihood_ratio>       this_var_to_check,
                                                     span<const log_likelihood_ratio> this_soft_bits,
                                                     span<const log_likelihood_ratio> this_check_to_var)
{
  srsran_srsvec_assert_size(this_var_to_check, this_soft_bits);
  srsran_srsvec_assert_size(this_var_to_check, this_check_to_var);

  // By definition, the difference between two LLRs saturates at +/- LLR_MAX. Moreover, if either term is infinite, so
  // is the result, with proper sign.
  std::transform(this_soft_bits.begin(),
                 this_soft_bits.end(),
                 this_check_to_var.begin(),
                 this_var_to_check.begin(),
                 [](log_likelihood_ratio a, log_likelihood_ratio b) { return a - b; });
}

span<log_likelihood_ratio> ldpc_decoder_generic::get_rotated_node(unsigned /*i_loop*/)
{
  // In the generic implementation we don't physically rotate the node, since we can access the corresponding values by
  // a simple shift. We return an empty span.
  return {};
}

void ldpc_decoder_generic::analyze_var_to_check_msgs(span<log_likelihood_ratio> min_var_to_check,
                                                     span<log_likelihood_ratio> second_min_var_to_check,
                                                     span<uint8_t>              min_var_to_check_index,
                                                     span<uint8_t>              sign_prod_var_to_check,
                                                     span<log_likelihood_ratio> /*rotated_node*/,
                                                     span<const log_likelihood_ratio> this_var_to_check,
                                                     unsigned                         shift,
                                                     unsigned                         var_node)
{
  // Look for the two var_to_check messages with minimum absolute value and compute the sign product of all
  // var_to_check messages.
  for (unsigned j = 0; j != lifting_size; ++j) {
    log_likelihood_ratio var_to_check_abs = log_likelihood_ratio::abs(this_var_to_check[j]);
    unsigned             tmp_index        = (j + lifting_size - shift) % lifting_size;
    bool                 is_min           = (var_to_check_abs < min_var_to_check[tmp_index]);
    log_likelihood_ratio new_second_min   = is_min ? min_var_to_check[tmp_index] : var_to_check_abs;
    bool                 is_best_two      = (var_to_check_abs < second_min_var_to_check[tmp_index]);
    second_min_var_to_check[tmp_index]    = is_best_two ? new_second_min : second_min_var_to_check[tmp_index];
    min_var_to_check[tmp_index]           = is_min ? var_to_check_abs : min_var_to_check[tmp_index];
    min_var_to_check_index[tmp_index]     = is_min ? var_node : min_var_to_check_index[tmp_index];

    // For consistency with the optimized implementations, we store 0 if the sign is positive and 1 if the sign is
    // negative.
    sign_prod_var_to_check[tmp_index] ^= (this_var_to_check[j] >= 0) ? 0U : 1U;
  }
}

static log_likelihood_ratio scale_llr(log_likelihood_ratio llr, float scaling_factor)
{
  srsran_assert((scaling_factor > 0) && (scaling_factor < 1), "Scaling factor should be in the interval (0, 1).");
  if (log_likelihood_ratio::isinf(llr)) {
    return llr;
  }
  // Since scaling_factor belongs to (0, 1), there is no risk of overflow.
  return static_cast<log_likelihood_ratio::value_type>(
      std::round(static_cast<float>(llr.to_value_type()) * scaling_factor));
}

// In the generic implementation we don't physically rotate the node, since we can access the corresponding values by
// a simple shift - therefore the unused parameter.
void ldpc_decoder_generic::compute_check_to_var_msgs(span<log_likelihood_ratio>       this_check_to_var,
                                                     span<const log_likelihood_ratio> this_var_to_check,
                                                     span<const log_likelihood_ratio> /*rotated_node*/,
                                                     span<const log_likelihood_ratio> min_var_to_check,
                                                     span<const log_likelihood_ratio> second_min_var_to_check,
                                                     span<const uint8_t>              min_var_to_check_index,
                                                     span<const uint8_t>              sign_prod_var_to_check,
                                                     unsigned                         shift,
                                                     unsigned                         var_node)
{
  for (unsigned j = 0; j != lifting_size; ++j) {
    unsigned tmp_index = (j + lifting_size - shift) % lifting_size;

    this_check_to_var[j] = (var_node != min_var_to_check_index[tmp_index]) ? min_var_to_check[tmp_index]
                                                                           : second_min_var_to_check[tmp_index];

    this_check_to_var[j] = scale_llr(this_check_to_var[j], scaling_factor);

    // For consistency with the optimized implementations, we store 0 if the sign is positive and 1 if the sign is
    // negative.
    uint8_t final_sign   = sign_prod_var_to_check[tmp_index] ^ ((this_var_to_check[j] >= 0) ? 0U : 1U);
    this_check_to_var[j] = log_likelihood_ratio::copysign(this_check_to_var[j], 1 - 2 * static_cast<int>(final_sign));
  }
}

void ldpc_decoder_generic::compute_soft_bits(span<log_likelihood_ratio>       this_soft_bits,
                                             span<const log_likelihood_ratio> this_var_to_check,
                                             span<const log_likelihood_ratio> this_check_to_var)
{
  srsran_srsvec_assert_size(this_soft_bits, this_var_to_check);
  srsran_srsvec_assert_size(this_soft_bits, this_check_to_var);

  for (int j = 0; j != lifting_size; ++j) {
    // Soft bits absolutely larger than LOCAL_MAX_RANGE are set to infinity (LOCAL_INF). As a result, they become
    // fixed bits, that is they won't change their value from now on.
    this_soft_bits[j] = log_likelihood_ratio::promotion_sum(this_check_to_var[j], this_var_to_check[j]);
  }
}

void ldpc_decoder_generic::get_hard_bits(bit_buffer& out)
{
  unsigned out_length = out.size();

  span<log_likelihood_ratio> llrs = span<log_likelihood_ratio>(soft_bits).first(out_length);
  srsran::hard_decision(out, llrs);
}
