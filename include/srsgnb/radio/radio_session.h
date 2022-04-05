
#ifndef SRSGNB_RADIO_RADIO_SESSION_H
#define SRSGNB_RADIO_RADIO_SESSION_H

#include "srsgnb/radio/radio_configuration.h"
#include "srsgnb/radio/radio_data_plane.h"
#include "srsgnb/radio/radio_management_plane.h"
#include "srsgnb/radio/radio_notifier.h"

namespace srsgnb {

/// Describes a radio baseband unit session interface.
class radio_session : radio_base
{
public:
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

} // namespace srsgnb
#endif // SRSGNB_RADIO_RADIO_SESSION_H
