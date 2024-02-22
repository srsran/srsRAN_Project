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
/// \brief LDPC decoder - declaration of the AVX512-optimized implementation.

#pragma once

#include "ldpc_decoder_impl.h"

namespace srsran {

/// LDPC decoder implementation based on AVX512 intrinsics.
class ldpc_decoder_avx512 : public ldpc_decoder_impl
{
private:
  // See ldpc_decoder_impl for the documentation.
  void specific_init() override;

  void compute_soft_bits(span<log_likelihood_ratio>       this_soft_bits,
                         span<const log_likelihood_ratio> this_var_to_check,
                         span<const log_likelihood_ratio> this_check_to_var) override;

  bool get_hard_bits(bit_buffer& out) override;

  void compute_var_to_check_msgs(span<log_likelihood_ratio>       this_var_to_check,
                                 span<const log_likelihood_ratio> this_soft_bits,
                                 span<const log_likelihood_ratio> this_check_to_var) override;

  void analyze_var_to_check_msgs(span<log_likelihood_ratio>       min_var_to_check,
                                 span<log_likelihood_ratio>       second_min_var_to_check,
                                 span<uint8_t>                    min_var_to_check_index,
                                 span<uint8_t>                    sign_prod_var_to_check,
                                 span<const log_likelihood_ratio> rotated_node,
                                 unsigned                         var_node) override;

  void compute_check_to_var_msgs(span<log_likelihood_ratio>       this_check_to_var,
                                 span<const log_likelihood_ratio> this_var_to_check,
                                 span<const log_likelihood_ratio> rotated_node,
                                 span<const log_likelihood_ratio> min_var_to_check,
                                 span<const log_likelihood_ratio> second_min_var_to_check,
                                 span<const uint8_t>              min_var_to_check_index,
                                 span<const uint8_t>              sign_prod_var_to_check,
                                 unsigned                         shift,
                                 unsigned                         var_node) override;

  /// Auxiliary buffer to store the rotated variable-to-check messages.
  std::array<log_likelihood_ratio, ldpc::MAX_LIFTING_SIZE * MAX_CHECK_NODE_DEGREE> rotated_var_to_check;

  /// Number of AVX512 vectors needed to cover a lifted node.
  unsigned node_size_avx512 = 0;
};

} // namespace srsran
