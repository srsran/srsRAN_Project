/// \file
/// LDPC decoder definition.

#ifndef SRSGNB_CHANNEL_CODING_LDPC_DECODER_IMPL
#define SRSGNB_CHANNEL_CODING_LDPC_DECODER_IMPL

#include "ldpc_graph_impl.h"
#include "srsgnb/phy/upper/channel_coding/ldpc_decoder.h"
#include <iostream>

namespace srsgnb {

/// Template LDPC decoder
class ldpc_decoder_impl : public ldpc_decoder
{
public:
  unsigned decode(span<const int8_t> input, span<uint8_t> output, const config_t& cfg) override;

private:
  void         init(const config_t& cfg);
  virtual void select_strategy()                                   = 0;
  virtual void update_variable_to_check_messages(unsigned i_layer) = 0;
  virtual void update_check_to_variable_messages(unsigned i_layer) = 0;
  virtual void update_soft_bits(unsigned i_layer)                  = 0;
  virtual void extract_message(span<uint8_t> out)                  = 0;

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
  /// \brief Number of encoded bits in the input vector.
  ///
  /// Instead of using all the variable nodes and setting to zero all the punctured LLRs, the decoder will work only
  /// with the enough variable nodes to accommodate all LLRs provided by the input vector. It is a multiple of the
  /// lifting size.
  uint16_t codeblock_length{};

  /// Maximum number of iterations
  unsigned max_iterations{};

  /// Pointer to a CRC calculator
  crc_calculator* crc = nullptr;
};

class ldpc_decoder_generic : public ldpc_decoder_impl
{
  void        select_strategy() override { not_implemented(__func__); }
  void        update_variable_to_check_messages(unsigned /*nn*/) override { not_implemented(__func__); }
  void        update_check_to_variable_messages(unsigned /*nn*/) override { not_implemented(__func__); }
  void        update_soft_bits(unsigned /*nn*/) override { not_implemented(__func__); }
  void        extract_message(span<uint8_t> /*nn*/) override { not_implemented(__func__); }
  static void not_implemented(std::string fn)
  {
    std::cout << "LDPC Generic Decoder -> " << fn << " - not implemented yet.\n";
  }
};

class ldpc_decoder_avx2 : public ldpc_decoder_impl
{
  void        select_strategy() override { not_implemented(__func__); }
  void        update_variable_to_check_messages(unsigned /*nn*/) override { not_implemented(__func__); }
  void        update_check_to_variable_messages(unsigned /*nn*/) override { not_implemented(__func__); }
  void        update_soft_bits(unsigned /*nn*/) override { not_implemented(__func__); }
  void        extract_message(span<uint8_t> /*nn*/) override { not_implemented(__func__); }
  static void not_implemented(std::string fn)
  {
    std::cout << "LDPC AVX2 Decoder -> " << fn << " - not implemented yet.\n";
  }
};
} // namespace srsgnb
#endif // SRSGNB_CHANNEL_CODING_LDPC_DECODER_IMPL