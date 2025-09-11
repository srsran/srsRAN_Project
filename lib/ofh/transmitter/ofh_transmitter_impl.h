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

#pragma once

#include "ofh_downlink_handler_impl.h"
#include "ofh_message_transmitter_impl.h"
#include "ofh_transmitter_metrics_collector_impl.h"
#include "ofh_transmitter_ota_symbol_task_dispatcher.h"
#include "ofh_uplink_request_handler_impl.h"
#include "ofh_uplink_request_handler_task_dispatcher.h"
#include "srsran/ofh/ofh_controller.h"
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
  /// Downlink task executor.
  task_executor* dl_executor = nullptr;
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
  /// Ethernet frame pool downlink Control-Plane.
  std::shared_ptr<ether::eth_frame_pool> frame_pool_dl_cp;
  /// Ethernet frame pool uplink Control-Plane.
  std::shared_ptr<ether::eth_frame_pool> frame_pool_ul_cp;
  /// Ethernet frame pool downlink User-Plane.
  std::shared_ptr<ether::eth_frame_pool> frame_pool_dl_up;
};

class transmitter_impl : public transmitter, public operation_controller
{
public:
  transmitter_impl(const transmitter_config& config, transmitter_impl_dependencies&& dependencies);

  ~transmitter_impl() override { stop(); }

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

  // See interface for documentation.
  operation_controller& get_operation_controller() override { return *this; }

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
  uplink_request_handler_task_dispatcher ul_task_dispatcher;
  message_transmitter_impl               msg_transmitter;
  transmitter_ota_symbol_task_dispatcher ota_dispatcher;
  transmitter_metrics_collector_impl     metrics_collector;
};

} // namespace ofh
} // namespace srsran
