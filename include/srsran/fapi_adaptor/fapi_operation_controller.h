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
namespace fapi {

/// FAPI operation controller interface.
class operation_controller
{
public:
  virtual ~operation_controller() = default;

  /// Starts the underlying PHY. Returns true on success, false otherwise.
  virtual bool start() = 0;

  /// Stops the underlying PHY. Returns true on success, false otherwise.
  virtual bool stop() = 0;
};

} // namespace fapi
} // namespace srsran
