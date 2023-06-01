/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "srsran/phy/lower/lower_phy_rx_symbol_notifier.h"
#include "srsran/ru/ru_uplink_plane.h"

namespace srsran {

/// Implements a generic lower physical layer to Radio Unit receive symbol adapter.
class ru_rx_symbol_adapter : public lower_phy_rx_symbol_notifier
{
public:
  explicit ru_rx_symbol_adapter(ru_uplink_plane_rx_symbol_notifier& rx_symbol_handler_) :
    rx_symbol_handler(rx_symbol_handler_)
  {
  }

  // See interface for documentation.
  void on_rx_symbol(const lower_phy_rx_symbol_context& context, const resource_grid_reader& grid) override
  {
    ru_uplink_rx_symbol_context upper_context;
    upper_context.slot      = context.slot;
    upper_context.sector    = context.sector;
    upper_context.symbol_id = context.nof_symbols;
    rx_symbol_handler.on_new_uplink_symbol(upper_context, grid);
  }

  // See interface for documentation.
  void on_rx_prach_window(const prach_buffer_context& context, const prach_buffer& buffer) override
  {
    rx_symbol_handler.on_new_prach_window_data(context, buffer);
  }

  // See interface for documentation.
  void on_rx_srs_symbol(const lower_phy_rx_symbol_context& context) override
  {
    ru_uplink_rx_symbol_context upper_context;
    upper_context.slot      = context.slot;
    upper_context.sector    = context.sector;
    upper_context.symbol_id = context.nof_symbols;

    rx_symbol_handler.on_rx_srs_symbol(upper_context);
  }

private:
  ru_uplink_plane_rx_symbol_notifier& rx_symbol_handler;
};

} // namespace srsran