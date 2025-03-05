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

#include "../support/prach_context_repository.h"
#include "../support/uplink_context_repository.h"
#include "ofh_data_flow_cplane_scheduling_commands.h"
#include "ofh_tx_window_checker.h"
#include "srsran/ofh/ethernet/ethernet_frame_pool.h"
#include "srsran/ofh/transmitter/ofh_transmitter_timing_parameters.h"
#include "srsran/ofh/transmitter/ofh_uplink_request_handler.h"
#include "srsran/ran/tdd/tdd_ul_dl_config.h"

namespace srsran {
namespace ofh {

/// Uplink request handler implementation configuration.
struct uplink_request_handler_impl_config {
  /// Radio sector identifier.
  unsigned sector;
  /// PRACH Contol-Plane enabled flag.
  bool is_prach_cp_enabled;
  /// PRACH eAxC.
  static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> prach_eaxc;
  /// Uplink data eAxC.
  static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> ul_data_eaxc;
  /// Optional TDD configuration.
  std::optional<tdd_ul_dl_config_common> tdd_config;
  /// Cyclic prefix.
  cyclic_prefix cp;
  /// Subcarrier spacing.
  subcarrier_spacing scs;
  /// Uplink request processing time in microseconds.
  std::chrono::microseconds ul_processing_time;
  /// Transmission window timing parameters for delay management.
  tx_window_timing_parameters tx_timing_params;
};

/// Uplink request handler implmentation dependencies.
struct uplink_request_handler_impl_dependencies {
  /// Logger.
  srslog::basic_logger* logger = nullptr;
  /// Uplink slot context repository.
  std::shared_ptr<uplink_context_repository> ul_slot_repo;
  /// Uplink PRACH context repository.
  std::shared_ptr<prach_context_repository> ul_prach_repo;
  /// Ethernet frame pool.
  std::shared_ptr<ether::eth_frame_pool> frame_pool;
  /// Data flow for Control-Plane scheduling commands.
  std::unique_ptr<data_flow_cplane_scheduling_commands> data_flow;
};

/// Open Fronthaul uplink request handler.
class uplink_request_handler_impl : public uplink_request_handler
{
public:
  explicit uplink_request_handler_impl(const uplink_request_handler_impl_config&  config,
                                       uplink_request_handler_impl_dependencies&& dependencies);

  // See interface for documentation.
  void handle_prach_occasion(const prach_buffer_context& context, prach_buffer& buffer) override;

  // See interface for documentation.
  void handle_new_uplink_slot(const resource_grid_context& context, const shared_resource_grid& grid) override;

  // See interface for documentation.
  void set_error_notifier(error_notifier& notifier) override { err_notifier = &notifier; }

  /// Returns the OTA symbol boundary notifier of this uplink request handler implementation.
  ota_symbol_boundary_notifier& get_ota_symbol_boundary_notifier() { return window_checker; }

private:
  srslog::basic_logger&                                 logger;
  const bool                                            is_prach_cp_enabled;
  const cyclic_prefix                                   cp;
  const std::optional<tdd_ul_dl_config_common>          tdd_config;
  const static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> prach_eaxc;
  const static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> ul_eaxc;
  tx_window_checker                                     window_checker;
  std::shared_ptr<uplink_context_repository>            ul_slot_repo;
  std::shared_ptr<prach_context_repository>             ul_prach_repo;
  std::unique_ptr<data_flow_cplane_scheduling_commands> data_flow;
  std::shared_ptr<ether::eth_frame_pool>                frame_pool;
  error_notifier*                                       err_notifier;
};

} // namespace ofh
} // namespace srsran
