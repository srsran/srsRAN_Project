
#pragma once

#include "srsran/gateways/baseband/baseband_gateway_base.h"
#include "srsran/gateways/baseband/baseband_gateway_buffer.h"
#include <cstdint>

namespace srsran {

/// Describes a baseband gateway timestamp. It is expressed in multiples of the sample time.
using baseband_gateway_timestamp = uint64_t;

/// Describes a baseband gateway interface for transmission.
class baseband_gateway_transmitter : public baseband_gateway_base
{
public:
  /// Describes transmitter metadata.
  struct metadata {
    /// Baseband transmitter timestamp. Indicates the time the data needs to be transmitted.
    baseband_gateway_timestamp ts;
  };

  /// \brief Transmits a given baseband buffer in a given time provided in the metadata.
  /// \param[in] stream_id Radio stream identifier.
  /// \param[in] metadata Provides additional parameters for transmission.
  /// \param[in] data Provides the baseband buffer to transmit.
  /// \note The data buffers must the same number of channels than the stream.
  /// \note The application terminates if the stream identifier is invalid.
  virtual void transmit(unsigned stream_id, const metadata& metadata, baseband_gateway_buffer& data) = 0;
};

/// Describes a baseband gateway interface for reception.
class baseband_gateway_receiver : public baseband_gateway_base
{
public:
  /// Describes receiver metadata.
  struct metadata {
    /// Indicates the baseband signal received timestamp.
    baseband_gateway_timestamp ts;
  };

  /// \brief Receives a given baseband buffer.
  /// \param[out,in] data Provides the baseband buffer destination and the number of samples to receive.
  /// \param[in] stream_id Radio stream identifier.
  /// \return Receive metadata.
  /// \note The data buffers must the same number of channels than the stream.
  /// \note The application terminates if the stream identifier is invalid.
  virtual metadata receive(baseband_gateway_buffer& data, unsigned stream_id) = 0;
};

/// Describes a baseband gateway interface.
class baseband_gateway : public baseband_gateway_base
{
public:
  /// \brief Get the transmitter gateway.
  virtual baseband_gateway_transmitter& get_transmitter() = 0;

  /// \brief Get the receiver gateway.
  virtual baseband_gateway_receiver& get_receiver() = 0;
};

} // namespace srsran
