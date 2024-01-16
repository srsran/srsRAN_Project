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
#include "srsran/phy/lower/lower_phy_rx_symbol_notifier.h"
#include "srsran/phy/upper/upper_phy_rx_symbol_handler.h"

namespace srsran {

/// Implements a generic physical layer receive symbol adapter.
class phy_rx_symbol_adapter : public lower_phy_rx_symbol_notifier
{
private:
  upper_phy_rx_symbol_handler* rx_symbol_handler = nullptr;

public:
  /// Connects the adaptor to the upper physical layer handler.
  void connect(upper_phy_rx_symbol_handler* upper_handler) { rx_symbol_handler = upper_handler; }

  // See interface for documentation.
  void on_rx_symbol(const lower_phy_rx_symbol_context& context, const resource_grid_reader& grid) override
  {
    report_fatal_error_if_not(rx_symbol_handler, "Adapter is not connected.");
    upper_phy_rx_symbol_context upper_context;
    upper_context.slot   = context.slot;
    upper_context.sector = context.sector;
    upper_context.symbol = context.nof_symbols;
    rx_symbol_handler->handle_rx_symbol(upper_context, grid);
  }

  // See interface for documentation.
  void on_rx_prach_window(const prach_buffer_context& context, const prach_buffer& buffer) override
  {
    report_fatal_error_if_not(rx_symbol_handler, "Adapter is not connected.");
    rx_symbol_handler->handle_rx_prach_window(context, buffer);
  }

  // See interface for documentation.
  void on_rx_srs_symbol(const lower_phy_rx_symbol_context& context) override
  {
    report_fatal_error_if_not(rx_symbol_handler, "Adapter is not connected.");
    upper_phy_rx_symbol_context upper_context;
    upper_context.slot   = context.slot;
    upper_context.sector = context.sector;
    upper_context.symbol = context.nof_symbols;
    rx_symbol_handler->handle_rx_srs_symbol(upper_context);
  }
};

} // namespace srsran
