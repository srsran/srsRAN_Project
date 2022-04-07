
#ifndef SRSGNB_RADIO_RADIO_DATA_PLANE_H
#define SRSGNB_RADIO_RADIO_DATA_PLANE_H

#include "radio_baseband_buffer.h"

namespace srsgnb {

/// Describes a radio timestamp. It is expressed in multiples of the sample time.
using radio_timestamp = uint64_t;

/// Describes a radio baseband unit data plane interface for transmission.
class radio_data_plane_transmitter : public radio_base
{
public:
  /// Describes transmitter metadata.
  struct metadata {
    /// Baseband transmitter timestamp. Indicates the time the data needs to be transmitted.
    radio_timestamp ts;
  };

  /// \brief Transmits a given baseband buffer in a given time provided in the metadata.
  /// \param[in] stream_id Radio stream identifier.
  /// \param[in] metadata Provides additional parameters for transmission.
  /// \param[in] data Provides the baseband buffer to transmit.
  /// \note The data buffers must the same number of channels than the stream.
  /// \note The application terminates if the stream identifier is invalid.
  virtual void transmit(unsigned stream_id, const metadata& metadata, radio_baseband_buffer& data) = 0;
};

/// Describes a radio baseband unit data plane interface for reception.
class radio_data_plane_receiver : public radio_base
{
public:
  /// Describes transmitter metadata.
  struct metadata {
    /// Baseband signal received timestamp.
    radio_timestamp ts;
  };

  /// \brief Receives a given baseband buffer.
  /// \param[out,in] data Provides the baseband buffer destination and the number of samples to receive.
  /// \param[in] stream_id Radio stream identifier.
  /// \return Receive metadata.
  /// \note The data buffers must the same number of channels than the stream.
  /// \note The application terminates if the stream identifier is invalid.
  virtual metadata receive(radio_baseband_buffer& data, unsigned stream_id) = 0;
};

/// Describes a radio baseband unit data plane interface.
class radio_data_plane : public radio_base
{
public:
  /// \brief Get the transmitter data plane.
  virtual radio_data_plane_transmitter& get_transmitter() = 0;

  /// \brief Get a receiver data plane.
  virtual radio_data_plane_receiver& get_receiver() = 0;
};

} // namespace srsgnb

#endif // SRSGNB_RADIO_RADIO_DATA_PLANE_H
