/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/phy/support/resource_grid.h"
#include "srsgnb/phy/support/resource_grid_context.h"

namespace srsgnb {

/// \brief Lower physical layer resource grid handler interface.
///
/// The resource grid handler modulates and transmits the given downlink resource grid through the underlying radio
/// device.
class lower_phy_rg_handler
{
public:
  /// Default destructor.
  virtual ~lower_phy_rg_handler() = default;

  /// \brief Handles the given resource grid to be transmitted.
  ///
  /// \param[in] context Resource grid context.
  /// \param[in] grid Resource grid to transmit.
  virtual void handle_resource_grid(const resource_grid_context& context, const resource_grid_reader& grid) = 0;
};

} // namespace srsgnb
