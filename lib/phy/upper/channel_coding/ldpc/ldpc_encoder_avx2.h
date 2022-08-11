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
/// \brief LDPC encoder - Declarations for AVX2-optimized version.

#pragma once

#include "avx2_support.h"
#include "ldpc_encoder_impl.h"
#include "srsgnb/srsvec/aligned_vec.h"

#include <immintrin.h>

namespace srsgnb {

/// \brief LDPC encoder implementation based on AVX2 intrinsics.
/// \warning Instantiating an object of this class entails a heap memory allocation.
class ldpc_encoder_avx2 : public ldpc_encoder_impl
{
private:
  /// Maximum number of AVX2 vectors needed to represent a BG node.
  static constexpr unsigned MAX_NODE_SIZE_AVX2 = divide_ceil(ldpc::MAX_LIFTING_SIZE, AVX2_SIZE_BYTE);

  /// Maximum number of AVX2 vectors needed to represent a codeblock.
  static constexpr unsigned MAX_BLK_SIZE_AVX2 = MAX_NODE_SIZE_AVX2 * ldpc::MAX_BG_N_FULL;

  /// Maximum number of AVX2 vectors needed to for the auxiliary buffer.
  static constexpr unsigned MAX_AUX_SIZE_AVX2 = MAX_NODE_SIZE_AVX2 * ldpc::MAX_BG_M;

  void select_strategy() override;
  void load_input(span<const uint8_t> in) override;
  void preprocess_systematic_bits() override { (this->*systematic_bits)(); }
  void encode_high_rate() override { (this->*high_rate)(); }
  void encode_ext_region() override { (this->*ext_region)(); }
  void write_codeblock(span<uint8_t> out) override;

  /// Alias for pointer to private methods.
  using strategy_method = void (ldpc_encoder_avx2::*)();

  /// Placeholder for strategy implementation of preprocess_systematic_bits.
  strategy_method systematic_bits = nullptr;
  /// Placeholder for strategy implementation of encode_high_rate.
  strategy_method high_rate = nullptr;
  /// Placeholder for strategy implementation of encode_ext_region.
  strategy_method ext_region = nullptr;

  /// Helper method to set the high-rate encoding strategy for short lifting sizes.
  static strategy_method select_hr_strategy_short(ldpc_base_graph_type current_bg, uint8_t current_ls_index);
  /// Helper method to set the high-rate encoding strategy for long lifting sizes.
  static strategy_method select_hr_strategy_long(ldpc_base_graph_type current_bg, uint8_t current_ls_index);

  /// Short lifting size version of preprocess_systematic_bits - short lifting size.
  void systematic_bits_short();
  /// Long lifting size version of preprocess_systematic_bits - short lifting size.
  void systematic_bits_long();
  /// Carries out the high-rate region encoding for BG1 and lifting size index 6 - short lifting size.
  void high_rate_bg1_i6_short();
  /// Carries out the high-rate region encoding for BG1 and lifting size index in {0, 1, 2, 3, 4, 5, 7} - short lifting
  /// size.
  void high_rate_bg1_other_short();
  /// Carries out the high-rate region encoding for BG2 and lifting size index in {3, 7} - short lifting size.
  void high_rate_bg2_i3_7_short();
  /// Carries out the high-rate region encoding for BG2 and lifting size index in {0, 1, 2, 4, 5, 6} - short lifting
  /// size.
  void high_rate_bg2_other_short();
  /// Carries out the high-rate region encoding for BG1 and lifting size index 6 - long lifting size.
  void high_rate_bg1_i6_long();
  /// Carries out the high-rate region encoding for BG1 and lifting size index in {0, 1, 2, 3, 4, 5, 7} - long lifting
  /// size.
  void high_rate_bg1_other_long();
  /// Carries out the high-rate region encoding for BG2 and lifting size index in {3, 7} - long lifting size.
  void high_rate_bg2_i3_7_long();
  /// Carries out the high-rate region encoding for BG2 and lifting size index in {0, 1, 2, 4, 5, 6} - long lifting
  /// size.
  void high_rate_bg2_other_long();
  /// Carries out the extended region encoding when the lifting size is short.
  void ext_region_short();
  /// Carries out the extended region encoding when the lifting size is long.
  void ext_region_long();

  /// Buffer containing the codeblock.
  avx2_array<MAX_BLK_SIZE_AVX2 + 1> codeblock{1};

  /// Used portion of the codeblock buffer.
  size_t codeblock_used_size = 0;

  /// Auxiliary buffer for storing chunks of codeblocks.
  avx2_array<MAX_AUX_SIZE_AVX2> auxiliary{0};
  /// Used portion of the auxiliary buffer.
  size_t auxiliary_used_size = 0;

  /// Auxiliary buffer for storing a single node during rotations.
  avx2_array<MAX_NODE_SIZE_AVX2 + 2> rotated_node{1};

  /// Node size as a number of AVX2 vectors.
  unsigned node_size_avx2 = 0;
};

} // namespace srsgnb
