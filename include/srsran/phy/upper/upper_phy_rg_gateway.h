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

#include "srsran/phy/support/resource_grid_context.h"

namespace srsran {

class shared_resource_grid;

/// Interface of the upper physical layer resource grid gateway.
class upper_phy_rg_gateway
{
public:
  /// Default destructor.
  virtual ~upper_phy_rg_gateway() = default;

  /// \brief Sends the given resource grid through the gateway.
  ///
  /// \param[in] context Context the resource grid belongs to.
  /// \param[in] grid    Resource grid.
  virtual void send(const resource_grid_context& context, shared_resource_grid grid) = 0;
};

} // namespace srsran
