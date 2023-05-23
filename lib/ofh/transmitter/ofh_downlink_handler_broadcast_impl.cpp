/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ofh_downlink_handler_broadcast_impl.h"
#include "srsran/phy/support/resource_grid_context.h"

using namespace srsran;
using namespace ofh;

downlink_handler_broadcast_impl::downlink_handler_broadcast_impl(
    span<const unsigned>                                  eaxc_data_,
    std::unique_ptr<data_flow_cplane_scheduling_commands> data_flow_cplane_,
    std::unique_ptr<data_flow_uplane_downlink_data>       data_flow_uplane_) :
  eaxc_data(eaxc_data_.begin(), eaxc_data_.end()),
  data_flow_cplane(std::move(data_flow_cplane_)),
  data_flow_uplane(std::move(data_flow_uplane_))
{
  srsran_assert(data_flow_cplane, "Invalid Control-Plane data flow");
  srsran_assert(data_flow_uplane, "Invalid Use-Plane data flow");
}

void downlink_handler_broadcast_impl::handle_dl_data(const resource_grid_context& context,
                                                     const resource_grid_reader&  grid)
{
  for (auto eaxc : eaxc_data) {
    // Control-Plane data flow.
    data_flow_cplane->enqueue_section_type_1_message(
        context.slot, eaxc, data_direction::downlink, filter_index_type::standard_channel_filter);

    // User-Plane data flow.
    data_flow_uplane->enqueue_section_type_1_message(context, grid, eaxc);
  }
}
