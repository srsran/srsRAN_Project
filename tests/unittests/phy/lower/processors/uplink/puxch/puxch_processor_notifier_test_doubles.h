/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/phy/lower/lower_phy_rx_symbol_context.h"
#include "srsran/phy/lower/processors/uplink/puxch/puxch_processor_notifier.h"
#include "srsran/phy/support/resource_grid_context.h"
#include "srsran/phy/support/shared_resource_grid.h"

namespace srsran {

class puxch_processor_notifier_spy : public puxch_processor_notifier
{
public:
  struct rx_symbol_entry {
    const resource_grid_reader* grid;
    lower_phy_rx_symbol_context context;
  };

  void on_puxch_request_late(const resource_grid_context& context) override { request_late.emplace_back(context); }

  void on_rx_symbol(const shared_resource_grid& grid, const lower_phy_rx_symbol_context& context) override
  {
    rx_symbol_entry entry = rx_symbol_entry{&grid.get_reader(), context};
    rx_symbol.emplace_back(entry);
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