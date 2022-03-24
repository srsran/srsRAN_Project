
#ifndef SRSGNB_RADIO_RADIO_NOTIFIER_H
#define SRSGNB_RADIO_RADIO_NOTIFIER_H

namespace srsgnb {

/// \brief Describes a baseband unit event notifier interface.
///
/// This class describes a an interface used by the baseband unit to notify events related with its operation.
class radio_notifier
{
public:
  /// Describes the different events that can occur at the radio.
  enum class event_type {
    /// Default event value meaning it is not set.
    UNDEFINED = 0,
    /// The transmit data arrives late to the baseband unit.
    LATE,
    /// The transmit data is not generated fast enough.
    UNDERFLOW,
    /// The receive buffers are not read fast enough.
    OVERFLOW,
    /// The event cause is not listed from the above causes.
    OTHER
  };

  /// Describes a radio notification.
  struct event_description {
    /// Indicates the event type.
    event_type event;
    /// Indicates the stream identifier that triggered the event if it is available, 0 otherwise.
    unsigned stream_id;
    /// Indicates the stream identifier that triggered the event if it is available, 0 otherwise.
    unsigned port_id;
  };

  /// Default destructor.
  virtual ~radio_notifier() = default;

  /// Notifies a new event that affected the real-time operation of the radio.
  virtual void on_radio_rt_event(const event_description& description) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_RADIO_RADIO_NOTIFIER_H
