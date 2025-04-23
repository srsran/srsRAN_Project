/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

namespace srsran {
namespace ofh {

class operation_controller;
class error_notifier;
class receiver;
class metrics_collector;
class transmitter;

/// \brief Open Fronthaul sector interface.
///
/// A sector represents the minimum entity to send and receive data from the Open Fronthaul.
class sector
{
public:
  virtual ~sector() = default;

  /// Returns the Open Fronthaul receiver of this sector.
  virtual receiver& get_receiver() = 0;

  /// Returns the Open Fronthaul transmitter of this sector.
  virtual transmitter& get_transmitter() = 0;

  /// Returns the Open Fronthaul controller of this sector.
  virtual operation_controller& get_operation_controller() = 0;

  /// Returns the Open Fronthaul metrics collector of this sector or nullptr if metrics are disabled.
  virtual metrics_collector* get_metrics_collector() = 0;
};

} // namespace ofh
} // namespace srsran
