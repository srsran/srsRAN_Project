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

namespace srsran {

class ru_controller;
class ru_downlink_plane_handler;
class ru_uplink_plane_handler;

/// \brief Radio Unit interface.
///
/// The Radio Unit interface provides downlink and uplink functionality through the uplink and downlink planes, allowing
/// data transmission and reception using a radio. It also notifies timing events using the \ref ru_timing_notifier.
/// Only a single Radio Unit interface should be used when operating with multiple sectors.
class radio_unit
{
public:
  /// Default destructor.
  virtual ~radio_unit() = default;

  /// Returns the controller interface of this Radio Unit.
  virtual ru_controller& get_controller() = 0;

  /// Returns the downlink plane handler interface of this Radio Unit.
  virtual ru_downlink_plane_handler& get_downlink_plane_handler() = 0;

  /// Returns the uplink plane interface handler of this Radio Unit.
  virtual ru_uplink_plane_handler& get_uplink_plane_handler() = 0;
};

} // namespace srsran
