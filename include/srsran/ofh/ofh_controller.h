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
namespace ofh {

/// Open Fronthaul control interface.
class controller
{
public:
  virtual ~controller() = default;

  /// \brief Starts the Open Fronthaul operation.
  /// \note Caller will be blocked until the controller is fully started.
  virtual void start() = 0;

  /// \brief Stops the Open Fronthaul operation.
  /// \note Caller will be blocked until the controller is fully stopped.
  virtual void stop() = 0;
};

} // namespace ofh
} // namespace srsran
