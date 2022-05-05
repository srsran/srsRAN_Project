#ifndef SRSGNB_LIB_PHY_UPPER_CHANNEL_CODING_SHORT_SHORT_BLOCK_DETECTOR_IMPL_H
#define SRSGNB_LIB_PHY_UPPER_CHANNEL_CODING_SHORT_SHORT_BLOCK_DETECTOR_IMPL_H

#include "srsgnb/phy/upper/channel_coding/short/short_block_detector.h"

namespace srsgnb {

static constexpr unsigned MAX_IN_BITS       = 32;
static constexpr unsigned MAX_OUT_BITS      = 11;
static constexpr unsigned MAX_NOF_CODEWORDS = 1U << MAX_OUT_BITS;

class short_block_detector_impl : public short_block_detector
{
public:
  // See interface for the documentation.
  bool detect(span<uint8_t> output, span<const int8_t> input) override;

  // See interface for the documentation.
  void rate_dematch(span<int8_t> output, span<const int8_t> input) override;

private:
  /// \brief Brute force ML detection for messages of length 3-11 bits.
  /// \param[out] output The detected message.
  /// \param[in]  input  The received codeword (represented as a sequence of 32 LLRs).
  /// \returns The value of the GLRT detection metric.
  /// \remark The size of \c output must match the length of the expected message.
  static double detect_3_11(span<uint8_t> output, span<const int8_t> input);

  /// \brief Look-up table of all possible transmitted sequences of 32 bits.
  ///
  /// Row \f$r\f$ corresponds to the codeword obtained by encoding the binary expansion of \f$r\f$ as described in
  /// TS 38.211 Section 5.3.3.3.
  static const std::array<std::array<int8_t, MAX_IN_BITS>, MAX_NOF_CODEWORDS> DETECT_TABLE;
};

} // namespace srsgnb
#endif // SRSGNB_LIB_PHY_UPPER_CHANNEL_CODING_SHORT_SHORT_BLOCK_DETECTOR_IMPL_H
