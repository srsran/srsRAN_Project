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
/// \brief LDPC decoder declaration.

#pragma once

#include "ldpc_graph_impl.h"
#include "srsgnb/phy/upper/channel_coding/ldpc/ldpc_decoder.h"
#include "srsgnb/support/error_handling.h"

namespace srsgnb {
namespace ldpc {
/// Maximum number of information bits in a codeblock (before shortening).
static constexpr unsigned MAX_BG_K = 22;
} // namespace ldpc

/// Template LDPC decoder
class ldpc_decoder_impl : public ldpc_decoder
{
public:
  /// Default constructor.
  ldpc_decoder_impl() = default;

  // See interface for the documentation.
  optional<unsigned> decode(bit_buffer&                      output,
                            span<const log_likelihood_ratio> input,
                            crc_calculator*                  crc,
                            const configuration&             cfg) override;

private:
  /// Initializes the decoder inner variables.
  void init(const configuration& cfg);

  /// Selects the appropriate decoding strategy and initializes concrete implementation buffers and variables.
  virtual void select_strategy() = 0;

  /// Loads the input log-likelihood ratios into the soft-bit buffers.
  virtual void load_soft_bits(span<const log_likelihood_ratio> llrs) = 0;

  /// \brief Updates the messages going from variable nodes to check nodes.
  /// \param[in] check_node The check node (in the base graph) the messages are directed to.
  virtual void update_variable_to_check_messages(unsigned check_node) = 0;

  /// \brief Updates the messages going from check nodes to variable nodes.
  /// \param[in] check_node The check node (in the base graph) the messages are coming from.
  virtual void update_check_to_variable_messages(unsigned check_node) = 0;

  /// \brief Updates the soft bits corresponding to the variable nodes connected to the same base graph check node.
  /// \param[in] check_node The check node (in the base graph) the variables nodes are connected to.
  virtual void update_soft_bits(unsigned check_node) = 0;

  /// Converts soft bits into hard bits and returns the decoded message.
  virtual void get_hard_bits(bit_buffer& out) = 0;

protected:
  /// Pointer to the Tanner graph (~ parity check matrix) used by the encoding algorithm.
  const ldpc_graph_impl* current_graph = nullptr;
  /// Lifting size as a natural number (as opposed to an element from srsgnb::ldpc::lifting_size_t).
  uint16_t lifting_size = 2;
  /// Total number of base graph variable nodes in the current graph.
  uint16_t bg_N_full = 68;
  /// Number of base graph variable nodes after shortening.
  uint16_t bg_N_short = 66;
  /// Number of base graph variable nodes in the high-rate region.
  uint16_t bg_N_high_rate = 26;
  /// Number of base graph check nodes.
  uint16_t bg_M = 46;
  /// Number of base graph variable nodes corresponding to information bits.
  uint16_t bg_K = 22;
  /// \brief Number of used variable nodes.
  ///
  /// Instead of using all the variable nodes and setting to zero all the punctured LLRs, the decoder will work only
  /// with the enough variable nodes to accommodate all LLRs provided by the input vector. It is a multiple of the
  /// lifting size.
  uint16_t codeblock_length = 52;
  /// Number of significant (not filler) bits.
  uint16_t nof_significant_bits = 44;

  /// Maximum number of iterations
  unsigned max_iterations = 6;

  /// Scaling factor of the normalized min-sum algorithm.
  float scaling_factor = 0.8;
};

/// Generic LDPC decoder implementation without any optimization.
class ldpc_decoder_generic : public ldpc_decoder_impl
{
  // See above for the documentation.
  void select_strategy() override { nof_hrr_nodes = bg_N_high_rate * lifting_size; }
  void load_soft_bits(span<const log_likelihood_ratio> llrs) override;
  void update_variable_to_check_messages(unsigned check_node) override;
  void update_check_to_variable_messages(unsigned check_node) override;
  void update_soft_bits(unsigned check_node) override;
  void get_hard_bits(bit_buffer& out) override;

  /// \brief Helper function for update_variable_to_check_messages().
  ///
  /// Computes the exact value of the variable-to-check messages for a specific subset of contiguous variable nodes.
  /// \param[out] v2c  Resulting variable-to-check messages.
  /// \param[in]  soft Soft bits at the given nodes.
  /// \param[in]  c2v  Check-to-variable messages at the given nodes.
  /// \note The three spans refer to the same set of nodes and, in turn, have the same dimension.
  static void compute_var_to_check_msgs(span<log_likelihood_ratio>       v2c,
                                        span<const log_likelihood_ratio> soft,
                                        span<const log_likelihood_ratio> c2v);

  /// Number of nodes in the (lifted) high-rate region.
  unsigned nof_hrr_nodes = 8;

  /// Buffer to store the current value of the soft bits.
  std::array<log_likelihood_ratio, static_cast<size_t>(ldpc::MAX_BG_N_FULL* ldpc::MAX_LIFTING_SIZE)> soft_bits = {};

  /// \brief Buffer to store the current value of the check-to-variable messages.
  ///
  /// In the base graph, each check node is connected, at most, to all variable nodes in the high-rate region
  /// (of max length max_BG_K + 4) and an extra variable node in the extension region. Then, the graph is lifted.
  std::array<std::array<log_likelihood_ratio, static_cast<size_t>((ldpc::MAX_BG_K + 5) * ldpc::MAX_LIFTING_SIZE)>,
             ldpc::MAX_BG_M>
      check_to_var = {};

  /// \brief Buffer to store the current value of the variable-to-check messages.
  ///
  /// Implementing a layered-based algorithm, we only need to store the variable-to-check messages corresponding
  /// to the current (base graph) check node.
  std::array<log_likelihood_ratio, static_cast<size_t>((ldpc::MAX_BG_K + 5) * ldpc::MAX_LIFTING_SIZE)> var_to_check =
      {};

  /// \name Helper buffers
  /// The following buffers refer to a base graph check node (that is, a block of
  /// lifting_size nodes in the lifted graph).

  ///@{
  /// \brief Buffer to store the minimum (in absolute value) variable-to-check message.
  std::array<log_likelihood_ratio, ldpc::MAX_LIFTING_SIZE> min_var_to_check = {};
  /// \brief Buffer to store the second minimum (in absolute value) variable-to-check message for each base graph
  /// check node.
  std::array<log_likelihood_ratio, ldpc::MAX_LIFTING_SIZE> second_min_var_to_check = {};
  /// \brief Buffer to store the index of the minimum-valued variable-to-check message.
  std::array<unsigned, ldpc::MAX_LIFTING_SIZE> min_var_to_check_index = {};
  /// \brief Buffer to store the sign product of all variable-to-check messages.
  std::array<int8_t, ldpc::MAX_LIFTING_SIZE> sign_prod_var_to_check = {};
  ///@}
};

} // namespace srsgnb
