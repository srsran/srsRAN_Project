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

namespace srsran {

class ru_downlink_plane_handler;
class ru_uplink_plane_handler;

/// \brief Radio Unit interface.
///
/// The radio unit interface provides downlink and uplink functionality through the uplink and downlink planes, allowing
/// data transmission and reception using a radio. It also notifies timing events using the \c ru_timing_notifier.
class radio_unit
{
public:
  virtual ~radio_unit() = default;

  /// Returns the downlink plane handler interface of this RU.
  virtual ru_downlink_plane_handler& get_downlink_plane_handler() = 0;

  /// Returns the uplink plane interface handler of this RU.
  virtual ru_uplink_plane_handler& get_uplink_plane_handler() = 0;
};

} // namespace srsran