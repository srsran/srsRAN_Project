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
