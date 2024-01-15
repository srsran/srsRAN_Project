/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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
