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

#include "srsgnb/phy/lower/lower_phy_request_handler.h"
#include "srsgnb/phy/upper/upper_phy_rx_symbol_request_notifier.h"
#include "srsgnb/support/error_handling.h"

namespace srsgnb {

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

} // namespace srsgnb
