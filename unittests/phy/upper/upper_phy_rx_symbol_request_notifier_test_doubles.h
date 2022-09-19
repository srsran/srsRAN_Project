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

#include "srsgnb/phy/upper/upper_phy_rx_symbol_request_notifier.h"

namespace srsgnb {

class upper_phy_rx_symbol_request_notifier_spy : public upper_phy_rx_symbol_request_notifier
{
  bool prach_capture_request_notified = false;

public:
  void on_prach_capture_request(const prach_buffer_context& context, prach_buffer& buffer) override
  {
    prach_capture_request_notified = true;
  }
  void on_uplink_slot_request(const resource_grid_context& context, resource_grid& grid) override {}

  bool has_prach_result_been_notified() const { return prach_capture_request_notified; }
};

} // namespace srsgnb
