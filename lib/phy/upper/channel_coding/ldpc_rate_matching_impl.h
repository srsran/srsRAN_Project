/// \file
/// \brief LDPC rate matcher definition.

#ifndef SRSGNB_CHANNEL_CODING_LDPC_RATE_MATCHER_IMPL
#define SRSGNB_CHANNEL_CODING_LDPC_RATE_MATCHER_IMPL

#include "ldpc_graph_impl.h"
#include "srsgnb/phy/upper/channel_coding/ldpc_rate_matching.h"
#include <iostream>

namespace srsgnb {
namespace ldpc {
/// Maximum codeblock length (recall that 2 * LS variable nodes are punctured from the full codeword).
constexpr unsigned max_codeblock_length = (max_BG_N_full - 2) * max_lifting_size;
} // namespace ldpc

class ldpc_rate_matching_impl : public ldpc_rate_matching
{
public:
  // See interface for the documentation.
  void rate_match(span<uint8_t> output, span<const uint8_t> input, const config_t& cfg) override;

private:
  static void not_implemented(const std::string& fn)
  {
    std::cout << "LDPC RM -> " << fn << " - not implemented yet.\n";
  }
  void init(const config_t& cfg);
  void select_bits(span<const uint8_t> in, span<uint8_t> out) const;
  void interleave_bits(span<const uint8_t> in, span<uint8_t> out) const { not_implemented(__func__); }

  // Data members

  /// Bit selection circular buffer.
  span<const uint8_t> buffer{};
  /// Auxiliary buffer.
  std::array<uint8_t, ldpc::max_codeblock_length> auxiliary_buffer{};
  /// Redundancy version, values in {0, 1, 2, 3}.
  unsigned rv{0};
  /// Modulation scheme.
  unsigned modulation_order{1};
  /// Buffer length.
  unsigned buffer_length{0};
  /// Shift \f$ k_0 \f$ as defined in TS38.212 Table 5.4.2.1-2
  unsigned shift_k0{0};
};

} // namespace srsgnb

#endif // SRSGNB_CHANNEL_CODING_LDPC_RATE_MATCHER_IMPL
