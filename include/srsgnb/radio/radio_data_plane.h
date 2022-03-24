
#ifndef SRSGNB_RADIO_RADIO_DATA_PLANE_H
#define SRSGNB_RADIO_RADIO_DATA_PLANE_H

#include "srsgnb/adt/span.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/radio/radio_constants.h"
#include <complex>

namespace srsgnb {

/// Describes a radio timestamp. It is expressed in multiples of the sample time.
using radio_timestamp = uint64_t;

/// Defines the radio sample type.
using radio_sample_type = std::complex<int16_t>;

/// Describes a radio baseband unit data plane interface for transmission.
class radio_data_plane_transmitter
{
public:
  /// Describes transmitter metadata.
  struct metadata {
    /// Baseband transmitter timestamp. Indicates the time the data needs to be transmitted.
    radio_timestamp ts;
  };

  /// Default destructor.
  virtual ~radio_data_plane_transmitter() = default;

  /// \brief Transmits a given baseband buffer in a given time provided in the metadata.
  /// \param[in] stream_id Radio stream identifier.
  /// \param[in] metadata Provides additional parameters for transmission.
  /// \param[in] data Provides the baseband buffer to transmit.
  /// \note All the data buffers must have the same number of elements.
  /// \note The application terminates if the stream identifier is invalid.
  virtual void transmit(unsigned                                                                 stream_id,
                        const metadata&                                                          metadata,
                        const static_vector<span<const radio_sample_type>, RADIO_MAX_NOF_PORTS>& data) = 0;
};

/// Describes a radio baseband unit data plane interface for reception.
class radio_data_plane_receiver
{
public:
  /// Describes transmitter metadata.
  struct metadata {
    /// Baseband signal received timestamp.
    radio_timestamp ts;
  };

  /// Default destructor.
  virtual ~radio_data_plane_receiver() = default;

  /// \brief Receives a given baseband buffer.
  /// \param[out,in] data Provides the baseband buffer destination and the number of samples to receive.
  /// \param[in] stream_id Radio stream identifier.
  /// \return Receive metadata.
  /// \note All the data buffers must have the same number of elements.
  /// \note The application terminates if the stream identifier is invalid.
  virtual metadata receive(static_vector<span<radio_sample_type>, RADIO_MAX_NOF_PORTS>& data, unsigned stream_id) = 0;
};

/// Describes a radio baseband unit data plane interface.
class radio_data_plane
{
public:
  /// Default destructor.
  virtual ~radio_data_plane() = default;

  /// \brief Get the transmitter data plane.
  virtual radio_data_plane_transmitter& get_transmitter() = 0;

  /// \brief Get a receiver data plane.
  virtual radio_data_plane_receiver& get_receiver() = 0;
};

} // namespace srsgnb

#endif // SRSGNB_RADIO_RADIO_DATA_PLANE_H
