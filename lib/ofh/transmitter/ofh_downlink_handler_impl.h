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

#pragma once

#include "ofh_data_flow_cplane_scheduling_commands.h"
#include "ofh_data_flow_uplane_downlink_data.h"
#include "ofh_tx_window_checker.h"
#include "srsran/adt/span.h"
#include "srsran/adt/static_vector.h"
#include "srsran/ofh/ethernet/ethernet_frame_pool.h"
#include "srsran/ofh/ofh_constants.h"
#include "srsran/ofh/transmitter/ofh_downlink_handler.h"
#include "srsran/ran/tdd/tdd_ul_dl_config.h"

namespace srsran {
namespace ofh {

/// Downlink handler implementation configuration.
struct downlink_handler_impl_config {
  /// Downlink eAxCs.
  static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> dl_eaxc;
  /// Optional TDD configuration.
  optional<tdd_ul_dl_config_common> tdd_config;
  /// Cyclic prefix.
  cyclic_prefix cp;
};

/// Downlink handler implementation dependencies.
struct downlink_handler_impl_dependencies {
  /// Logger
  srslog::basic_logger* logger;
  /// Data flow for Control-Plane.
  std::unique_ptr<data_flow_cplane_scheduling_commands> data_flow_cplane;
  /// Data flow for User-Plane.
  std::unique_ptr<data_flow_uplane_downlink_data> data_flow_uplane;
  /// Transmission window checker.
  std::unique_ptr<tx_window_checker> window_checker;
  /// Ethernet frame pool.
  std::shared_ptr<ether::eth_frame_pool> frame_pool_ptr;
};

/// Open Fronthaul downlink handler implementation.
class downlink_handler_impl : public downlink_handler
{
public:
  downlink_handler_impl(const downlink_handler_impl_config& config, downlink_handler_impl_dependencies&& dependencies);

  // See interface for documentation.
  void handle_dl_data(const resource_grid_context& context, const resource_grid_reader& grid) override;

private:
  srslog::basic_logger&                                 logger;
  const cyclic_prefix                                   cp;
  const optional<tdd_ul_dl_config_common>               tdd_config;
  const static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> dl_eaxc;
  std::unique_ptr<tx_window_checker>                    window_checker;
  std::unique_ptr<data_flow_cplane_scheduling_commands> data_flow_cplane;
  std::unique_ptr<data_flow_uplane_downlink_data>       data_flow_uplane;
  std::shared_ptr<ether::eth_frame_pool>                frame_pool_ptr;
  ether::eth_frame_pool&                                frame_pool;
};

} // namespace ofh
} // namespace srsran
