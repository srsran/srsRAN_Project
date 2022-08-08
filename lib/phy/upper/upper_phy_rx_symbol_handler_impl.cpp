/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "upper_phy_rx_symbol_handler_impl.h"
#include "srsgnb/phy/support/prach_buffer_context.h"
#include "srsgnb/phy/support/resource_grid.h"
#include "srsgnb/phy/upper/uplink_processor.h"

using namespace srsgnb;

static const std::string logger_name = "Uplink processor";

upper_phy_rx_symbol_handler_impl::upper_phy_rx_symbol_handler_impl(uplink_processor_pool& ul_processor_pool) :
  ul_processor_pool(ul_processor_pool)
{
}

void upper_phy_rx_symbol_handler_impl::handle_rx_symbol(const upper_phy_rx_symbol_context& context,
                                                        const resource_grid_reader&        grid)
{
  // :TODO: Implement me!!!
}

void upper_phy_rx_symbol_handler_impl::handle_rx_prach_window(const prach_buffer_context& context,
                                                              const prach_buffer&         buffer)
{
  // Get uplink processor.
  uplink_processor& ul_processor = ul_processor_pool.get_processor(context.slot, context.sector);

  // Process PRACH.
  ul_processor.process_prach(buffer, context);
}

void upper_phy_rx_symbol_handler_impl::handle_rx_srs_symbol(const upper_phy_rx_symbol_context& context)
{
  // :TODO: Implement me!!!
}
