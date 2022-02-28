#ifndef SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_ENCODER
#define SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_ENCODER

#include "srsgnb/adt/span.h"
#include "srsgnb/phy/upper/channel_coding/ldpc.h"

namespace srsgnb {

/// LDPC encoder interface.
class ldpc_encoder
{
public:
  /// Default destructor.
  virtual ~ldpc_encoder() = default;

  /// Encoder configuration.
  struct config_t {
    /// Code base graph.
    srsgnb::ldpc::base_graph_t base_graph{ldpc::base_graph_t::BG1};
    /// Code lifting size.
    srsgnb::ldpc::lifting_size_t lifting_size{ldpc::lifting_size_t::LS2};
  };

  /// \brief Encodes a message.
  ///
  /// \param[in]  input   Message: original information bits (can contain filler_bit).
  /// \param[out] output  Resulting codeblock.
  /// \param[in]  cfg     Encoder configuration.
  /// \note The length of the output codeblock is deduced from the size of parameter \c output.
  virtual void encode(span<const uint8_t> input, span<uint8_t> output, const config_t& cfg) = 0;
};

std::unique_ptr<ldpc_encoder> create_ldpc_encoder(const std::string& enc_type);

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_ENCODER
