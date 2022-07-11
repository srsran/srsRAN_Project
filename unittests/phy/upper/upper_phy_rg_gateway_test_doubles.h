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

#include "srsgnb/phy/upper/upper_phy_rg_gateway.h"

namespace srsgnb {

/// Implemetantion of an upper phy resource grid gateway for test only. It provides a bool that will be set to true when
/// the send method is called.
class upper_phy_rg_gateway_fto : public upper_phy_rg_gateway
{
public:
  bool sent = false;
  void send(const resource_grid_context& context, const resource_grid_reader& grid) override { sent = true; }
  void clear_sent() { sent = false; }
};

} // namespace srsgnb
