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
/// \brief Polar decoder declaration.

#pragma once

#include "srsran/phy/upper/channel_coding/polar/polar_decoder.h"
#include "srsran/phy/upper/channel_coding/polar/polar_encoder.h"
#include "srsran/srsvec/aligned_vec.h"

namespace srsran {

/// \brief Polar decoder implementation.
///
/// Generic polar decoder according to the specifications of TS38.212 Section 5.3.1.
class polar_decoder_impl : public polar_decoder
{
private:
  /// Types of node in an SSC decoder.
  enum node_rate : uint8_t {
    /// See function rate_0_node().
    RATE_0 = 0,
    /// See function rate_r_node().
    RATE_R = 2,
    /// See function rate_1_node().
    RATE_1 = 3,
  };

  /// Collection of SSC polar decoder parameters.
  struct params_s {
    /// \f$log_2\f$ of the code size.
    uint8_t code_size_log;
    /// Number of bits of the encoder input/output vector, for all stages.
    srsvec::aligned_vec<uint16_t> code_stage_size;
    /// Stores the type of all nodes in stage 0.
    std::vector<node_rate> node_type_alloc;
    /// Stores the type of all nodes, stage by stage (e.g., <tt>node_type[0]</tt> is the same as \c node_type_alloc).
    std::vector<node_rate*> node_type;
  };

  /// Describes the SSC polar decoder inner state.
  struct state_s {
    /// Current stage of the decoding algorithm (0,...,code_size_log).
    uint8_t stage;
    /// True if the last bit is decoded. False otherwise.
    bool flag_finished;
    /// Indicates the active node at each stage of the algorithm at a given moment.
    srsvec::aligned_vec<uint16_t> active_node_per_stage;
  };

  /// Structure with pointers needed to obtain the node_type.
  struct tmp_node_s {
    /// \brief Denotes whether a node is of type [RATE_0](#polar_decoder_impl::node_rate) (value 0) or of another
    /// type (value 1).
    srsvec::aligned_vec<uint8_t> is_not_rate_0;
    /// \brief Denotes whether a node is of type [RATE_1](#polar_decoder_impl::node_rate) (value 1) or of another type
    /// (value 0).
    span<uint8_t> is_rate_1;
    /// List of even-valued node indices.
    srsvec::aligned_vec<uint16_t> i_even;
    /// List of odd-valued node indices.
    srsvec::aligned_vec<uint16_t> i_odd;

    /// \brief Allocates memory resources for the computation of the node_type.
    /// \param[in] nMax \f$log_2\f$ of the maximum number of bits in the codeword.
    explicit tmp_node_s(uint8_t nMax);

    /// \brief Computes node types for the decoding tree associated to the given frozen set.
    /// \param[out] node_type      Two-dimensional pointer containing the node type at each stage of the decoding tree.
    /// \param[in] frozen_set      Position of the frozen bits in the codeword.
    /// \param[in] code_size_log   \f$log_2\f$ of code size.
    void compute(std::vector<node_rate*>& node_type, span<const uint16_t> frozen_set, uint16_t code_size_log);
  };

  /// LLR values for stage 0 (i.e., received LLRs).
  srsvec::aligned_vec<log_likelihood_ratio> llr_alloc;
  /// Pointers to the upper half of the LLR values for all stages.
  std::vector<span<log_likelihood_ratio>> llr0;
  /// Pointers to the lower half of LLR values for all stages.
  std::vector<span<log_likelihood_ratio>> llr1;
  /// Temporary estimated bits.
  srsvec::aligned_vec<uint8_t> est_bit;
  /// Decoder inner parameters.
  params_s param;
  /// Decoder state.
  state_s state;
  /// Node type manager.
  tmp_node_s tmp_node_type;
  /// A polar encoder.
  std::unique_ptr<polar_encoder> enc;

  /// \brief Polar decoder initialization.
  ///
  /// Initializes internal parameters and registers.
  /// \param[in] data_decoded     Register for writing the decoded data.
  /// \param[in] input_llr        Input codeblock as a sequence of log-likelihood ratios.
  /// \param[in] code_size_log    Codeblock size (binary logarithm thereof).
  /// \param[in] frozen_set       Indices of the frozen bits.
  void init(span<uint8_t>                    data_decoded,
            span<const log_likelihood_ratio> input_llr,
            uint8_t                          code_size_log,
            span<const uint16_t>             frozen_set);

  /// \brief Updates a RATE_0 node.
  ///
  /// All decoded bits below a [RATE_0](#polar_decoder_impl::node_rate) node are 0.
  /// The function updates the \a p->state->active_node_per_stage
  /// pointer to point to the next active node. It is assumed that message bits are initialized to 0.
  void rate_0_node();

  /// \brief Updates a RATE_1 node.
  ///
  /// [RATE_1](#polar_decoder_impl::node_rate) nodes at stage \f$ s \f$ return the associated \f$2^s\f$ estimated bits
  /// by making a hard decision on them. RATE_1 nodes also update message bits vector.
  void rate_1_node(span<uint8_t> data_decoded);

  /// \brief Updates a RATE_R node.
  ///
  /// RATE_R nodes at stage \f$ s \f$ return the associated \f$2^s\f$ decoded bit by calling
  /// the child nodes to the right and left of the decoding tree and then polar encoding (xor) their output.
  void rate_r_node(span<uint8_t> data_decoded);

  /// \brief Node processing.
  ///
  /// Switches between the different [types of node](#polar_decoder_impl::node_rate) for the SSC algorithm.
  /// Nodes in the decoding tree at stage \f$ s\f$ get the \f$2^s\f$ LLRs from the parent node and
  /// return the associated \f$2^s\f$ estimated bits.
  void simplified_node(span<uint8_t> data_decoded);

public:
  /// \brief Polar decoder initialization.
  ///
  /// Initializes all the polar decoder variables according to the Simplified Successive
  /// Cancellation (SSC) decoder algorithm and the maximum given code size.
  ///
  /// \param[in] enc           A polar encoder.
  /// \param[in] code_size_log The \f$ log_2\f$ of the number of bits of the decoder input/output vector.
  /// \return A unique pointer to the initialized SSC polar decoder.
  polar_decoder_impl(std::unique_ptr<polar_encoder> enc_, uint8_t nMax);

  // See interface for the documentation.
  void decode(span<uint8_t> data_decoded, span<const log_likelihood_ratio> input_llr, const polar_code& code) override;
};

} // namespace srsran
