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
/// \brief LDPC encoder declaration.
#pragma once

#include "ldpc_graph_impl.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/phy/upper/channel_coding/ldpc/ldpc_encoder.h"
#include "srsgnb/support/error_handling.h"
#include <array>

namespace srsgnb {

/// Template LDPC encoder.
class ldpc_encoder_impl : public ldpc_encoder
{
public:
  /// \name Constructors, destructor, copy and move operators
  /// Either default or deleted.

  ///@{
  ldpc_encoder_impl() = default;
  // no copy and move constructors
  ldpc_encoder_impl(ldpc_encoder_impl&)  = delete;
  ldpc_encoder_impl(ldpc_encoder_impl&&) = delete;

  // no copy and move operators
  ldpc_encoder_impl& operator=(ldpc_encoder_impl&)  = delete;
  ldpc_encoder_impl& operator=(ldpc_encoder_impl&&) = delete;

  ~ldpc_encoder_impl() override = default;
  ///@}

  // See interface for the documentation.
  void
  encode(span<uint8_t> output, span<const uint8_t> input, const codeblock_metadata::tb_common_metadata& cfg) override;

private:
  /// Initializes the encoder inner variables.
  void init(const codeblock_metadata::tb_common_metadata& cfg);
  /// Selects the appropriate encoding strategy.
  virtual void select_strategy() {}
  /// Loads the input bits into the inner register.
  virtual void load_input(span<const uint8_t> in) = 0;
  /// Computes some intermediate variables required by the actual encoding.
  virtual void preprocess_systematic_bits() = 0;
  /// Computes the shortest possible codeword (systematic part plus high-rate region, that is the first
  /// 4 x lifting size redundancy bits).
  virtual void encode_high_rate() = 0;
  /// Computes the rest of the redundancy bits (extension region).
  virtual void encode_ext_region() = 0;
  /// Moves relevant encoded bits from the internal register to the output vector.
  virtual void write_codeblock(span<uint8_t> out) = 0;

protected:
  // member variables
  /// Pointer to the Tanner graph (~ parity check matrix) used by the encoding algorithm.
  const ldpc_graph_impl* current_graph = nullptr;
  /// Lifting size as a natural number (as opposed to an element from srsgnb::ldpc::lifting_size_t).
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

/// Generic LDPC encoder implementation without any optimization.
class ldpc_encoder_generic : public ldpc_encoder_impl
{
  void select_strategy() override;
  void load_input(span<const uint8_t> in) override { message = in; }
  void preprocess_systematic_bits() override;
  void encode_high_rate() override { (this->*high_rate)(); }
  void encode_ext_region() override;
  void write_codeblock(span<uint8_t> out) override;

  /// Pointer type shortcut.
  using high_rate_strategy = void (ldpc_encoder_generic::*)();
  /// Pointer to a high-rate strategy member.
  high_rate_strategy high_rate;

  /// Carries out the high-rate region encoding for BG1 and lifting size index 6.
  void high_rate_bg1_i6();
  /// Carries out the high-rate region encoding for BG1 and lifting size index in {0, 1, 2, 3, 4, 5, 7}.
  void high_rate_bg1_other();
  /// Carries out the high-rate region encoding for BG2 and lifting size index in {3, 7}.
  void high_rate_bg2_i3_7();
  /// Carries out the high-rate region encoding for BG2 and lifting size index in {0, 1, 2, 4, 5, 6}.
  void high_rate_bg2_other();

  /// Local copy of the message to encode.
  span<const uint8_t> message = {};
  // Set up registers for the largest LS.
  /// Register to store auxiliary computation results.
  std::array<std::array<uint8_t, ldpc::MAX_LIFTING_SIZE>, ldpc::MAX_BG_M> auxiliary = {};
  /// Register to store computed encoded bits.
  std::array<uint8_t, static_cast<size_t>(ldpc::MAX_BG_N_FULL* ldpc::MAX_LIFTING_SIZE)> codeblock = {};
};

/// LDPC encoder implementation based on AVX2 intrinsics.
class ldpc_encoder_avx2 : public ldpc_encoder_impl
{
  void select_strategy() override { not_implemented(); };
  void load_input(span<const uint8_t> /*in*/) override { not_implemented(); }
  void preprocess_systematic_bits() override { not_implemented(); };
  void encode_high_rate() override { not_implemented(); }
  void encode_ext_region() override { not_implemented(); }
  void write_codeblock(span<uint8_t> /*out*/) override { not_implemented(); }

  static void not_implemented() { srsgnb_terminate("AVX2 Encoder - not implemented yet"); }
};

} // namespace srsgnb
