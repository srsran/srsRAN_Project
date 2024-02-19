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

#pragma once

#include "ofh_data_flow_cplane_scheduling_commands_impl.h"
#include "ofh_data_flow_uplane_downlink_data_impl.h"
#include "ofh_downlink_handler_impl.h"
#include "ofh_downlink_manager.h"
#include "ofh_message_transmitter_impl.h"
#include "ofh_transmitter_ota_symbol_task_dispatcher.h"
#include "ofh_uplink_request_handler_impl.h"
#include "sequence_identifier_generator.h"
#include "srsran/ofh/compression/iq_compressor.h"
#include "srsran/ofh/transmitter/ofh_transmitter.h"
#include "srsran/ofh/transmitter/ofh_transmitter_configuration.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul transmitter implementation dependencies.
struct transmitter_impl_dependencies {
  /// Log.
  srslog::basic_logger* logger = nullptr;
  /// Transmitter task executor.
  task_executor* executor = nullptr;
  /// Downlink manager.
  std::unique_ptr<downlink_manager> dl_manager;
  /// Uplink request handler.
  std::unique_ptr<uplink_request_handler> ul_request_handler;
  /// Ethernet gateway.
  std::unique_ptr<ether::gateway> eth_gateway;
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

  // See interface for documentation.
  void set_error_notifier(error_notifier& notifier) override;

private:
  std::unique_ptr<downlink_manager>       dl_manager;
  std::unique_ptr<uplink_request_handler> ul_request_handler;
  message_transmitter_impl                msg_transmitter;
  transmitter_ota_symbol_task_dispatcher  ota_dispatcher;
};

} // namespace ofh
} // namespace srsran
