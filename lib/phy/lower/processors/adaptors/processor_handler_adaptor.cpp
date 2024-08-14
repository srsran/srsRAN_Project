/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "processor_handler_adaptor.h"
#include "srsran/phy/support/shared_resource_grid.h"

using namespace srsran;

void processor_handler_adaptor::rg_handler_adaptor::handle_resource_grid(const resource_grid_context& context,
                                                                         const shared_resource_grid&  grid)
{
  pdxch_handler.handle_request(grid, context);
}

void processor_handler_adaptor::request_handler_adaptor::request_prach_window(const prach_buffer_context& context,
                                                                              prach_buffer&               buffer)
{
  prach_handler.handle_request(buffer, context);
}

void processor_handler_adaptor::request_handler_adaptor::request_uplink_slot(const resource_grid_context& context,
                                                                             const shared_resource_grid&  grid)
{
  puxch_handler.handle_request(grid, context);
}
