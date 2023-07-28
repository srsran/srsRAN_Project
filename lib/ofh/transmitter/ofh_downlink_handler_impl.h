/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

namespace srsran {
namespace ofh {

/// Downlink handler implementation configuration.
struct downlink_handler_impl_config {
  /// Downlink eAxCs.
  static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> dl_eaxc;
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
  std::unique_ptr<tx_window_checker>                    window_checker;
  const static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> dl_eaxc;
  std::unique_ptr<data_flow_cplane_scheduling_commands> data_flow_cplane;
  std::unique_ptr<data_flow_uplane_downlink_data>       data_flow_uplane;
  std::shared_ptr<ether::eth_frame_pool>                frame_pool_ptr;
  ether::eth_frame_pool&                                frame_pool;
};

} // namespace ofh
} // namespace srsran
