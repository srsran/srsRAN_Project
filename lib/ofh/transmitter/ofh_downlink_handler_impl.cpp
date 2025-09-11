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

#include "ofh_downlink_handler_impl.h"
#include "../support/logger_utils.h"
#include "helpers.h"
#include "srsran/instrumentation/traces/ofh_traces.h"
#include "srsran/ofh/ofh_error_notifier.h"
#include "srsran/phy/support/resource_grid_context.h"
#include "srsran/phy/support/resource_grid_reader.h"
#include "srsran/phy/support/shared_resource_grid.h"

using namespace srsran;
using namespace ofh;

downlink_handler_impl::downlink_handler_impl(const downlink_handler_impl_config&  config,
                                             downlink_handler_impl_dependencies&& dependencies) :
  sector_id(config.sector),
  logger(dependencies.logger),
  cp(config.cp),
  tdd_config(config.tdd_config),
  dl_eaxc(config.dl_eaxc),
  window_checker(
      dependencies.logger,
      config.sector,
      calculate_nof_symbols_before_ota(config.cp, config.scs, config.dl_processing_time, config.tx_timing_params),
      get_nsymb_per_slot(config.cp)),
  data_flow_cplane(std::move(dependencies.data_flow_cplane)),
  data_flow_uplane(std::move(dependencies.data_flow_uplane)),
  frame_pool_dl_cp(std::move(dependencies.frame_pool_dl_cp)),
  frame_pool_dl_up(std::move(dependencies.frame_pool_dl_up)),
  err_notifier(dependencies.err_notifier),
  metrics_collector(*data_flow_cplane, *data_flow_uplane, window_checker),
  enable_log_warnings_for_lates(config.enable_log_warnings_for_lates)
{
  srsran_assert(data_flow_cplane, "Invalid Control-Plane data flow");
  srsran_assert(data_flow_uplane, "Invalid User-Plane data flow");
  srsran_assert(frame_pool_dl_cp, "Invalid downlink Control-Plane frame pool");
  srsran_assert(frame_pool_dl_up, "Invalid downlink User-Plane frame pool");
}

void downlink_handler_impl::start()
{
  is_running.store(true, std::memory_order_relaxed);
}

void downlink_handler_impl::stop()
{
  if (!is_running.load(std::memory_order_relaxed)) {
    return;
  }

  // Stop accepting grids.
  is_running.store(false, std::memory_order_relaxed);

  // Stop the data flows.
  data_flow_cplane->get_operation_controller().stop();
  data_flow_uplane->get_operation_controller().stop();
}

void downlink_handler_impl::handle_dl_data(const resource_grid_context& context, const shared_resource_grid& grid)
{
  // Do nothing if handler is not running.
  if (!is_running.load(std::memory_order_relaxed)) {
    return;
  }

  const resource_grid_reader& reader = grid.get_reader();
  srsran_assert(reader.get_nof_ports() <= dl_eaxc.size(),
                "Number of RU ports is '{}' and must be equal or greater than the number of cell ports which is '{}'",
                dl_eaxc.size(),
                reader.get_nof_ports());

  trace_point tp = ofh_tracer.now();

  // Clear any stale buffers associated with the context slot.
  metrics_collector.update_cp_dl_lates(frame_pool_dl_cp->clear_slot(context.slot, context.sector));
  metrics_collector.update_up_dl_lates(frame_pool_dl_up->clear_slot(context.slot, context.sector));

  // Nothing to do on empty resource grids.
  if (grid.get_reader().is_empty()) {
    return;
  }

  if (window_checker.is_late(context.slot)) {
    log_conditional_warning(
        logger,
        enable_log_warnings_for_lates,
        "Sector#{}: dropped late downlink resource grid in slot '{}'. No OFH data will be transmitted for this slot",
        sector_id,
        context.slot);
    ofh_tracer << trace_event("ofh_handle_dl_late", tp);

    err_notifier.on_late_downlink_message({context.slot, sector_id});
    return;
  }

  data_flow_cplane_type_1_context cplane_context;
  cplane_context.slot         = context.slot;
  cplane_context.filter_type  = filter_index_type::standard_channel_filter;
  cplane_context.direction    = data_direction::downlink;
  cplane_context.symbol_range = tdd_config ? get_active_tdd_dl_symbols(*tdd_config, context.slot.slot_index(), cp)
                                           : ofdm_symbol_range(0, reader.get_nof_symbols());

  data_flow_uplane_resource_grid_context uplane_context;
  uplane_context.slot         = context.slot;
  uplane_context.sector       = context.sector;
  uplane_context.symbol_range = cplane_context.symbol_range;

  for (unsigned cell_port_id = 0, e = reader.get_nof_ports(); cell_port_id != e; ++cell_port_id) {
    cplane_context.eaxc = dl_eaxc[cell_port_id];
    // Control-Plane data flow.
    data_flow_cplane->enqueue_section_type_1_message(cplane_context);

    // User-Plane data flow.
    uplane_context.port = cell_port_id;
    uplane_context.eaxc = dl_eaxc[cell_port_id];
    data_flow_uplane->enqueue_section_type_1_message(uplane_context, grid);
  }

  ofh_tracer << trace_event("ofh_handle_downlink", tp);
}
