/// \file
/// LDPC encoder definition.
#ifndef SRSGNB_CHANNEL_CODING_LDPC_ENCODER_IMPL
#define SRSGNB_CHANNEL_CODING_LDPC_ENCODER_IMPL

#include "ldpc_graph_impl.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/phy/upper/channel_coding/ldpc_encoder.h"
#include <array>
#include <iostream>

namespace srsgnb {

/// Template LDPC encoder.
class ldpc_encoder_impl : public ldpc_encoder
{
  /// All possible LDPC graphs.
  static const std::array<ldpc_graph_impl, srsgnb::ldpc::total_nof_graphs> graph_array;

public:
  /// \name Constructors, destructor, copy and move operators
  /// Either default or deleted.
  ///@{
  ldpc_encoder_impl() = default;
  // no copy and move constructors
  ldpc_encoder_impl(ldpc_encoder_impl&)  = delete;
  ldpc_encoder_impl(ldpc_encoder_impl&&) = delete;

  // no copy and move operators
  ldpc_encoder_impl& operator=(ldpc_encoder_impl&) = delete;
  ldpc_encoder_impl& operator=(ldpc_encoder_impl&&) = delete;

  ~ldpc_encoder_impl() override = default;
  ///@}

  // See interface for the documentation.
  void encode(span<const uint8_t> input, span<uint8_t> output, const config_t& cfg) override;

private:
  /// Initializes the encoder registers and inner variables.
  void init(config_t cfg);
  /// Selects the appropriate encoding strategy.
  virtual void select_strategy() {}
  /// Loads the input bits into the inner register.
  virtual void load_input(span<const uint8_t> in) = 0;
  /// Computes some intermediate variables required by the actual encoding.
  virtual void preprocess_systematic_bits() = 0;
  /// Computes the shortest possible codeword (systematic part plus high-rate region, that is the first
  /// 4 x lifting size redundancy bits).
  virtual void encode_high_rate() = 0;
  /// Computes the rest of the redundancy bits.
  virtual void encode_ext_region() = 0;
  /// Moves relevant encoded bits from the internal register to the output vector.
  virtual void write_codeblock(span<uint8_t> out) = 0;

protected:
  // member variables
  /// Pointer to the Tanner graph (~ parity check matrix) used by the encoding algorithm.
  const ldpc_graph_impl* current_graph{};
  /// Lifting size as a natural number (as opposed to an element from srsgnb::ldpc::lifting_size_t).
  uint16_t lifting_size{};
  /// Total number of variable nodes in the current graph.
  uint16_t bg_N_full{};
  /// Number of variable nodes after shortening.
  uint16_t bg_N_short{};
  /// Number of check nodes.
  uint16_t bg_M{};
  /// Number of variable nodes corresponding to information bits.
  uint16_t bg_K{};
  /// \brief Number of encoded bits needed to fill the output vector.
  ///
  /// Instead of computing all the variable nodes, the encoder can work with just enough bits
  /// to fill the vector returned by the ldpc_encoder_impl::encode function. It is a multiple of the lifting size.
  uint16_t codeblock_length{};
};

/// Basic LDPC encoder implementation without any optimization.
class ldpc_encoder_basic : public ldpc_encoder_impl
{
  void select_strategy() override;
  void load_input(span<const uint8_t> in) override { message = in; }
  void preprocess_systematic_bits() override;
  void encode_high_rate() override { (this->*high_rate)(); }
  void encode_ext_region() override;
  void write_codeblock(span<uint8_t> out) override;

  /// Pointer type shortcut.
  using high_rate_strategy = void (ldpc_encoder_basic::*)();
  /// Pointer to a high-rate strategy member.
  high_rate_strategy high_rate;

  /// Carries out the high-rate region encoding for BG1 and lifting size index 6.
  void high_rate_bg1_i6();
  /// Carries out the high-rate region encoding for BG1 and lifting size index in the set {0, 1, 2, 3, 4, 5, 7}.
  void high_rate_bg1_other();

  /// Local copy of the message to encode.
  span<const uint8_t> message{};
  // Set up registers for the largest LS.
  /// Register to store auxiliary computation results.
  std::array<std::array<uint8_t, ldpc::max_lifting_size>, ldpc::max_BG_M> auxiliary{};
  /// Register to store computed encoded bits.
  std::array<uint8_t, static_cast<size_t>(ldpc::max_BG_N_full* ldpc::max_lifting_size)> codeblock{};
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

  static void not_implemented() { std::cout << "AVX2 Encoder - not implemented yet.\n"; }
};
} // namespace srsgnb

#endif // SRSGNB_CHANNEL_CODING_LDPC_ENCODER_IMPL