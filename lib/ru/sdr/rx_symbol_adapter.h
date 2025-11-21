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

#include "srsran/phy/lower/lower_phy_rx_symbol_context.h"
#include "srsran/phy/lower/lower_phy_rx_symbol_notifier.h"
#include "srsran/ru/ru_uplink_plane.h"

namespace srsran {

/// Implements a lower physical layer to Radio Unit receive symbol adapter.
class ru_lower_phy_rx_symbol_adapter : public lower_phy_rx_symbol_notifier
{
public:
  explicit ru_lower_phy_rx_symbol_adapter(ru_uplink_plane_rx_symbol_notifier& rx_symbol_handler_) :
    rx_symbol_handler(rx_symbol_handler_)
  {
  }

  // See interface for documentation.
  void
  on_rx_symbol(const lower_phy_rx_symbol_context& context, const shared_resource_grid& grid, bool is_valid) override
  {
    rx_symbol_handler.on_new_uplink_symbol(
        ru_uplink_rx_symbol_context{.slot = context.slot, .sector = context.sector, .symbol_id = context.nof_symbols},
        grid,
        is_valid);
  }

  // See interface for documentation.
  void on_rx_prach_window(const prach_buffer_context& context, shared_prach_buffer buffer) override
  {
    rx_symbol_handler.on_new_prach_window_data(context, std::move(buffer));
  }

private:
  ru_uplink_plane_rx_symbol_notifier& rx_symbol_handler;
};

} // namespace srsran
