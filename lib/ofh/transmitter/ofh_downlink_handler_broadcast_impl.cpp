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

#include "ofh_downlink_handler_broadcast_impl.h"
#include "helpers.h"
#include "srsran/ofh/ofh_error_notifier.h"
#include "srsran/phy/support/resource_grid_context.h"
#include "srsran/phy/support/resource_grid_reader.h"

using namespace srsran;
using namespace ofh;

namespace {

/// Open Fronthaul error notifier dummy implementation.
class error_notifier_dummy : public error_notifier
{
public:
  void on_late_downlink_message(const error_context& context) override {}
};

} // namespace

/// Dummy error notifier for the downlink handler construction.
static error_notifier_dummy dummy_err_notifier;

downlink_handler_broadcast_impl::downlink_handler_broadcast_impl(
    const downlink_handler_broadcast_impl_config&  config,
    downlink_handler_broadcast_impl_dependencies&& dependencies) :
  sector_id(config.sector),
  logger(*dependencies.logger),
  cp(config.cp),
  tdd_config(config.tdd_config),
  dl_eaxc(config.dl_eaxc.begin(), config.dl_eaxc.end()),
  data_flow_cplane(std::move(dependencies.data_flow_cplane)),
  data_flow_uplane(std::move(dependencies.data_flow_uplane)),
  window_checker(
      *dependencies.logger,
      calculate_nof_symbols_before_ota(config.cp, config.scs, config.dl_processing_time, config.tx_timing_params),
      get_nsymb_per_slot(config.cp),
      to_numerology_value(config.scs)),
  frame_pool_ptr(dependencies.frame_pool_ptr),
  frame_pool(*frame_pool_ptr),
  err_notifier(dummy_err_notifier)
{
  srsran_assert(data_flow_cplane, "Invalid Control-Plane data flow");
  srsran_assert(data_flow_uplane, "Invalid User-Plane data flow");
  srsran_assert(frame_pool_ptr, "Invalid frame pool");
}

void downlink_handler_broadcast_impl::handle_dl_data(const resource_grid_context& context,
                                                     const resource_grid_reader&  grid)
{
  // Clear any stale buffers associated with the context slot.
  frame_pool.clear_downlink_slot(context.slot, logger);

  if (window_checker.is_late(context.slot)) {
    err_notifier.get().on_late_downlink_message({context.slot, sector_id});
    logger.warning(
        "Dropped late downlink resource grid in slot '{}' and sector#{}. No OFH data will be transmitted for this slot",
        context.slot,
        context.sector);

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
  uplane_context.port         = 0;

  for (auto eaxc : dl_eaxc) {
    cplane_context.eaxc = eaxc;
    // Control-Plane data flow.
    data_flow_cplane->enqueue_section_type_1_message(cplane_context);

    // User-Plane data flow.
    uplane_context.eaxc = eaxc;
    data_flow_uplane->enqueue_section_type_1_message(uplane_context, grid);
  }
}
