/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ru_uplink_request_handler_generic_impl.h"
#include "srsran/phy/lower/lower_phy_request_handler.h"

using namespace srsran;

void ru_uplink_request_handler_generic_impl::handle_prach_occasion(const prach_buffer_context& context,
                                                                   prach_buffer&               buffer)
{
  low_phy_handler.request_prach_window(context, buffer);
}

void ru_uplink_request_handler_generic_impl::handle_new_uplink_slot(const resource_grid_context& context,
                                                                    resource_grid&               grid)
{
  low_phy_handler.request_uplink_slot(context, grid);
}
