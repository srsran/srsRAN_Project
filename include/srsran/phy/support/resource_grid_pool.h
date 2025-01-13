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

#include "srsran/phy/support/resource_grid.h"
#include "srsran/phy/support/resource_grid_context.h"
#include "srsran/phy/support/shared_resource_grid.h"
#include <vector>

namespace srsran {

/// Resource grid pool interface.
class resource_grid_pool
{
public:
  /// Default destructor.
  virtual ~resource_grid_pool() = default;

  /// \brief Allocates a resource grid for the given context.
  ///
  /// Attempts to allocate a resource grid based on the provided allocation context. If the system is under high load,
  /// the allocation might fail. When allocation fails, the reason for the failure is logged to the \e PHY logger
  /// channel.
  ///
  /// \param [in] slot The slot for resource grid allocation.
  /// \return A valid shared resource grid if the allocation is successful; otherwise, an invalid shared resource grid.
  virtual shared_resource_grid allocate_resource_grid(slot_point slot) = 0;
};

} // namespace srsran
