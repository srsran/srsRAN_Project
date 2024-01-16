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

#include "srsran/phy/lower/lower_phy_request_handler.h"
#include "srsran/phy/upper/upper_phy_rx_symbol_request_notifier.h"
#include "srsran/support/error_handling.h"

namespace srsran {

/// Implements a generic physical layer adapter for symbol capture event requests.
class phy_rx_symbol_request_adapter : public upper_phy_rx_symbol_request_notifier
{
  lower_phy_request_handler* rx_symbol_request_handler = nullptr;

public:
  /// Connects the adaptor to the lower physical layer handler.
  void connect(lower_phy_request_handler* lower_handler) { rx_symbol_request_handler = lower_handler; }

  // See interface for documentation.
  void on_prach_capture_request(const prach_buffer_context& context, prach_buffer& buffer) override
  {
    report_fatal_error_if_not(rx_symbol_request_handler, "Adapter is not connected.");
    rx_symbol_request_handler->request_prach_window(context, buffer);
  }

  // See interface for documentation.
  void on_uplink_slot_request(const resource_grid_context& context, resource_grid& grid) override
  {
    report_fatal_error_if_not(rx_symbol_request_handler, "Adapter is not connected.");

    rx_symbol_request_handler->request_uplink_slot(context, grid);
  }
};

} // namespace srsran
