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

#include "srsran/phy/upper/upper_phy_rx_symbol_request_notifier.h"

namespace srsran {

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

} // namespace srsran
