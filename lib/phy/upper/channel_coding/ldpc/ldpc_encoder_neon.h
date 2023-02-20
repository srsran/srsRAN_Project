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
/// \brief LDPC encoder - Declarations for NEON-optimized version.

#pragma once

#include "ldpc_encoder_impl.h"
#include "neon_support.h"

namespace srsgnb {

/// \brief LDPC encoder implementation based on NEON intrinsics.
/// \warning Instantiating an object of this class entails a heap memory allocation.
class ldpc_encoder_neon : public ldpc_encoder_impl
{
private:
  /// Maximum number of NEON vectors needed to represent a BG node.
  static constexpr unsigned MAX_NODE_SIZE_NEON = divide_ceil(ldpc::MAX_LIFTING_SIZE, NEON_SIZE_BYTE);

  /// Maximum number of NEON vectors needed to represent a codeblock.
  static constexpr unsigned MAX_BLK_SIZE_NEON = MAX_NODE_SIZE_NEON * ldpc::MAX_BG_N_FULL;

  /// Maximum number of NEON vectors needed for the auxiliary buffer.
  static constexpr unsigned MAX_AUX_SIZE_NEON = MAX_NODE_SIZE_NEON * ldpc::MAX_BG_M;

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
  neon::neon_array<MAX_BLK_SIZE_NEON> codeblock;

  /// Used portion of the codeblock buffer.
  size_t codeblock_used_size = 0;

  /// Auxiliary buffer for storing chunks of codeblocks.
  neon::neon_array<MAX_AUX_SIZE_NEON> auxiliary;
  /// Used portion of the auxiliary buffer.
  size_t auxiliary_used_size = 0;

  /// Auxiliary buffer for storing a single node during rotations.
  neon::neon_array<MAX_NODE_SIZE_NEON> rotated_node;

  /// Node size as a number of NEON vectors.
  unsigned node_size_neon = 0;
};

} // namespace srsgnb
