/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "uplink_request_processor_impl.h"
#include "srsran/phy/support/shared_resource_grid.h"
#include "srsran/phy/upper/upper_phy_rx_symbol_request_notifier.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/memory_pool/bounded_object_pool.h"

using namespace srsran;

uplink_request_processor_impl::uplink_request_processor_impl(
    upper_phy_rx_symbol_request_notifier&       rx_symbol_request_notifier_,
    std::vector<std::unique_ptr<prach_buffer>>& prach_buffers,
    srslog::basic_logger&                       logger_) :
  rx_symbol_request_notifier(rx_symbol_request_notifier_), prach_pool(prach_buffers), logger(logger_)
{
}

void uplink_request_processor_impl::process_prach_request(const prach_buffer_context& context)
{
  // Grab a PRACH buffer from the pool.
  auto buffer = prach_pool.get();

  if (!buffer) {
    logger.warning("PRACH buffer pool depleted. Ignoring PRACH request");
    return;
  }

  // Notify the PRACH capture request event.
  rx_symbol_request_notifier.on_prach_capture_request(context, std::move(buffer));
}

void uplink_request_processor_impl::process_uplink_slot_request(const resource_grid_context& context,
                                                                const shared_resource_grid&  grid)
{
  rx_symbol_request_notifier.on_uplink_slot_request(context, grid);
}
