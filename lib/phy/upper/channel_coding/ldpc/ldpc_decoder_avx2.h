/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief LDPC decoder - declaration of the AVX2-optimized implementation.

#pragma once

#include "avx2_support.h"
#include "ldpc_decoder_impl.h"

namespace srsgnb {

/// LDPC decoder implementation based on AVX2 intrinsics.
class ldpc_decoder_avx2 : public ldpc_decoder_impl
{
private:
  /// Maximum number of AVX2 vectors needed to represent a BG node.
  static constexpr unsigned MAX_NODE_SIZE_AVX2 = divide_ceil(ldpc::MAX_LIFTING_SIZE, AVX2_SIZE_BYTE);

  /// Maximum number of AVX2 vectors needed to represent a codeblock.
  static constexpr unsigned MAX_BLK_SIZE_AVX2 = MAX_NODE_SIZE_AVX2 * ldpc::MAX_BG_N_FULL;

  /// Maximum number of AVX2 vectors needed to represent all messages entering a single check node.
  static constexpr unsigned MAX_CHECK_CONNECTION_SIZE_AVX2 = MAX_NODE_SIZE_AVX2 * (ldpc::MAX_BG_K + 5);

  // See ldpc_decoder_impl for the documentation.
  void select_strategy() override
  { /* do nothing */
  }

  void load_soft_bits(span<const log_likelihood_ratio> llrs) override;
  void update_variable_to_check_messages(unsigned check_node) override;
  void update_check_to_variable_messages(unsigned check_node) override;
  void update_soft_bits(unsigned check_node) override;
  void get_hard_bits(span<uint8_t> out) override;

  /// \brief Helper function for update_variable_to_check_messages().
  ///
  /// Computes the exact value of the variable-to-check messages for a specific subset of contiguous variable nodes.
  /// \param[out] v2c  Resulting variable-to-check messages.
  /// \param[in]  soft Soft bits at the given nodes.
  /// \param[in]  c2v  Check-to-variable messages at the given nodes.
  /// \note The three spans refer to the same set of nodes and, in turn, have the same dimension.
  void static compute_var_to_check_msgs_avx2(mm256::avx2_span       v2c,
                                             mm256::avx2_const_span soft,
                                             mm256::avx2_const_span c2v);

  /// Buffer to store the current value of the soft bits.
  mm256::avx2_array<MAX_BLK_SIZE_AVX2> soft_bits = {};

  /// \brief Buffer to store the current value of the check-to-variable messages.
  ///
  /// In the base graph, each check node is connected, at most, to all variable nodes in the high-rate region
  /// (of max length max_BG_K + 4) and an extra variable node in the extension region. Then, the graph is lifted.
  std::array<mm256::avx2_array<MAX_CHECK_CONNECTION_SIZE_AVX2>, ldpc::MAX_BG_M> check_to_var = {};

  /// \brief Buffer to store the current value of the variable-to-check messages.
  ///
  /// Implementing a layered-based algorithm, we only need to store the variable-to-check messages corresponding
  /// to the current (base graph) check node.
  mm256::avx2_array<MAX_CHECK_CONNECTION_SIZE_AVX2> var_to_check = {};

  /// Auxiliary buffer to store the rotated variable-to-check messages.
  mm256::avx2_array<MAX_CHECK_CONNECTION_SIZE_AVX2> rotated_var_to_check = {};

  /// \name Helper buffers
  /// The following buffers refer to a base graph check node (that is, a block of
  /// lifting_size nodes in the lifted graph).

  ///@{
  /// \brief Buffer to store the minimum (in absolute value) variable-to-check message.
  mm256::avx2_array<MAX_NODE_SIZE_AVX2> min_var_to_check = {};
  /// \brief Buffer to store the second minimum (in absolute value) variable-to-check message for each base graph
  /// check node.
  mm256::avx2_array<MAX_NODE_SIZE_AVX2> second_min_var_to_check = {};
  /// \brief Buffer to store the index of the minimum-valued variable-to-check message.
  mm256::avx2_array<MAX_NODE_SIZE_AVX2> min_var_to_check_index = {};
  /// \brief Buffer to store the sign product of all variable-to-check messages.
  mm256::avx2_array<MAX_NODE_SIZE_AVX2> sign_prod_var_to_check = {};
  ///@}

  /// Number of AVX2 vectors needed to cover a lifted node.
  unsigned node_size_avx2 = 0;
};

} // namespace srsgnb
