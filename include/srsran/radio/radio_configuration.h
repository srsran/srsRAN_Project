/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/static_vector.h"
#include "srsran/radio/radio_constants.h"

namespace srsran {

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

/// Describes a channel configuration common for transmit and receive chains.
struct channel {
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
  /// Provides the configuration for each channel. The number of elements indicates the number of channels for the
  /// stream.
  static_vector<channel, RADIO_MAX_NOF_CHANNELS> channels;
  /// \brief Indicates any device specific parameters to create the stream.
  /// \remark Not all driver and/or devices support this feature.
  std::string args;
};

/// \brief Over the wire format. Indicates the data format baseband samples are transported between the baseband unit
/// and the host.
/// \note Not all driver and/or devices support this feature.
enum class over_the_wire_format {
  /// Default, the parameter is unset and let the driver use the default type.
  DEFAULT = 0,
  /// Signed 16 bit integer complex values.
  SC16,
  /// Signed 12 bit integer complex values.
  SC12,
  /// Signed 8 bit integer complex values.
  SC8
};

/// Describes the necessary parameters to initialize a radio.
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
  double sampling_rate_hz;
  /// Indicates the baseband signal transport format between the device and the host.
  over_the_wire_format otw_format;
  /// \brief Indicates any device specific parameters to create the session.
  /// \remark Not all driver and/or devices support this feature.
  std::string args;
  /// Logging level. Leave empty for default.
  std::string log_level;
};

/// Converts a string into a clock source. No error or invalid type is returned if the string is not valid.
inline clock_sources::source to_clock_source(const std::string& str)
{
  if (str == "internal") {
    return clock_sources::source::INTERNAL;
  }
  if (str == "external") {
    return clock_sources::source::EXTERNAL;
  }
  if (str == "gpsdo") {
    return clock_sources::source::GPSDO;
  }
  return clock_sources::source::DEFAULT;
}

/// Converts a string into a OTW format. No error or invalid type is returned if the string is not valid.
inline over_the_wire_format to_otw_format(const std::string& str)
{
  if (str == "sc8") {
    return over_the_wire_format::SC8;
  }
  if (str == "sc12") {
    return over_the_wire_format::SC12;
  }
  if (str == "sc16") {
    return over_the_wire_format::SC16;
  }
  return over_the_wire_format::DEFAULT;
}

/// Interface for validating a given radio configuration.
class validator
{
public:
  /// Default destructor.
  virtual ~validator() = default;

  /// \brief Determines whether a radio configuration is valid.
  ///
  /// The call mi
  ///
  /// \param[in] config Configuration parameters.
  /// \return True if the given radio configuration is valid for the given device, otherwise false.
  virtual bool is_configuration_valid(const radio& config) const = 0;
};

} // namespace radio_configuration
} // namespace srsran
