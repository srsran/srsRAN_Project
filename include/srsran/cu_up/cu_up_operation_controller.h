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
