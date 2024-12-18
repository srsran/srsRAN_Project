/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief LDPC decoder declaration - generic implementation.

#pragma once

#include "ldpc_decoder_impl.h"

namespace srsran {

/// Generic LDPC decoder implementation without any optimization.
class ldpc_decoder_generic : public ldpc_decoder_impl
{
  // See above for the documentation.
  void specific_init() override { node_size_byte = lifting_size; }

  void compute_var_to_check_msgs(span<log_likelihood_ratio>       this_var_to_check,
                                 span<const log_likelihood_ratio> this_soft_bits,
                                 span<const log_likelihood_ratio> this_check_to_var) override;

  void compute_soft_bits(span<log_likelihood_ratio>       this_soft_bits,
                         span<const log_likelihood_ratio> this_var_to_check,
                         span<const log_likelihood_ratio> this_check_to_var) override;

  void analyze_var_to_check_msgs(span<log_likelihood_ratio>       min_var_to_check,
                                 span<log_likelihood_ratio>       second_min_var_to_check,
                                 span<uint8_t>                    min_var_to_check_index,
                                 span<uint8_t>                    sign_prod_var_to_check,
                                 span<const log_likelihood_ratio> rotated_node,
                                 unsigned                         var_node) override;

  void scale(span<log_likelihood_ratio> out, span<const log_likelihood_ratio> in) override;

  void compute_check_to_var_msgs(span<log_likelihood_ratio>       this_check_to_var,
                                 span<const log_likelihood_ratio> this_var_to_check,
                                 span<const log_likelihood_ratio> rotated_node,
                                 span<const log_likelihood_ratio> min_var_to_check,
                                 span<const log_likelihood_ratio> second_min_var_to_check,
                                 span<const uint8_t>              min_var_to_check_index,
                                 span<const uint8_t>              sign_prod_var_to_check,
                                 unsigned                         shift,
                                 unsigned                         var_node) override;
};

} // namespace srsran