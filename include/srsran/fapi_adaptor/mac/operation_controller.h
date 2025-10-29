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
namespace fapi_adaptor {

/// FAPI adaptor operation controller.
class operation_controller
{
public:
  virtual ~operation_controller() = default;

  /// Starts the controller.
  virtual void start() = 0;

  /// Stops the controller.
  virtual void stop() = 0;
};

} // namespace fapi_adaptor
} // namespace srsran
