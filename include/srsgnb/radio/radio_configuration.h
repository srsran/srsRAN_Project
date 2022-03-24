
#ifndef SRSGNB_RADIO_RADIO_CONFIGURATION_H
#define SRSGNB_RADIO_RADIO_CONFIGURATION_H

#include "srsgnb/adt/static_vector.h"
#include "srsgnb/radio/radio_constants.h"

namespace srsgnb {

namespace radio_configuration {

/// \brief Describes clock source configuration.
/// \note Not all driver and/or devices support all possible combinations.
struct clock_sources {
  /// Defines the possible clock sources.
  enum class source {
    /// Let the driver to select the default.
    DEFAULT = 0,
    /// Use internal generator.
    INTERNAL,
    /// Use external source.
    EXTERNAL,
    /// Use GPSDO.
    GPSDO
  };

  /// Synchronization source selection.
  source sync;

  /// Clock source selection.
  source clock;
};

/// \brief Describes frequency settings common for transmit and receive chains.
/// \note Not all driver and/or devices support a different \c lo_frequency_hz.
struct lo_frequency {
  /// Baseband center frequency in Hz.
  double center_frequency_hz;
  /// Local oscillator frequency in Hz. Set to zero to let the driver select it according to \c center_frequency_hz.
  double lo_frequency_hz;
};

/// Describes a port configuration common for transmit and receive chains.
struct port {
  /// Frequency parameters.
  lo_frequency freq;
  /// Initial gain in decibels.
  double gain_dB;
  /// \brief Indicates any device specific parameters to create the port.
  /// \remark Not all driver and/or devices support this feature.
  std::string args;
};

/// Describes a stream configuration common for transmit and receive chains.
struct stream {
  /// Provides the configuration for each port. The number of elements indicates the number of ports for the stream.
  static_vector<port, RADIO_MAX_NOF_PORTS> ports;
  /// \brief Indicates any device specific parameters to create the stream.
  /// \remark Not all driver and/or devices support this feature.
  std::string args;
};

/// Describes the necessary parameters to initialise a radio.
struct radio {
  /// Clock configuration.
  clock_sources clock;
  /// Provides the configuration for each transmit stream. The number of elements indicates the number of transmit
  /// streams.
  static_vector<stream, RADIO_MAX_NOF_STREAMS> tx_streams;
  /// Provides the configuration for each receive stream. The number of elements indicates the number of receive
  /// streams.
  static_vector<stream, RADIO_MAX_NOF_STREAMS> rx_streams;
  /// Sampling rate in Hz common for all transmit and receive chains.
  double sampling_rate_Hz;
  /// \brief Over the wire format. Indicates the data format baseband samples are transported between the baseband unit
  /// and the host.
  /// \note Not all driver and/or devices support this feature.
  enum {
    /// Default, the parameter is unset and let the driver use the default type.
    OTW_DEFAULT = 0,
    /// Signed 16 bit integer complex values.
    OTW_SC16
  } otw_format;
  /// \brief Indicates any device specific parameters to create the session.
  /// \remark Not all driver and/or devices support this feature.
  std::string args;
};

} // namespace radio_configuration
} // namespace srsgnb

#endif // SRSGNB_RADIO_RADIO_CONFIGURATION_H
