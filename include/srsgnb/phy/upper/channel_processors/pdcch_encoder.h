#ifndef SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDCCH_ENCODER_H
#define SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDCCH_ENCODER_H

#include "srsgnb/adt/span.h"

namespace srsgnb {

/// Describes the PDCCH encoder interface.
class pdcch_encoder
{
public:
  /// Describes the PDCCH message transmission.
  struct config_t {
    // ...
  };

  /// Default destructor.
  virtual ~pdcch_encoder() = default;

  /// \brief Encodes a PDCCH message.
  /// \param [in] pdcch_msg
  /// \param [out] encoded
  virtual void encode(span<uint8_t> encoded, span<const uint8_t> data, const config_t& config) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDCCH_ENCODER_H
