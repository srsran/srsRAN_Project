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

#include "srsran/phy/support/shared_resource_grid.h"
#include "srsran/phy/upper/upper_phy_rg_gateway.h"

namespace srsran {

/// Implemetantion of an upper phy resource grid gateway for test only. It provides a bool that will be set to true when
/// the send method is called.
class upper_phy_rg_gateway_fto : public upper_phy_rg_gateway
{
public:
  bool sent = false;
  void send(const resource_grid_context& context, shared_resource_grid grid) override { sent = true; }
  void clear_sent() { sent = false; }
};

} // namespace srsran
