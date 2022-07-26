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

#include "srsgnb/phy/lower/lower_phy_input_gateway.h"
#include "srsgnb/phy/upper/upper_phy_rg_gateway.h"

namespace srsgnb {

/// Implements a generic physical layer resource grid gateway adapter.
class phy_rg_gateway_adapter : public upper_phy_rg_gateway
{
private:
  lower_phy_input_gateway* gateway = nullptr;

public:
  /// Connects the adaptor to the lower physical layer gateway.
  void connect(lower_phy_input_gateway* lower_gateway) { gateway = lower_gateway; }

  // See interface for documentation.
  void send(const resource_grid_context& context, const resource_grid_reader& grid) override
  {
    report_fatal_error_if_not(gateway, "Adapter is not connected.");
    gateway->send(context, grid);
  }
};

} // namespace srsgnb
