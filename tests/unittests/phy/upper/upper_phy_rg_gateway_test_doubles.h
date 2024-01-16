/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/phy/upper/upper_phy_rg_gateway.h"

namespace srsran {

/// Implemetantion of an upper phy resource grid gateway for test only. It provides a bool that will be set to true when
/// the send method is called.
class upper_phy_rg_gateway_fto : public upper_phy_rg_gateway
{
public:
  bool sent = false;
  void send(const resource_grid_context& context, const resource_grid_reader& grid) override { sent = true; }
  void clear_sent() { sent = false; }
};

} // namespace srsran
