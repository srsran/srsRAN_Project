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

#include "processor_handler_adaptor.h"

using namespace srsran;

void processor_handler_adaptor::rg_handler_adaptor::handle_resource_grid(const resource_grid_context& context,
                                                                         const resource_grid_reader&  grid)
{
  pdxch_handler.handle_request(grid, context);
}

void processor_handler_adaptor::request_handler_adaptor::request_prach_window(const prach_buffer_context& context,
                                                                              prach_buffer&               buffer)
{
  prach_handler.handle_request(buffer, context);
}

void processor_handler_adaptor::request_handler_adaptor::request_uplink_slot(const resource_grid_context& context,
                                                                             resource_grid&               grid)
{
  puxch_handler.handle_request(grid, context);
}
