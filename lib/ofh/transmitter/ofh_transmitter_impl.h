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

#include "ofh_downlink_handler_impl.h"
#include "ofh_message_transmitter_impl.h"
#include "ofh_transmitter_metrics_collector_impl.h"
#include "ofh_transmitter_ota_symbol_task_dispatcher.h"
#include "ofh_uplink_request_handler_impl.h"
#include "srsran/ofh/transmitter/ofh_transmitter.h"
#include "srsran/ofh/transmitter/ofh_transmitter_configuration.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul transmitter implementation dependencies.
struct transmitter_impl_dependencies {
  /// Log.
  srslog::basic_logger* logger = nullptr;
  /// Error notifier.
  error_notifier* err_notifier = nullptr;
  /// Transmitter task executor.
  task_executor* executor = nullptr;
  /// Data flow for downlink Control-Plane.
  std::unique_ptr<data_flow_cplane_scheduling_commands> dl_df_cplane;
  /// Data flow for downlink User-Plane.
  std::unique_ptr<data_flow_uplane_downlink_data> dl_df_uplane;
  /// Data flow for uplink Control-Plane scheduling commands.
  std::unique_ptr<data_flow_cplane_scheduling_commands> ul_df_cplane;
  /// Uplink slot context repository.
  std::shared_ptr<uplink_context_repository> ul_slot_repo;
  /// Uplink PRACH context repository.
  std::shared_ptr<prach_context_repository> ul_prach_repo;
  /// Notified uplink grid symbol repository.
  std::shared_ptr<uplink_notified_grid_symbol_repository> notifier_symbol_repo;
  /// Ethernet transmitter.
  std::unique_ptr<ether::transmitter> eth_transmitter;
  /// Ethernet frame pool.
  std::shared_ptr<ether::eth_frame_pool> frame_pool;
};

class transmitter_impl : public transmitter
{
public:
  transmitter_impl(const transmitter_config& config, transmitter_impl_dependencies&& dependencies);

  // See interface for documentation.
  uplink_request_handler& get_uplink_request_handler() override;

  // See interface for documentation.
  downlink_handler& get_downlink_handler() override;

  // See interface for documentation.
  ota_symbol_boundary_notifier& get_ota_symbol_boundary_notifier() override;

  transmitter_metrics_collector* get_metrics_collector() override;

private:
  downlink_handler_impl                  dl_handler;
  uplink_request_handler_impl            ul_request_handler;
  message_transmitter_impl               msg_transmitter;
  transmitter_ota_symbol_task_dispatcher ota_dispatcher;
  transmitter_metrics_collector_impl     metrics_collector;
};

} // namespace ofh
} // namespace srsran
