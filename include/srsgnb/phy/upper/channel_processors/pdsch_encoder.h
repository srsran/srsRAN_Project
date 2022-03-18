#ifndef SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDSCH_ENCODER_H
#define SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDSCH_ENCODER_H

#include "srsgnb/adt/span.h"

namespace srsgnb {

/// Describes the PDSCH encoder interface.
class pdsch_encoder
{
public:
  /// Describes the PDSCH message transmission.
  struct config_t {
    // ...
  };

  /// Default destructor.
  virtual ~pdsch_encoder() = default;

  /// \brief Encodes a PDSCH message.
  /// \param [in] pdsch_msg
  /// \param [out] encoded
  virtual void encode(span<uint8_t> encoded, span<const uint8_t> data, const config_t& config) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDSCH_ENCODER_H
