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

#include "uplink_request_processor_impl.h"
#include "srsran/phy/support/prach_buffer_pool.h"
#include "srsran/phy/upper/upper_phy_rx_symbol_request_notifier.h"

using namespace srsran;

uplink_request_processor_impl::uplink_request_processor_impl(
    upper_phy_rx_symbol_request_notifier& rx_symbol_request_notifier_,
    prach_buffer_pool&                    prach_pool_) :
  rx_symbol_request_notifier(rx_symbol_request_notifier_), prach_pool(prach_pool_)
{
}

void uplink_request_processor_impl::process_prach_request(const prach_buffer_context& context)
{
  // Grab a PRACH buffer from the pool.
  prach_buffer& buffer = prach_pool.get_prach_buffer();

  // Notify the PRACH capture request event.
  rx_symbol_request_notifier.on_prach_capture_request(context, buffer);
}

void uplink_request_processor_impl::process_uplink_slot_request(const resource_grid_context& context,
                                                                resource_grid&               grid)
{
  rx_symbol_request_notifier.on_uplink_slot_request(context, grid);
}
