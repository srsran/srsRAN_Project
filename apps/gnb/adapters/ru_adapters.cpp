/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ru_adapters.h"
#include "srsran/support/error_handling.h"

using namespace srsran;

void upper_ru_dl_rg_adapter::send(const resource_grid_context& context, const resource_grid_reader& grid)
{
  report_fatal_error_if_not(dl_handler, "Adapter is not connected.");
  dl_handler->handle_dl_data(context, grid);
}

void upper_ru_ul_adapter::on_new_uplink_symbol(const ru_uplink_rx_symbol_context& context,
                                               const resource_grid_reader&        grid)
{
  report_fatal_error_if_not(handler, "Adapter is not connected.");
  handler->handle_rx_symbol({context.sector, context.slot, context.symbol_id}, grid);
}

void upper_ru_ul_adapter::on_new_prach_window_data(const prach_buffer_context& context, const prach_buffer& buffer)
{
  report_fatal_error_if_not(handler, "Adapter is not connected.");
  handler->handle_rx_prach_window(context, buffer);
}

void upper_ru_timing_adapter::on_tti_boundary(slot_point slot)
{
  report_fatal_error_if_not(handler, "Adapter is not connected.");
  handler->handle_tti_boundary({slot});
}

void upper_ru_ul_request_adapter::on_prach_capture_request(const prach_buffer_context& context, prach_buffer& buffer)
{
  report_fatal_error_if_not(ul_handler, "Adapter is not connected.");
  ul_handler->handle_prach_occasion(context, buffer);
}

void upper_ru_ul_request_adapter::on_uplink_slot_request(const resource_grid_context& context, resource_grid& grid)
{
  report_fatal_error_if_not(ul_handler, "Adapter is not connected.");
  ul_handler->handle_new_uplink_slot(context, grid);
}
