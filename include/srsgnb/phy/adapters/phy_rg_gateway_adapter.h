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

#include "srsgnb/phy/lower/lower_phy_rg_handler.h"
#include "srsgnb/phy/upper/upper_phy_rg_gateway.h"

namespace srsran {

/// Implements a generic physical layer resource grid gateway adapter.
class phy_rg_gateway_adapter : public upper_phy_rg_gateway
{
private:
  lower_phy_rg_handler* rg_handler = nullptr;

public:
  /// Connects the adaptor to the lower physical layer gateway.
  void connect(lower_phy_rg_handler* handler) { rg_handler = handler; }

  // See interface for documentation.
  void send(const resource_grid_context& context, const resource_grid_reader& grid) override
  {
    report_fatal_error_if_not(rg_handler, "Adapter is not connected.");
    rg_handler->handle_resource_grid(context, grid);
  }
};

} // namespace srsran
