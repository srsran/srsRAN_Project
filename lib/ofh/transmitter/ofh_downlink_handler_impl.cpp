/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ofh_downlink_handler_impl.h"
#include "srsran/phy/support/resource_grid_context.h"
#include "srsran/phy/support/resource_grid_reader.h"

using namespace srsran;
using namespace ofh;

downlink_handler_impl::downlink_handler_impl(span<const unsigned>                                  eaxc_data_,
                                             std::unique_ptr<data_flow_cplane_scheduling_commands> data_flow_cplane_,
                                             std::unique_ptr<data_flow_uplane_downlink_data>       data_flow_uplane_) :
  dl_eaxc(eaxc_data_.begin(), eaxc_data_.end()),
  data_flow_cplane(std::move(data_flow_cplane_)),
  data_flow_uplane(std::move(data_flow_uplane_))
{
  srsran_assert(eaxc_data_.size() <= MAX_NOF_SUPPORTED_EAXC,
                "Unsupported number of Radio Unit downlink ports={}. Currently supporting up to {} ports",
                eaxc_data_.size(),
                MAX_NOF_SUPPORTED_EAXC);
  srsran_assert(data_flow_cplane, "Invalid Control-Plane data flow");
  srsran_assert(data_flow_uplane, "Invalid Use-Plane data flow");
}

void downlink_handler_impl::handle_dl_data(const resource_grid_context& context, const resource_grid_reader& grid)
{
  srsran_assert(grid.get_nof_ports() <= dl_eaxc.size(),
                "RU number of ports={} must be equal or greater than cell number of ports={}",
                dl_eaxc.size(),
                grid.get_nof_ports());

  for (unsigned cell_port_id = 0, e = grid.get_nof_ports(); cell_port_id != e; ++cell_port_id) {
    // Control-Plane data flow.
    data_flow_cplane->enqueue_section_type_1_message(
        context.slot, dl_eaxc[cell_port_id], data_direction::downlink, filter_index_type::standard_channel_filter);

    // User-Plane data flow.
    data_flow_resource_grid_context df_context;
    df_context.slot   = context.slot;
    df_context.sector = context.sector;
    df_context.port   = cell_port_id;
    data_flow_uplane->enqueue_section_type_1_message(df_context, grid, dl_eaxc[cell_port_id]);
  }
}
