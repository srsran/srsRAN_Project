/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
/// \brief LDPC encoder - Declarations of algorithm template.
#pragma once

#include "ldpc_graph_impl.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc_encoder.h"

namespace srsran {

/// Template LDPC encoder.
class ldpc_encoder_impl : public ldpc_encoder
{
public:
  // See interface for the documentation.
  void encode(bit_buffer& output, const bit_buffer& input, const codeblock_metadata::tb_common_metadata& cfg) override;

private:
  /// Initializes the encoder inner variables.
  void init(const codeblock_metadata::tb_common_metadata& cfg);
  /// Selects the appropriate encoding strategy.
  virtual void select_strategy() {}
  /// Loads the input bits into the inner register.
  virtual void load_input(const bit_buffer& in) = 0;
  /// Computes some intermediate variables required by the actual encoding.
  virtual void preprocess_systematic_bits() = 0;
  /// Computes the shortest possible codeword (systematic part plus high-rate region, that is the first
  /// 4 x lifting size redundancy bits).
  virtual void encode_high_rate() = 0;
  /// Computes the rest of the redundancy bits (extension region).
  virtual void encode_ext_region() = 0;
  /// Moves relevant encoded bits from the internal register to the output vector.
  virtual void write_codeblock(bit_buffer& out) = 0;

protected:
  /// Number of base graph parity nodes in the high-rate region.
  static constexpr unsigned bg_hr_parity_nodes = 4;
  /// Pointer to the Tanner graph (~ parity check matrix) used by the encoding algorithm.
  const ldpc_graph_impl* current_graph = nullptr;
  /// Lifting size as a natural number (as opposed to an element from srsran::ldpc::lifting_size_t).
  uint16_t lifting_size = 2;
  /// Total number of base graph variable nodes in the current graph.
  uint16_t bg_N_full = 68;
  /// Number of base graph variable nodes after shortening.
  uint16_t bg_N_short = 66;
  /// Number of base graph check nodes.
  uint16_t bg_M = 46;
  /// Number of base graph variable nodes corresponding to information bits.
  uint16_t bg_K = 22;
  /// \brief Number of encoded bits needed to fill the output vector.
  ///
  /// Instead of computing all the variable nodes, the encoder can work with just enough bits
  /// to fill the vector returned by the ldpc_encoder_impl::encode function. It is a multiple of the lifting size.
  uint16_t codeblock_length = 52;
};

} // namespace srsran
