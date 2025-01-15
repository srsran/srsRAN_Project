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
namespace srs_cu_up {

/// CU-UP operation controller interface that allows to start/stop the CU-UP.
class cu_up_operation_controller
{
public:
  virtual ~cu_up_operation_controller() = default;

  /// Starts the CU-UP.
  virtual void start() = 0;

  /// Stops the CU-UP.
  virtual void stop() = 0;
};

} // namespace srs_cu_up
} // namespace srsran
