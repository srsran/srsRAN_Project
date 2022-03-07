/// \file
/// \brief LDPC decoder interface.

#ifndef SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_DECODER
#define SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_DECODER

#include "srsgnb/adt/span.h"
#include "srsgnb/phy/upper/channel_coding/crc_calculator.h"
#include "srsgnb/phy/upper/channel_coding/ldpc.h"

namespace srsgnb {

/// LDPC decoder interface.
class ldpc_decoder
{
public:
  /// Default destructor.
  virtual ~ldpc_decoder() = default;

  /// Decoder configuration.
  struct config_t {
    /// Code base graph.
    srsgnb::ldpc::base_graph_t base_graph{ldpc::base_graph_t::BG1};
    /// Code lifting size.
    srsgnb::ldpc::lifting_size_t lifting_size{ldpc::lifting_size_t::LS2};
    /// Maximum number of iterations.
    unsigned max_iterations{6};
    /// Scaling factor of the normalized min-sum decoding algorithm.
    float scaling_factor{0.8};
    /// CRC calculator
    crc_calculator* crc = nullptr;
  };

  /// \brief Decodes a codeblock.
  ///
  /// \param[out] output  Reconstructed message of information bits.
  /// \param[in]  input   Log-likelihood ratios of the codeblock to be decoded.
  /// \param[in]  cfg     Decoder configuration.
  virtual unsigned decode(span<uint8_t> output, span<const int8_t> input, const config_t& cfg) = 0;
};

std::unique_ptr<ldpc_decoder> create_ldpc_decoder(const std::string& dec_type);

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_DECODER
