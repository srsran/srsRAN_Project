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
/// \brief LDPC encoder - Declarations for NEON-optimized version.

#pragma once

#include "ldpc_encoder_impl.h"

namespace srsran {

/// \brief LDPC encoder implementation based on NEON intrinsics.
/// \warning Instantiating an object of this class entails a heap memory allocation.
class ldpc_encoder_neon : public ldpc_encoder_impl
{
private:
  void select_strategy() override;
  void load_input(span<const uint8_t> in) override;
  void preprocess_systematic_bits() override { (this->*systematic_bits)(); }
  void encode_high_rate() override { (this->*high_rate)(); }
  void encode_ext_region() override { (this->*ext_region)(); }
  void write_codeblock(span<uint8_t> out) override;

  /// Alias for pointer to private methods.
  using strategy_method = void (ldpc_encoder_neon::*)();

  /// Placeholder for strategy implementation of preprocess_systematic_bits.
  strategy_method systematic_bits;
  /// Placeholder for strategy implementation of encode_high_rate.
  strategy_method high_rate;
  /// Placeholder for strategy implementation of encode_ext_region.
  strategy_method ext_region;

  /// Helper method to set the high-rate encoding strategy.
  template <unsigned NODE_SIZE_NEON_PH>
  static strategy_method
  select_hr_strategy(ldpc_base_graph_type current_bg, uint8_t current_ls_index, unsigned node_size_neon);

  /// Helper method to set the strategy for the systematic bits.
  template <unsigned NODE_SIZE_NEON_PH>
  static strategy_method select_sys_bits_strategy(ldpc_base_graph_type current_bg, unsigned node_size_neon);
  /// Helper method to set the extended-region encoding strategy.
  template <unsigned NODE_SIZE_NEON_PH>
  static strategy_method select_ext_strategy(unsigned node_size_neon);

  /// \brief Long lifting size version of preprocess_systematic_bits - short lifting size.
  /// \tparam BG_K_PH            Placeholder for the number of information nodes (i.e., K) for the current base graph.
  /// \tparam BG_M_PH            Placeholder for the number of check nodes (i.e., M) for the current base graph.
  /// \tparam NODE_SIZE_NEON_PH  Placeholder for the number of NEON registers used to represent a code node.
  template <unsigned BG_K_PH, unsigned BG_M_PH, unsigned NODE_SIZE_NEON_PH>
  void systematic_bits_inner();
  /// \brief Carries out the high-rate region encoding for BG1 and lifting size index 6 - long lifting size.
  /// \tparam NODE_SIZE_NEON_PH  Placeholder for the number of NEON registers used to represent a code node.
  template <unsigned NODE_SIZE_NEON_PH>
  void high_rate_bg1_i6_inner();
  /// \brief Carries out the high-rate region encoding for BG1 and lifting size index in {0, 1, 2, 3, 4, 5, 7} - long
  /// lifting size.
  /// \tparam NODE_SIZE_NEON_PH  Placeholder for the number of NEON registers used to represent a code node.
  template <unsigned NODE_SIZE_NEON_PH>
  void high_rate_bg1_other_inner();
  /// \brief Carries out the high-rate region encoding for BG2 and lifting size index in {3, 7} - long lifting size.
  /// \tparam NODE_SIZE_NEON_PH  Placeholder for the number of NEON registers used to represent a code node.
  template <unsigned NODE_SIZE_NEON_PH>
  void high_rate_bg2_i3_7_inner();
  /// \brief Carries out the high-rate region encoding for BG2 and lifting size index in {0, 1, 2, 4, 5, 6} - long
  /// lifting size.
  /// \tparam NODE_SIZE_NEON_PH  Placeholder for the number of NEON registers used to represent a code node.
  template <unsigned NODE_SIZE_NEON_PH>
  void high_rate_bg2_other_inner();
  /// \brief Carries out the extended region encoding when the lifting size is long.
  /// \tparam NODE_SIZE_NEON_PH  Placeholder for the number of NEON registers used to represent a code node.
  template <unsigned NODE_SIZE_NEON_PH>
  void ext_region_inner();

  /// Buffer containing the codeblock.
  std::array<uint8_t, ldpc::MAX_BG_N_FULL* ldpc::MAX_LIFTING_SIZE> codeblock_buffer = {};
  /// Length of the portion of the codeblock buffer actually used for the current configuration (as a number of NEON
  /// registers).
  size_t codeblock_used_size = 0;

  /// Auxiliary buffer for storing chunks of codeblocks (corresponds to the four redundancy nodes of the high-rate
  /// region).
  std::array<uint8_t, bg_hr_parity_nodes* ldpc::MAX_LIFTING_SIZE> auxiliary_buffer = {};
  /// Length of the extended region actually used for the current configuration (as a number of NEON registers).
  size_t length_extended = 0;

  /// Auxiliary buffer for storing a single node during rotations.
  std::array<uint8_t, ldpc::MAX_LIFTING_SIZE> rotated_node_buffer = {};

  /// Node size as a number of NEON vectors.
  unsigned node_size_neon = 0;
};

} // namespace srsran
