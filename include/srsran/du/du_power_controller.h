/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

namespace srsran {

/// DU power controller interface that allows to start/stop a DU.
class du_power_controller
{
public:
  virtual ~du_power_controller() = default;

  /// Starts the DU.
  virtual void start() = 0;

  /// Stops the DU.
  virtual void stop() = 0;
};

} // namespace srsran