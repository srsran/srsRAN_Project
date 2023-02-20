/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief LDPC decoder - declaration of the NEON-optimized implementation.

#pragma once

#include "ldpc_decoder_impl.h"
#include "neon_support.h"

namespace srsgnb {

/// LDPC decoder implementation based on NEON intrinsics.
class ldpc_decoder_neon : public ldpc_decoder_impl
{
private:
  /// Maximum number of NEON vectors needed to represent a BG node.
  static constexpr unsigned MAX_NODE_SIZE_NEON = divide_ceil(ldpc::MAX_LIFTING_SIZE, NEON_SIZE_BYTE);

  /// Maximum number of NEON vectors needed to represent a codeblock.
  static constexpr unsigned MAX_BLK_SIZE_NEON = MAX_NODE_SIZE_NEON * ldpc::MAX_BG_N_FULL;

  /// Maximum number of NEON vectors needed to represent all messages entering a single check node.
  static constexpr unsigned MAX_CHECK_CONNECTION_SIZE_NEON = MAX_NODE_SIZE_NEON * (ldpc::MAX_BG_K + 5);

  /// Alias for a pointer to the private method that computes variable-to-check messages.
  using var_to_check_strategy = std::function<void(neon::neon_span, const neon::neon_span&, const neon::neon_span&)>;

  /// Alias for a pointer to the private method that updates the soft bits.
  using soft_bit_strategy = std::function<void(neon::neon_span, const neon::neon_span&, const neon::neon_span&)>;

  /// Alias for a pointer to the private method that analyzes the variable-to-check messages at the check nodes.
  using analyze_var_to_check_strategy = std::function<
      void(neon::neon_span, neon::neon_span, neon::neon_span, neon::neon_span, const neon::neon_span&, unsigned)>;

  /// Alias for a pointer to the private method that computes the check-to-variable messages.
  using check_to_var_strategy = std::function<void(neon::neon_span,
                                                   const neon::neon_span&,
                                                   const neon::neon_span&,
                                                   const neon::neon_span&,
                                                   const neon::neon_span&,
                                                   const neon::neon_span&,
                                                   float,
                                                   unsigned)>;

  // See ldpc_decoder_impl for the documentation.
  void select_strategy() override;
  void load_soft_bits(span<const log_likelihood_ratio> llrs) override;
  void update_variable_to_check_messages(unsigned check_node) override;
  void update_check_to_variable_messages(unsigned check_node) override;
  void update_soft_bits(unsigned check_node) override;
  void get_hard_bits(bit_buffer& out) override;

  /// \brief Helper function for update_variable_to_check_messages().
  ///
  /// Computes the exact value of the variable-to-check messages for a specific subset of contiguous variable nodes.
  /// \tparam NOF_NODES      Number of variable nodes the method is applied to.
  /// \tparam NODE_SIZE_NEON Size of a lifted node as a number of NEON vectors.
  /// \param[out] v2c        Resulting variable-to-check messages.
  /// \param[in]  soft       Soft bits at the given nodes.
  /// \param[in]  c2v        Check-to-variable messages at the given nodes.
  /// \note The three spans refer to the same set of nodes and, in turn, have the same dimension.
  template <unsigned NOF_NODES, unsigned NODE_SIZE_NEON>
  void static compute_var_to_check_msgs(neon::neon_span v2c, const neon::neon_span& soft, const neon::neon_span& c2v);

  /// Pointer to the specialization of \ref compute_var_to_check_msgs used for the high-rate region.
  var_to_check_strategy compute_var_to_check_msgs_hr;
  /// Pointer to the specialization of \ref compute_var_to_check_msgs used for the extended region.
  var_to_check_strategy compute_var_to_check_msgs_ext;

  /// Helper function for setting \ref compute_var_to_check_msgs_hr for base graph 1.
  template <unsigned NODE_SIZE_NEON_PH>
  var_to_check_strategy select_var_to_check_strategy_hr_bg1();
  /// Helper function for setting \ref compute_var_to_check_msgs_hr for base graph 2.
  template <unsigned NODE_SIZE_NEON_PH>
  var_to_check_strategy select_var_to_check_strategy_hr_bg2();
  /// Helper function for setting \ref compute_var_to_check_msgs_ext (for both base graphs).
  template <unsigned NODE_SIZE_NEON_PH>
  var_to_check_strategy select_var_to_check_strategy_ext();

  /// Pointer to the function that computes the soft-bit updates.
  soft_bit_strategy compute_soft_bits;
  /// Helper function for setting \ref compute_soft_bits.
  template <unsigned NODE_SIZE_NEON_PH>
  soft_bit_strategy select_soft_bits_strategy();

  /// Pointer to the function that analyzes the variable-to-check messages at the check nodes.
  analyze_var_to_check_strategy analyze_var_to_check_msgs;
  /// Helper function for setting \ref analyze_var_to_check_msgs.
  template <unsigned NODE_SIZE_NEON_PH>
  analyze_var_to_check_strategy select_analyze_var_to_check_strategy();

  /// Pointer to the function that computes the check-to-variable messages.
  check_to_var_strategy compute_check_to_var_msgs;
  /// Helper function for setting \ref compute_check_to_var_msgs.
  template <unsigned NODE_SIZE_NEON_PH>
  check_to_var_strategy select_check_to_var_strategy();

  /// Buffer to store the current value of the soft bits.
  neon::neon_array<MAX_BLK_SIZE_NEON> soft_bits = {};

  /// \brief Buffer to store the current value of the check-to-variable messages.
  ///
  /// In the base graph, each check node is connected, at most, to all variable nodes in the high-rate region
  /// (of max length max_BG_K + 4) and an extra variable node in the extension region. Then, the graph is lifted.
  std::array<neon::neon_array<MAX_CHECK_CONNECTION_SIZE_NEON>, ldpc::MAX_BG_M> check_to_var = {};

  /// Set to true if \c check_to_var has not been initialised.
  std::array<bool, ldpc::MAX_BG_M> is_check_to_var_initialised;

  /// \brief Buffer to store the current value of the variable-to-check messages.
  ///
  /// Implementing a layered-based algorithm, we only need to store the variable-to-check messages corresponding
  /// to the current (base graph) check node.
  neon::neon_array<MAX_CHECK_CONNECTION_SIZE_NEON> var_to_check = {};

  /// Auxiliary buffer to store the rotated variable-to-check messages.
  neon::neon_array<MAX_CHECK_CONNECTION_SIZE_NEON> rotated_var_to_check = {};

  /// Number of NEON vectors needed to cover a lifted node.
  unsigned node_size_neon = 0;
};
} // namespace srsgnb