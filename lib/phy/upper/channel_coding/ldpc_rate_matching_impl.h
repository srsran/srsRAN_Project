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

/// LDPC rate matching implementation.
class ldpc_rate_matching_impl : public ldpc_rate_matching
{
public:
  // See interface for the documentation.
  void rate_match(span<uint8_t> output, span<const uint8_t> input, const config_t& cfg) override;

private:
  /// Initializes the rate matcher internal state.
  void init(const config_t& cfg);

  /// \brief Carries out bit selection, as per TS38.212 Section 5.4.2.1.
  ///
  /// \param[out] out Sequence of selected bits.
  /// \param[in]  in  Input codeblock.
  void select_bits(span<uint8_t> out, span<const uint8_t> in) const;

  /// \brief Carries out bit interleaving, as per TS38.212 Section 5.4.2.2.
  ///
  /// \param[out] out Sequence of interleaved bits.
  /// \param[in]  in  Sequence of selected bits (see ldpc_rate_matching_impl::select_bits).
  void interleave_bits(span<uint8_t> out, span<const uint8_t> in) const;

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
