/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/radio/radio_base.h"
#include <cstdint>
#include <optional>

namespace srsran {

/// Radio event sources.
enum class radio_event_source {
  /// Default event value meaning it is not set.
  UNDEFINED = 0,
  /// The event is from a receiver chain.
  RECEIVE,
  /// The event is from a transmit chain.
  TRANSMIT,
  /// The event cause is not listed from the above chain.
  OTHER
};

/// Translate an event_source to a string.
inline const char* to_string(radio_event_source value)
{
  switch (value) {
    case radio_event_source::UNDEFINED:
      return "undefined";
    case radio_event_source::RECEIVE:
      return "rx";
    case radio_event_source::TRANSMIT:
      return "tx";
    case radio_event_source::OTHER:
      return "other";
  }
  return "";
}

/// Radio event types.
enum class radio_event_type {
  /// Default event value meaning it is not set.
  UNDEFINED = 0,
  /// Indicates an start of a burst.
  START_OF_BURST,
  /// Indicates the end of a burst.
  END_OF_BURST,
  /// The transmit data arrives late to the baseband unit.
  LATE,
  /// The transmit data is not generated fast enough.
  UNDERFLOW,
  /// The receive buffers are not read fast enough.
  OVERFLOW,
  /// The event cause is not listed from the above causes.
  OTHER
};

/// Translate an event_type to a string.
inline const char* to_string(radio_event_type value)
{
  switch (value) {
    case radio_event_type::UNDEFINED:
      return "undefined";
    case radio_event_type::START_OF_BURST:
      return "start-of-burst";
    case radio_event_type::END_OF_BURST:
      return "end-of-burst";
    case radio_event_type::LATE:
      return "late";
    case radio_event_type::UNDERFLOW:
      return "underflow";
    case radio_event_type::OVERFLOW:
      return "overflow";
    case radio_event_type::OTHER:
      return "other";
  }
  return "";
}

/// \brief Describes a baseband unit event notifier interface.
///
/// This class describes an interface used by the baseband unit to notify events related with its operation.
class radio_event_notifier : public radio_base
{
public:
  /// Describes a radio notification.
  struct event_description {
    /// Indicates the stream identifier that triggered the event if it is available.
    std::optional<unsigned> stream_id;
    /// Indicates the channel identifier that triggered the event if it is available.
    std::optional<unsigned> channel_id;
    /// Indicates the source of the event.
    radio_event_source source = radio_event_source::UNDEFINED;
    /// Indicates the event type.
    radio_event_type type = radio_event_type::UNDEFINED;
    /// Indicates the timestamp of this event if it is available.
    std::optional<uint64_t> timestamp;
  };

  /// Notifies a new event that affected the real-time operation of the radio.
  virtual void on_radio_rt_event(const event_description& description) = 0;
};

} // namespace srsran
