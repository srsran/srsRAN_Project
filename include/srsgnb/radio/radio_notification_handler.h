/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/optional.h"
#include "srsgnb/radio/radio_base.h"

namespace srsgnb {

/// \brief Describes a baseband unit event notifier interface.
///
/// This class describes an interface used by the baseband unit to notify events related with its operation.
class radio_notification_handler : public radio_base
{
public:
  /// Defines an unknown identifier for streams and ports.
  static constexpr unsigned UNKNOWN_ID = UINT32_MAX;

  /// Indicates the event source.
  class event_source
  {
  public:
    enum sources {
      /// Default event value meaning it is not set.
      UNDEFINED = 0,
      /// The event is from a receiver chain.
      RECEIVE,
      /// The event is from a transmit chain.
      TRANSMIT,
      /// The event cause is not listed from the above chain.
      OTHER
    };

    /// Get string value.
    const char* to_string() const
    {
      switch (value) {
        case sources::UNDEFINED:
          return "undefined";
        case sources::RECEIVE:
          return "rx";
        case sources::TRANSMIT:
          return "tx";
        case sources::OTHER:
          return "other";
      }
      return "";
    }

    event_source() = default;
    event_source(const event_source& other) : value(other.value) {}
    event_source(sources value_) : value(value_) {}
    bool operator==(const event_source& other) const { return value == other.value; }
    bool operator==(const sources& other) const { return value == other; }
         operator sources() const { return value; }
         operator sources&() { return value; }

  private:
    sources value = sources::UNDEFINED;
  };

  /// Describes the different events that can occur at the radio.
  class event_type
  {
  public:
    enum types {
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

    /// Get string value.
    const char* to_string() const
    {
      switch (value) {
        case types::UNDEFINED:
          return "undefined";
        case START_OF_BURST:
          return "start-of-burst";
        case END_OF_BURST:
          return "end-of-burst";
        case types::LATE:
          return "late";
        case types::UNDERFLOW:
          return "underflow";
        case types::OVERFLOW:
          return "overflow";
        case types::OTHER:
          return "other";
      }
      return "";
    }

    event_type() = default;
    event_type(const event_type& other) : value(other.value) {}
    event_type(types value_) : value(value_) {}
    bool operator==(const event_type& other) const { return value == other.value; }
    bool operator==(const types& other) const { return value == other; }
         operator types() const { return value; }
         operator types&() { return value; }

  private:
    types value = types::UNDEFINED;
  };

  /// Describes a radio notification.
  struct event_description {
    /// Indicates the stream identifier that triggered the event if it is available, UNKNOWN_ID otherwise.
    unsigned stream_id = UNKNOWN_ID;
    /// Indicates the stream identifier that triggered the event if it is available, UNKNOWN_ID otherwise.
    unsigned channel_id = UNKNOWN_ID;
    /// Indicates the source of the event.
    event_source source = event_source::UNDEFINED;
    /// Indicates the event type.
    event_type type = event_type::UNDEFINED;
    /// Optional - Event timestamp.
    optional<uint64_t> timestamp;
  };

  /// Notifies a new event that affected the real-time operation of the radio.
  virtual void on_radio_rt_event(const event_description& description) = 0;
};

} // namespace srsgnb
