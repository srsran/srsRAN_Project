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

#include "resource_grid.h"
#include "resource_grid_context.h"
#include <vector>

namespace srsran {

/// Describes a resource grid pool interface
class resource_grid_pool
{
public:
  /// Default destructor
  virtual ~resource_grid_pool() = default;

  /// Get a resource grid for the given context
  /// \param [in] context Provides the given context
  virtual resource_grid& get_resource_grid(const resource_grid_context& context) = 0;
};

} // namespace srsran
