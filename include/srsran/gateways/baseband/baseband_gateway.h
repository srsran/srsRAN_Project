
#pragma once

#include "srsran/gateways/baseband/baseband_gateway_base.h"

namespace srsran {

class baseband_gateway_transmitter;
class baseband_gateway_receiver;

/// Describes a baseband gateway interface.
class baseband_gateway : public baseband_gateway_base
{
public:
  /// \brief Get the transmitter gateway.
  /// \remark An assertion is triggered if the stream identifier is invalid.
  virtual baseband_gateway_transmitter& get_transmitter(unsigned stream_id) = 0;

  /// \brief Get the receiver gateway.
  /// \remark An assertion is triggered if the stream identifier is invalid.
  virtual baseband_gateway_receiver& get_receiver(unsigned stream_id) = 0;
};

} // namespace srsran
