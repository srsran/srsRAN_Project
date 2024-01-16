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
/// \brief LDPC decoder declaration - algorithm template.

#pragma once

#include "ldpc_graph_impl.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc_decoder.h"

namespace srsran {
namespace ldpc {
/// Maximum number of information bits in a codeblock (before shortening).
static constexpr unsigned MAX_BG_K = 22;
} // namespace ldpc

/// Template LDPC decoder
class ldpc_decoder_impl : public ldpc_decoder
{
public:
  /// \brief Maximum degree of a check node.
  ///
  /// In the base graph, each check node is connected, at most, to all variable nodes in the high-rate region
  /// (of maximum length max_BG_K + 4) and an extra variable node in the extension region.
  static constexpr unsigned MAX_CHECK_NODE_DEGREE = ldpc::MAX_BG_K + 5;

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

  /// Initializes implementation-specific inner variables.
  virtual void specific_init() = 0;

  /// Loads the input log-likelihood ratios into the soft-bit buffers.
  void load_soft_bits(span<const log_likelihood_ratio> llrs);

  /// \brief Updates the messages going from variable nodes to check nodes.
  /// \param[in] check_node The check node (in the base graph) the messages are directed to.
  void update_variable_to_check_messages(unsigned check_node);

  /// \brief Updates the messages going from check nodes to variable nodes.
  /// \param[in] check_node The check node (in the base graph) the messages are coming from.
  void update_check_to_variable_messages(unsigned check_node);

  /// Helper function for \ref update_check_to_variable_messages -  Returns a view to the portion of a buffer used to
  /// store a cyclically shifted version of the variable-to-check messages from \c var_node to the current check node.
  virtual span<log_likelihood_ratio> get_rotated_node(unsigned var_node) = 0;

  /// \brief Helper function for \ref update_check_to_variable_messages - Analyzes the variable-to-check messages for a
  /// check node.
  ///
  /// Finds the two absolute minimums of all the arriving variable-to-check messages and the product of their signs.
  /// The function operates on all the messages corresponding to the variable-to-check edges obtained from lifting a
  /// single variable-to-check edge in the base graph. Therefore, all buffers below have a size equal to the lifting
  /// size.
  /// \param[out] min_var_to_check        Buffer to store the value of the absolute minimum variable-to-check message.
  /// \param[out] second_min_var_to_check Buffer to store the value of the second absolute minimum variable-to-check
  ///                                     message.
  /// \param[out] min_var_to_check_index  Buffer to store the index of the absolute minimum variable-to-check message.
  /// \param[out] sign_prod_var_to_check  Buffer to store the sign of the product of all the messages reaching the check
  ///                                     node.
  /// \param[out] rotated_node            Buffer to store a cyclically-shifted version of the variable-to-check
  ///                                     messages.
  /// \param[in]  this_var_to_check       Buffer with the variable-to-check messages for the considered check node.
  /// \param[in]  shift                   Shift applied to the variable-to-check edges when lifting the base graph.
  /// \param[in]  var_node                Counting index of the variable node, that is it can only take values between
  ///                                     zero and the degree of the check node minus one.
  virtual void analyze_var_to_check_msgs(span<log_likelihood_ratio>       min_var_to_check,
                                         span<log_likelihood_ratio>       second_min_var_to_check,
                                         span<uint8_t>                    min_var_to_check_index,
                                         span<uint8_t>                    sign_prod_var_to_check,
                                         span<log_likelihood_ratio>       rotated_node,
                                         span<const log_likelihood_ratio> this_var_to_check,
                                         unsigned                         shift,
                                         unsigned                         var_node) = 0;

  /// \brief Helper function for \ref update_check_to_variable_messages - Computes the new check-to-variable messages.
  ///
  /// The function operates on all the messages corresponding to the variable-to-check edges obtained from lifting a
  /// single variable-to-check edge in the base graph. Therefore, all buffers below have a size equal to the lifting
  /// size.
  /// \param[out] this_check_to_var       Buffer to store the check-to-variable messages for the considered check node.
  /// \param[in]  this_var_to_check       Buffer with the variable-to-check messages for the considered check node.
  /// \param[in]  rotated_node            Buffer containing a cyclically-shifted version of the variable-to-check
  ///                                     messages.
  /// \param[in]  min_var_to_check        Buffer containing the value of the absolute minimum variable-to-check message.
  /// \param[in]  second_min_var_to_check Buffer containing the value of the second absolute minimum variable-to-check
  ///                                     message.
  /// \param[in]  min_var_to_check_index  Buffer containing the index of the absolute minimum variable-to-check message.
  /// \param[in]  sign_prod_var_to_check  Buffer containing the sign of the product of all the messages reaching the
  ///                                     check node.
  /// \param[in]  shift                   Shift applied to the variable-to-check edges when lifting the base graph.
  /// \param[in]  var_node                Counting index of the variable node, that is it can only take values between
  ///                                     zero and the degree of the check node minus one.
  virtual void compute_check_to_var_msgs(span<log_likelihood_ratio>       this_check_to_var,
                                         span<const log_likelihood_ratio> this_var_to_check,
                                         span<const log_likelihood_ratio> rotated_node,
                                         span<const log_likelihood_ratio> min_var_to_check,
                                         span<const log_likelihood_ratio> second_min_var_to_check,
                                         span<const uint8_t>              min_var_to_check_index,
                                         span<const uint8_t>              sign_prod_var_to_check,
                                         unsigned                         shift,
                                         unsigned                         var_node) = 0;

  /// \brief Updates the soft bits corresponding to the variable nodes connected to the same base graph check node.
  /// \param[in] check_node The check node (in the base graph) the variables nodes are connected to.
  void update_soft_bits(unsigned check_node);

  /// \brief Helper function for \ref update_soft_bits().
  ///
  /// Updates the soft bits corresponding to one variable node of the base graph (that is, the number of updated soft
  /// bits is equal to the lifting size).
  /// \param[out] this_soft_bits     View to the updated soft bits.
  /// \param[in]  this_var_to_check  View to the input variable-to-check messages.
  /// \param[in]  this_check_to_var  View to the input check-to-variable messages.
  virtual void compute_soft_bits(span<log_likelihood_ratio>       this_soft_bits,
                                 span<const log_likelihood_ratio> this_var_to_check,
                                 span<const log_likelihood_ratio> this_check_to_var) = 0;

  /// Converts soft bits into hard bits and returns the decoded message.
  virtual void get_hard_bits(bit_buffer& out) = 0;

  /// \brief Helper function for \ref update_variable_to_check_messages().
  ///
  /// Computes the updated value of the messages between a lifted variable node and a lifted check node (therefore, the
  /// number of computed messages is equal to the lifting size).
  /// \param[out] this_var_to_check  Buffer to store the updated variable-to-check messages.
  /// \param[in]  this_soft_bits     Current soft bits.
  /// \param[in]  this_check_to_var  Current check-to-variable messages.
  /// \remard An exception is raised if the size of the three spans is not the same.
  virtual void compute_var_to_check_msgs(span<log_likelihood_ratio>       this_var_to_check,
                                         span<const log_likelihood_ratio> this_soft_bits,
                                         span<const log_likelihood_ratio> this_check_to_var) = 0;

protected:
  /// Number of base graph variable nodes corresponding to information bits.
  uint16_t bg_K = 22;
  /// Lifting size as a natural number (as opposed to an element from srsran::ldpc::lifting_size_t).
  uint16_t lifting_size = 2;
  /// Number of bytes used to store a lifted node.
  unsigned node_size_byte = 2;
  /// Scaling factor of the normalized min-sum algorithm.
  float scaling_factor = 0.8;

  /// Buffer to store the current value of the soft bits.
  std::array<log_likelihood_ratio, static_cast<size_t>(ldpc::MAX_BG_N_FULL* ldpc::MAX_LIFTING_SIZE)> soft_bits;

private:
  /// Pointer to the Tanner graph (~ parity check matrix) used by the encoding algorithm.
  const ldpc_graph_impl* current_graph = nullptr;
  /// Total number of base graph variable nodes in the current graph.
  uint16_t bg_N_full = 68;
  /// Number of base graph variable nodes after shortening.
  uint16_t bg_N_short = 66;
  /// Number of base graph variable nodes in the high-rate region.
  uint16_t bg_N_high_rate = 26;
  /// Number of base graph check nodes.
  uint16_t bg_M = 46;
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

  /// \brief Buffer to store the current value of the variable-to-check messages.
  ///
  /// Implementing a layered-based algorithm, we only need to store the variable-to-check messages corresponding
  /// to the current (base graph) check node.
  std::array<log_likelihood_ratio, static_cast<size_t>(MAX_CHECK_NODE_DEGREE* ldpc::MAX_LIFTING_SIZE)> var_to_check =
      {};

  /// Buffer to store the current value of the check-to-variable messages.
  std::array<std::array<log_likelihood_ratio, static_cast<size_t>(MAX_CHECK_NODE_DEGREE* ldpc::MAX_LIFTING_SIZE)>,
             ldpc::MAX_BG_M>
      check_to_var;

  /// Initialization flags of check-to-variable messages: true if initialized.
  std::array<bool, ldpc::MAX_BG_M> is_check_to_var_initialised;
};

} // namespace srsran
