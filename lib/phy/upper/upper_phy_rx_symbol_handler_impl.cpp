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

#include "upper_phy_rx_symbol_handler_impl.h"
#include "upper_phy_rx_results_notifier_wrapper.h"
#include "srsran/phy/support/prach_buffer_context.h"
#include "srsran/phy/support/shared_resource_grid.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc.h"
#include "srsran/phy/upper/channel_processors/pusch/formatters.h"
#include "srsran/phy/upper/rx_buffer_pool.h"
#include "srsran/phy/upper/unique_rx_buffer.h"
#include "srsran/phy/upper/uplink_processor.h"
#include "srsran/support/error_handling.h"
#include <utility>

using namespace srsran;

upper_phy_rx_symbol_handler_impl::upper_phy_rx_symbol_handler_impl(uplink_slot_processor_pool& ul_processor_pool_) :
  ul_processor_pool(ul_processor_pool_)
{
}

void upper_phy_rx_symbol_handler_impl::handle_rx_symbol(const upper_phy_rx_symbol_context& context,
                                                        const shared_resource_grid&        grid,
                                                        bool                               is_valid)
{
  // Get uplink processor.
  uplink_slot_processor& ul_proc = ul_processor_pool.get_slot_processor(context.slot);

  // Notify Rx symbol.
  ul_proc.handle_rx_symbol(context.symbol, is_valid);
}

void upper_phy_rx_symbol_handler_impl::handle_rx_prach_window(const prach_buffer_context& context,
                                                              const prach_buffer&         buffer)
{
  // Get uplink processor.
  uplink_slot_processor& ul_proc = ul_processor_pool.get_slot_processor(context.slot);

  // Process PRACH.
  ul_proc.process_prach(buffer, context);
}
