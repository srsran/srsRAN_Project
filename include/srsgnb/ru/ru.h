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

namespace srsgnb {

class ru_downlink_plane;
class ru_uplink_plane;
class ru_timing_notifier;

/// \brief Radio Unit interface.
///
/// The radio unit interface provide downlink and uplink functionality through the uplink and downlink planes, allowing
/// to transmit and receive data using a radio. It also notifies of timing events using the \c ru_timing_notifier.
class radio_unit
{
public:
  virtual ~radio_unit() = default;

  /// Returns the downlink plane interface of this RU.
  virtual ru_downlink_plane& get_downlink_plane() = 0;

  /// Returns the uplink plane interface of this RU.
  virtual ru_uplink_plane& get_uplink_plane() = 0;

  /// \brief Sets the RU timing notifier for this RU.
  virtual void set_timing_notifier(ru_timing_notifier& notifier_) = 0;
};

} // namespace srsgnb