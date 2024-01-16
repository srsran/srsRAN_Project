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

#include "ofh_downlink_handler_impl.h"
#include "helpers.h"
#include "srsran/instrumentation/traces/du_traces.h"
#include "srsran/phy/support/resource_grid_context.h"
#include "srsran/phy/support/resource_grid_reader.h"

using namespace srsran;
using namespace ofh;

downlink_handler_impl::downlink_handler_impl(const downlink_handler_impl_config&  config,
                                             downlink_handler_impl_dependencies&& dependencies) :
  logger(*dependencies.logger),
  cp(config.cp),
  tdd_config(config.tdd_config),
  dl_eaxc(config.dl_eaxc),
  window_checker(
      *dependencies.logger,
      calculate_nof_symbols_before_ota(config.cp, config.scs, config.dl_processing_time, config.tx_timing_params),
      get_nsymb_per_slot(config.cp),
      to_numerology_value(config.scs)),
  data_flow_cplane(std::move(dependencies.data_flow_cplane)),
  data_flow_uplane(std::move(dependencies.data_flow_uplane)),
  frame_pool_ptr(dependencies.frame_pool_ptr),
  frame_pool(*frame_pool_ptr)
{
  srsran_assert(data_flow_cplane, "Invalid Control-Plane data flow");
  srsran_assert(data_flow_uplane, "Invalid User-Plane data flow");
  srsran_assert(frame_pool_ptr, "Invalid frame pool");
}

void downlink_handler_impl::handle_dl_data(const resource_grid_context& context, const resource_grid_reader& grid)
{
  srsran_assert(grid.get_nof_ports() <= dl_eaxc.size(),
                "Number of RU ports is '{}' and must be equal or greater than the number of cell ports which is '{}'",
                dl_eaxc.size(),
                grid.get_nof_ports());

  // Clear any stale buffers associated with the context slot.
  frame_pool.clear_slot(context.slot);

  if (window_checker.is_late(context.slot)) {
    logger.warning(
        "Dropped late downlink resource grid in slot '{}' and sector#{}. No OFH data will be transmitted for this slot",
        context.slot,
        context.sector);
    l1_tracer << instant_trace_event{"handle_dl_data_late", instant_trace_event::cpu_scope::thread};
    return;
  }

  data_flow_cplane_type_1_context cplane_context;
  cplane_context.slot         = context.slot;
  cplane_context.filter_type  = filter_index_type::standard_channel_filter;
  cplane_context.direction    = data_direction::downlink;
  cplane_context.symbol_range = tdd_config
                                    ? get_active_tdd_dl_symbols(tdd_config.value(), context.slot.slot_index(), cp)
                                    : ofdm_symbol_range(0, grid.get_nof_symbols());

  data_flow_uplane_resource_grid_context uplane_context;
  uplane_context.slot         = context.slot;
  uplane_context.sector       = context.sector;
  uplane_context.symbol_range = cplane_context.symbol_range;

  for (unsigned cell_port_id = 0, e = grid.get_nof_ports(); cell_port_id != e; ++cell_port_id) {
    cplane_context.eaxc = dl_eaxc[cell_port_id];
    // Control-Plane data flow.
    data_flow_cplane->enqueue_section_type_1_message(cplane_context);

    // User-Plane data flow.
    uplane_context.port = cell_port_id;
    uplane_context.eaxc = dl_eaxc[cell_port_id];
    data_flow_uplane->enqueue_section_type_1_message(uplane_context, grid);
  }
}
