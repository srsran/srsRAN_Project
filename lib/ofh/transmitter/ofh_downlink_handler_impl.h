/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include "srsran/ofh/transmitter/ofh_transmitter_configuration.h"
#include "srsran/ran/tdd/tdd_ul_dl_config.h"

namespace srsran {
namespace ofh {

/// Downlink handler implementation configuration.
struct downlink_handler_impl_config {
  /// Radio sector identifier.
  unsigned sector;
  /// Downlink eAxCs.
  static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> dl_eaxc;
  /// Optional TDD configuration.
  std::optional<tdd_ul_dl_config_common> tdd_config;
  /// Cyclic prefix.
  cyclic_prefix cp;
  /// Subcarrier spacing.
  subcarrier_spacing scs;
  /// Downlink processing time in microseconds.
  std::chrono::microseconds dl_processing_time;
  /// Transmission window timing parameters for delay management.
  tx_window_timing_parameters tx_timing_params;
};

/// Downlink handler implementation dependencies.
struct downlink_handler_impl_dependencies {
  /// Logger
  srslog::basic_logger* logger;
  /// Data flow for Control-Plane.
  std::unique_ptr<data_flow_cplane_scheduling_commands> data_flow_cplane;
  /// Data flow for User-Plane.
  std::unique_ptr<data_flow_uplane_downlink_data> data_flow_uplane;
  /// Ethernet frame pool.
  std::shared_ptr<ether::eth_frame_pool> frame_pool;
};

/// Open Fronthaul downlink handler implementation.
class downlink_handler_impl : public downlink_handler
{
public:
  downlink_handler_impl(const downlink_handler_impl_config& config, downlink_handler_impl_dependencies&& dependencies);

  // See interface for documentation.
  void handle_dl_data(const resource_grid_context& context, const shared_resource_grid& grid) override;

  // See interface for documentation.
  void set_error_notifier(error_notifier& notifier) override { err_notifier = &notifier; }

  /// Returns the OTA symbol boundary notifier of this downlink handler implementation.
  ota_symbol_boundary_notifier& get_ota_symbol_boundary_notifier() { return window_checker; }

private:
  const unsigned                                        sector_id;
  srslog::basic_logger&                                 logger;
  const cyclic_prefix                                   cp;
  const std::optional<tdd_ul_dl_config_common>          tdd_config;
  const static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> dl_eaxc;
  tx_window_checker                                     window_checker;
  std::unique_ptr<data_flow_cplane_scheduling_commands> data_flow_cplane;
  std::unique_ptr<data_flow_uplane_downlink_data>       data_flow_uplane;
  std::shared_ptr<ether::eth_frame_pool>                frame_pool;
  error_notifier*                                       err_notifier;
};

} // namespace ofh
} // namespace srsran
