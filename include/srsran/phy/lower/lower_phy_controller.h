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

#include "srsran/gateways/baseband/baseband_gateway_timestamp.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {

/// \brief Lower physical layer - control interface.
///
/// Provides a start and stop methods.
///
/// \remark The methods \c start() and \c stop() must be called in order and only once; other uses will result in
/// undefined behavior.
class lower_phy_controller
{
public:
  /// Default destructor.
  virtual ~lower_phy_controller() = default;

  /// \brief Starts the lower physical layer operation.
  ///
  /// The fist uplink processing block is expected at \c init_time and the first downlink processing block transmission
  /// will be relative to this time.
  ///
  /// \param[in] init_time Initial time in clock ticks.
  virtual void start(baseband_gateway_timestamp init_time) = 0;

  /// \brief Stops the lower physical layer operation.
  ///
  /// Waits for all asynchronous processes to be over before returning.
  virtual void stop() = 0;
};

} // namespace srsran
