/// \file
/// \brief Encoder interface for small block lengths.

#ifndef SRSGNB_PHY_UPPER_CHANNEL_CODING_SHORT_SHORT_BLOCK_ENCODER_H
#define SRSGNB_PHY_UPPER_CHANNEL_CODING_SHORT_SHORT_BLOCK_ENCODER_H

#include "srsgnb/adt/span.h"
#include "srsgnb/phy/modulation_scheme.h"

#include <memory>

namespace srsgnb {

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
};

std::unique_ptr<short_block_encoder> create_short_block_encoder();
} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_CODING_SHORT_SHORT_BLOCK_ENCODER_H
