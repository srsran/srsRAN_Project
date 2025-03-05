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

#include "ofh_downlink_manager.h"
#include "ofh_message_transmitter_impl.h"
#include "ofh_transmitter_ota_symbol_task_dispatcher.h"
#include "ofh_uplink_request_handler_impl.h"
#include "sequence_identifier_generator.h"
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
  /// Data flow for uplink Control-Plane scheduling commands.
  std::unique_ptr<data_flow_cplane_scheduling_commands> data_flow;
  /// Uplink slot context repository.
  std::shared_ptr<uplink_context_repository> ul_slot_repo;
  /// Uplink PRACH context repository.
  std::shared_ptr<prach_context_repository> ul_prach_repo;
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
  std::unique_ptr<downlink_manager>      dl_manager;
  uplink_request_handler_impl            ul_request_handler;
  message_transmitter_impl               msg_transmitter;
  transmitter_ota_symbol_task_dispatcher ota_dispatcher;
};

} // namespace ofh
} // namespace srsran
