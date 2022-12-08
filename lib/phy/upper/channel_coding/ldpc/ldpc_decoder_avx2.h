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

  /// Alias for a pointer to the private method that computes variable-to-check messages.
  using var_to_check_strategy = std::function<void(mm256::avx2_span, mm256::avx2_const_span, mm256::avx2_const_span)>;

  /// Alias for a pointer to the private method that updates the soft bits.
  using soft_bit_strategy = std::function<void(mm256::avx2_span, mm256::avx2_const_span, mm256::avx2_const_span)>;

  /// Alias for a pointer to the private method that analyzes the variable-to-check messages at the check nodes.
  using analyze_var_to_check_strategy = std::function<
      void(mm256::avx2_span, mm256::avx2_span, mm256::avx2_span, mm256::avx2_span, mm256::avx2_const_span, unsigned)>;

  /// Alias for a pointer to the private method that computes the check-to-variable messages.
  using check_to_var_strategy = std::function<void(mm256::avx2_span,
                                                   mm256::avx2_const_span,
                                                   mm256::avx2_const_span,
                                                   mm256::avx2_const_span,
                                                   mm256::avx2_const_span,
                                                   mm256::avx2_const_span,
                                                   float,
                                                   unsigned)>;

  // See ldpc_decoder_impl for the documentation.
  void select_strategy() override;
  void load_soft_bits(span<const log_likelihood_ratio> llrs) override;
  void update_variable_to_check_messages(unsigned check_node) override;
  void update_check_to_variable_messages(unsigned check_node) override;
  void update_soft_bits(unsigned check_node) override;
  void get_hard_bits(span<uint8_t> out) override;

  /// \brief Helper function for update_variable_to_check_messages().
  ///
  /// Computes the exact value of the variable-to-check messages for a specific subset of contiguous variable nodes.
  /// \tparam NOF_NODES      Number of variable nodes the method is applied to.
  /// \tparam NODE_SIZE_AVX2 Size of a lifted node as a number of AVX2 vectors.
  /// \param[out] v2c        Resulting variable-to-check messages.
  /// \param[in]  soft       Soft bits at the given nodes.
  /// \param[in]  c2v        Check-to-variable messages at the given nodes.
  /// \note The three spans refer to the same set of nodes and, in turn, have the same dimension.
  template <unsigned NOF_NODES, unsigned NODE_SIZE_AVX2>
  void static compute_var_to_check_msgs(mm256::avx2_span v2c, mm256::avx2_const_span soft, mm256::avx2_const_span c2v);

  /// Pointer to the specialization of \ref compute_var_to_check_msgs used for the high-rate region.
  var_to_check_strategy compute_var_to_check_msgs_hr;
  /// Pointer to the specialization of \ref compute_var_to_check_msgs used for the extended region.
  var_to_check_strategy compute_var_to_check_msgs_ext;
  /// Helper function for setting \ref compute_var_to_check_msgs_hr for base graph 1.
  template <unsigned NODE_SIZE_AVX2_PH>
  var_to_check_strategy select_var_to_check_strategy_hr_bg1();
  /// Helper function for setting \ref compute_var_to_check_msgs_hr for base graph 2.
  template <unsigned NODE_SIZE_AVX2_PH>
  var_to_check_strategy select_var_to_check_strategy_hr_bg2();
  /// Helper function for setting \ref compute_var_to_check_msgs_ext (for both base graphs).
  template <unsigned NODE_SIZE_AVX2_PH>
  var_to_check_strategy select_var_to_check_strategy_ext();
  /// Pointer to the function that computes the soft-bit updates.
  soft_bit_strategy compute_soft_bits;
  /// Helper function for setting \ref compute_soft_bits.
  template <unsigned NODE_SIZE_AVX2_PH>
  soft_bit_strategy select_soft_bits_strategy();
  /// Pointer to the function that analyzes the variable-to-check messages at the check nodes.
  analyze_var_to_check_strategy analyze_var_to_check;
  /// Helper function for setting \ref analyze_var_to_check.
  template <unsigned NODE_SIZE_AVX2_PH>
  analyze_var_to_check_strategy select_analyze_var_to_check_strategy();
  /// Pointer to the function that computes the check-to-variable messages.
  check_to_var_strategy compute_check_to_var_msgs;
  /// Helper function for setting \ref compute_check_to_var_msgs.
  template <unsigned NODE_SIZE_AVX2_PH>
  check_to_var_strategy select_check_to_var_strategy();

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
