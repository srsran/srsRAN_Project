
#ifndef SRSGNB_RADIO_RADIO_SESSION_H
#define SRSGNB_RADIO_RADIO_SESSION_H

#include "srsgnb/radio/radio_configuration.h"
#include "srsgnb/radio/radio_data_plane.h"
#include "srsgnb/radio/radio_management_plane.h"
#include "srsgnb/radio/radio_notifier.h"
#include <memory>

namespace srsgnb {

/// Describes a radio baseband unit session interface.
class radio_session
{
public:
  /// Default destructor.
  virtual ~radio_session() = default;

  /// \brief Gets the management plane.
  /// \return The reference to the management plane for this radio session.
  virtual radio_management_plane& get_management_plane() = 0;

  /// \brief Gets the data plane.
  /// \return The reference to the data plane for this radio session.
  virtual radio_data_plane& get_data_plane() = 0;

  /// \brief Stops the radio session operation.
  /// \remark Any call to transmit or receive after calling stop() will return instantly without interacting with the
  /// physical radio.
  virtual void stop() = 0;
};

/// Describes a radio session factory for a determined radio driver.
class radio_session_factory
{
public:
  /// Default destructor.
  virtual ~radio_session_factory() = default;

  /// \brief Creates a new radio session with the given configuration parameters.
  /// \param[in] config Provides the configuration parameters.
  /// \param[in] notifier Provides radio event notifier interface.
  /// \return The ownership to a radio session if the session was successfully created.
  virtual std::unique_ptr<radio_session> create(const radio_configuration::radio& config, radio_notifier& notifier) = 0;
};

} // namespace srsgnb
#endif // SRSGNB_RADIO_RADIO_SESSION_H
