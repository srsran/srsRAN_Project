#ifndef SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDCCH_ENCODER_H
#define SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDCCH_ENCODER_H

#include "srsgnb/adt/span.h"
#include "srsgnb/phy/upper/channel_processors/pdcch_constants.h"
#include <memory>

namespace srsgnb {

/// Describes the PDCCH encoder interface.
class pdcch_encoder
{
public:
  /// Describes the necessary parameters to encode PDCCH message.
  struct config_t {
    /// Polar encoder K parameter (payload size including CRC bits).
    unsigned K;
    /// Number of rate-matched bits.
    unsigned E;
    /// RNTI used for CRC bits scrambling according to TS 38.212 section 7.3.2
    unsigned rnti;
  };

  /// Default destructor.
  virtual ~pdcch_encoder() = default;

  /// \brief Encodes a PDCCH message.
  /// \param [in] pdcch_msg
  /// \param [out] encoded
  virtual void encode(span<uint8_t> encoded, span<const uint8_t> data, const config_t& config) = 0;
};

/// Creates a PDCCH encoder instance
std::unique_ptr<pdcch_encoder> create_pdcch_encoder();

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDCCH_ENCODER_H
