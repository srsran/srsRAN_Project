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

#include "resource_grid.h"
#include "resource_grid_context.h"
#include "shared_resource_grid.h"
#include <vector>

namespace srsran {

/// Resource grid pool interface.
class resource_grid_pool
{
public:
  /// Default destructor.
  virtual ~resource_grid_pool() = default;

  /// \brief Allocates a unique resource grid.
  /// \param [in] context Allocation context.
  virtual shared_resource_grid allocate_resource_grid(const resource_grid_context& context) = 0;
};

} // namespace srsran
