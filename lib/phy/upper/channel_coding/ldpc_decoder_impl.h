/// \file
/// LDPC decoder definition.

#ifndef SRSGNB_CHANNEL_CODING_LDPC_DECODER_IMPL
#define SRSGNB_CHANNEL_CODING_LDPC_DECODER_IMPL

#include "ldpc_graph_impl.h"
#include "srsgnb/phy/upper/channel_coding/ldpc_decoder.h"
#include <iostream>

namespace srsgnb {
namespace ldpc {
/// Maximum number of information bits in a codeblock (before shortening).
static constexpr unsigned max_BG_K = 22;
} // namespace ldpc

/// Template LDPC decoder
class ldpc_decoder_impl : public ldpc_decoder
{
public:
  ldpc_decoder_impl() = default;

  // no copy and move constructors
  ldpc_decoder_impl(ldpc_decoder_impl&)  = delete;
  ldpc_decoder_impl(ldpc_decoder_impl&&) = delete;

  // no copy and move operators
  ldpc_decoder_impl& operator=(ldpc_decoder_impl&) = delete;
  ldpc_decoder_impl& operator=(ldpc_decoder_impl&&) = delete;

  ~ldpc_decoder_impl() override = default;

  unsigned decode(span<const int8_t> input, span<uint8_t> output, const config_t& cfg) override;

private:
  void         init(const config_t& cfg);
  virtual void select_strategy()                                   = 0;
  virtual void load_soft_bits(span<const int8_t> llrs)             = 0;
  virtual void update_variable_to_check_messages(unsigned i_layer) = 0;
  virtual void update_check_to_variable_messages(unsigned i_layer) = 0;
  virtual void update_soft_bits(unsigned i_layer)                  = 0;
  virtual void get_hard_bits(span<uint8_t> out)                    = 0;

protected:
  /// Pointer to the Tanner graph (~ parity check matrix) used by the encoding algorithm.
  const ldpc_graph_impl* current_graph{};
  /// Lifting size as a natural number (as opposed to an element from srsgnb::ldpc::lifting_size_t).
  uint16_t lifting_size{};
  /// Total number of base graph variable nodes in the current graph.
  uint16_t bg_N_full{};
  /// Number of base graph variable nodes after shortening.
  uint16_t bg_N_short{};
  /// Number of base graph variable nodes in the high-rate region.
  uint16_t bg_N_high_rate{};
  /// Number of base graph check nodes.
  uint16_t bg_M{};
  /// Number of base graph variable nodes corresponding to information bits.
  uint16_t bg_K{};
  /// \brief Number of used variable nodes.
  ///
  /// Instead of using all the variable nodes and setting to zero all the punctured LLRs, the decoder will work only
  /// with the enough variable nodes to accommodate all LLRs provided by the input vector. It is a multiple of the
  /// lifting size.
  uint16_t codeblock_length{};

  /// Maximum number of iterations
  unsigned max_iterations{6};

  /// Scaling factor of the normalized min-sum algorithm.
  float scaling_factor{0.8};

  /// Pointer to a CRC calculator
  crc_calculator* crc = nullptr;
};

class ldpc_decoder_generic : public ldpc_decoder_impl
{
  void        select_strategy() override { nof_hrr_nodes = bg_N_high_rate * lifting_size; }
  void        load_soft_bits(span<const int8_t> llrs) override;
  void        update_variable_to_check_messages(unsigned i_layer) override;
  void        update_check_to_variable_messages(unsigned i_layer) override;
  void        update_soft_bits(unsigned i_layer) override;
  void        get_hard_bits(span<uint8_t> out) override;
  static void compute_var_to_check_msgs(span<const int8_t> soft, span<const int8_t> c2v, span<int8_t> v2c);

  /// Number of nodes in the (lifted) high-rate region.
  unsigned nof_hrr_nodes{};

  /// Register to store the current value of the soft bits.
  std::array<int8_t, static_cast<size_t>(ldpc::max_BG_N_full* ldpc::max_lifting_size)> soft_bits{};

  /// \brief Register to store the current value of the check-to-variable messages.
  ///
  /// In the base graph, each check node is connected, at most, to all variable nodes in the high-rate region
  /// (of max length max_BG_K + 4) and an extra variable node in the extension region. Then, the graph is lifted.
  std::array<std::array<int8_t, static_cast<size_t>((ldpc::max_BG_K + 5) * ldpc::max_lifting_size)>, ldpc::max_BG_M>
      check_to_var{};

  /// \brief Register to store the current value of the variable-to-check messages.
  ///
  /// Implementing a layered-based algorithm, we only need to store the variable-to-check messages corresponding
  /// to the current (base graph) check node.
  std::array<int8_t, static_cast<size_t>((ldpc::max_BG_K + 5) * ldpc::max_lifting_size)> var_to_check{};

  /// \name Helper registers
  /// The following registers refer to a base graph check node (that is, a block of
  /// lifting_size nodes in the lifted graph).
  ///@{

  /// \brief Register to store the minimum variable-to-check message.
  std::array<int8_t, ldpc::max_lifting_size> min_var_to_check{};
  /// \brief Register to store the second minimum variable-to-check message for each base graph check node.
  std::array<int8_t, ldpc::max_lifting_size> second_min_var_to_check{};
  /// \brief Index of the minimum-valued variable-to-check message.
  std::array<unsigned, ldpc::max_lifting_size> min_var_to_check_index{};
  /// \brief Sign product of all variable-to-check messages.
  std::array<int8_t, ldpc::max_lifting_size> sign_prod_var_to_check{};
  ///@}
};

class ldpc_decoder_avx2 : public ldpc_decoder_impl
{
  void        select_strategy() override { not_implemented(__func__); }
  void        load_soft_bits(span<const int8_t> /*nn*/) override { not_implemented(__func__); }
  void        update_variable_to_check_messages(unsigned /*nn*/) override { not_implemented(__func__); }
  void        update_check_to_variable_messages(unsigned /*nn*/) override { not_implemented(__func__); }
  void        update_soft_bits(unsigned /*nn*/) override { not_implemented(__func__); }
  void        get_hard_bits(span<uint8_t> /*nn*/) override { not_implemented(__func__); }
  static void not_implemented(const std::string& fn)
  {
    std::cout << "LDPC AVX2 Decoder -> " << fn << " - not implemented yet.\n";
  }
};
} // namespace srsgnb
#endif // SRSGNB_CHANNEL_CODING_LDPC_DECODER_IMPL