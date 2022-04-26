/// \file
/// \brief Encoder interface for short blocks.

#ifndef SRSGNB_PHY_UPPER_CHANNEL_CODING_SHORT_SHORT_BLOCK_ENCODER_H
#define SRSGNB_PHY_UPPER_CHANNEL_CODING_SHORT_SHORT_BLOCK_ENCODER_H

#include "srsgnb/adt/span.h"

#include <memory>

namespace srsgnb {

// TODO(david,borja): These will probably need to be moved somewhere else since they must be accessible by the
// scrambler.
/// \brief UCI placeholder bit.
///
/// A placeholder for setting a bit to 1 in the scrambler. It is denoted as "\c x" in TS38.212 Section 5.3.3 and in
/// TS38.211 Section 6.3.1.1.
constexpr uint8_t PLACEHOLDER_ONE = 255;
/// \brief UCI placeholder bit.
///
/// A placeholder for repeating the previous bit in the scrambler. It is denoted as "\c y" in TS38.212 Section 5.3.3 and
/// in TS38.211 Section 6.3.1.1.
constexpr uint8_t PLACEHOLDER_REPEAT = 254;

/// Short-block encoder interface.
class short_block_encoder
{
public:
  /// Default destructor.
  virtual ~short_block_encoder() = default;

  /// \brief Encodes a small number (less than 12) bits.
  ///
  /// Encodes a short sequence of bits (length 1 to 11, both included), following the specifications of TS38.212
  /// Section 5.3.3.
  /// \param[out] output Sequence of encoded bits (i.e., \f$d_0, d_1, \dots, d_{N-1}\f$ in TS38.212).
  /// \param[in]  input  Sequence of bits to encode (i.e., \f$c_0, c_1, \dots, c_{K-1}\f$ in TS38.212).
  /// \remark Both \c output and \c input follow an unpacked representation, that is one bit per entry.
  /// \remark The size \f$N\f$ of \c output should be as follows:
  ///   - \f$N=Q_m\f$, where \f$Q_m\f$ is the modulation order, if \c input is a single bit;
  ///   - \f$N=3Q_m\f$ if \c input consists of 2 bits;
  ///   - \f$N=32\f$ otherwise (\c input size from 3 to 11 bits).
  virtual void encode(span<uint8_t> output, span<const uint8_t> input) = 0;

  /// \brief Rate matching for short blocks.
  ///
  /// Matches the rate of a short block according to TS38.212 Section 5.4.3.
  /// \param[out] output  Rate-matched codeblock.
  /// \param[in]  input   Original codeblock.
  /// \remark Both \c output and \c input follow an unpacked representation, that is one bit per entry.
  /// \remark The size of \c output should not be smaller of the size of \c input.
  virtual void rate_match(span<uint8_t> output, span<const uint8_t> input) = 0;
};

std::unique_ptr<short_block_encoder> create_short_block_encoder();
} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_CODING_SHORT_SHORT_BLOCK_ENCODER_H
