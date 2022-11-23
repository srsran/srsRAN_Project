/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "uplink_request_processor_impl.h"
#include "srsgnb/phy/support/prach_buffer_pool.h"
#include "srsgnb/phy/upper/upper_phy_rx_symbol_request_notifier.h"

using namespace srsgnb;

uplink_request_processor_impl::uplink_request_processor_impl(
    upper_phy_rx_symbol_request_notifier& rx_symbol_request_notifier,
    prach_buffer_pool&                    prach_pool) :
  rx_symbol_request_notifier(rx_symbol_request_notifier), prach_pool(prach_pool)
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
