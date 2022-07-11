/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/phy/resource_grid.h"
#include "srsgnb/phy/resource_grid_context.h"

namespace srsgnb {

/// Describes the upper physical layer output gateway.
class upper_phy_rg_gateway
{
public:
  /// Default destructor.
  virtual ~upper_phy_rg_gateway() = default;

  /// \brief Sends resource grid through the gateway.
  /// \param[in] context Indicates the context the resource grid belongs.
  /// \param[in] grid Provides the resource grid reader instance.
  virtual void send(const resource_grid_context& context, const resource_grid_reader& grid) = 0;
};

} // namespace srsgnb
