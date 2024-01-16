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

#include "srsran/phy/lower/lower_phy_rx_symbol_context.h"
#include "srsran/phy/lower/processors/uplink/puxch/puxch_processor_notifier.h"
#include "srsran/phy/support/resource_grid_context.h"

namespace srsran {

class puxch_processor_notifier_spy : public puxch_processor_notifier
{
public:
  struct rx_symbol_entry {
    const resource_grid_reader* grid;
    lower_phy_rx_symbol_context context;
  };

  void on_puxch_request_late(const resource_grid_context& context) override { request_late.emplace_back(context); }

  void on_rx_symbol(const resource_grid_reader& grid, const lower_phy_rx_symbol_context& context) override
  {
    rx_symbol_entry entry = {&grid, context};
    rx_symbol.emplace_back(std::move(entry));
  }

  const std::vector<resource_grid_context>& get_request_late() const { return request_late; }

  const std::vector<rx_symbol_entry>& get_rx_symbol() const { return rx_symbol; }

  unsigned get_nof_notifications() const { return request_late.size() + rx_symbol.size(); }

  void clear_notifications()
  {
    request_late.clear();
    rx_symbol.clear();
  }

private:
  std::vector<resource_grid_context> request_late;
  std::vector<rx_symbol_entry>       rx_symbol;
};

} // namespace srsran