/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "srsran/phy/support/resource_grid_context.h"
#include "srsran/phy/support/resource_grid_reader.h"

using namespace srsran;
using namespace ofh;

downlink_handler_broadcast_impl::downlink_handler_broadcast_impl(
    srslog::basic_logger&                                 logger_,
    cyclic_prefix                                         cp_,
    const optional<tdd_ul_dl_config_common>&              tdd_config_,
    span<const unsigned>                                  eaxc_data_,
    std::unique_ptr<data_flow_cplane_scheduling_commands> data_flow_cplane_,
    std::unique_ptr<data_flow_uplane_downlink_data>       data_flow_uplane_,
    std::unique_ptr<tx_window_checker>                    window_checker_) :
  logger(logger_),
  cp(cp_),
  tdd_config(tdd_config_),
  dl_eaxc(eaxc_data_.begin(), eaxc_data_.end()),
  data_flow_cplane(std::move(data_flow_cplane_)),
  data_flow_uplane(std::move(data_flow_uplane_)),
  window_checker(std::move(window_checker_))
{
  srsran_assert(data_flow_cplane, "Invalid Control-Plane data flow");
  srsran_assert(data_flow_uplane, "Invalid Use-Plane data flow");
  srsran_assert(window_checker, "Invalid transmission window checker");
}

void downlink_handler_broadcast_impl::handle_dl_data(const resource_grid_context& context,
                                                     const resource_grid_reader&  grid)
{
  if (window_checker->is_late(context.slot)) {
    logger.warning(
        "Dropping downlink resource grid at slot={} and sector={} as it arrived late", context.slot, context.sector);

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
