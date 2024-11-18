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
namespace srs_cu_cp {

/// CU-CP power controller interface that allows to start/stop a CU-CP.
class cu_cp_power_controller
{
public:
  virtual ~cu_cp_power_controller() = default;

  /// Starts the CU-CP.
  virtual void start() = 0;

  /// Stops the CU-CP.
  virtual void stop() = 0;
};

} // namespace srs_cu_cp
} // namespace srsran
