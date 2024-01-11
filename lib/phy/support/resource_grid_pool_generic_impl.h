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
