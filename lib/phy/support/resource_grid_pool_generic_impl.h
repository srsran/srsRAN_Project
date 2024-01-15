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

#include "srsran/adt/complex.h"
#include "srsran/adt/concurrent_queue.h"
#include "srsran/adt/ring_buffer.h"
#include "srsran/phy/support/resource_grid_pool.h"
#include "srsran/srsvec/aligned_vec.h"
#include "srsran/support/executors/task_executor.h"
#include <vector>

namespace srsran {

/// Implements a generic resource grid pool.
class resource_grid_pool_generic_impl : public resource_grid_pool
{
public:
  using resource_grid_ptr = std::unique_ptr<resource_grid>;

  /// \brief Constructs a resource grid pool.
  /// \param grids_ Actual vector containing resource grids.
  resource_grid_pool_generic_impl(std::vector<resource_grid_ptr> grids_);

  // See interface for documentation.
  resource_grid& get_resource_grid(const resource_grid_context& context) override;

private:
  unsigned count = 0;
  /// Actual pool of resource grids.
  std::vector<resource_grid_ptr> grids;
};

} // namespace srsran
