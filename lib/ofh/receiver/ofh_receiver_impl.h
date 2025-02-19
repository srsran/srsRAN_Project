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

#include "../support/uplink_context_repository.h"
#include "ofh_closed_rx_window_handler.h"
#include "ofh_message_receiver.h"
#include "ofh_message_receiver_task_dispatcher.h"
#include "ofh_receiver_controller.h"
#include "ofh_rx_window_checker.h"
#include "srsran/ofh/receiver/ofh_receiver.h"
#include "srsran/ofh/receiver/ofh_receiver_configuration.h"

namespace srsran {

class task_executor;

namespace ofh {

/// Open Fronthaul receiver implementation dependencies.
struct receiver_impl_dependencies {
  /// Message receiver dependencies.
  struct message_rx_dependencies {
    /// Logger.
    srslog::basic_logger* logger = nullptr;
    /// Ethernet receiver.
    std::unique_ptr<ether::receiver> eth_receiver;
    /// eCPRI packet decoder.
    std::unique_ptr<ecpri::packet_decoder> ecpri_decoder;
    /// Ethernet frame decoder.
    std::unique_ptr<ether::vlan_frame_decoder> eth_frame_decoder;
    /// User-Plane uplink data flow.
    std::unique_ptr<data_flow_uplane_uplink_data> data_flow_uplink;
    /// User-Plane uplink PRACH data flow.
    std::unique_ptr<data_flow_uplane_uplink_prach> data_flow_prach;
    /// Sequence id checker.
    std::unique_ptr<sequence_id_checker> seq_id_checker;
  };

  struct close_rx_window_dependencies {
    std::shared_ptr<prach_context_repository>  prach_repo;
    std::shared_ptr<uplink_context_repository> uplink_repo;
    std::shared_ptr<uplane_rx_symbol_notifier> notifier;
  };

  /// Logger.
  srslog::basic_logger* logger = nullptr;
  /// Task executor.
  task_executor* executor = nullptr;
  /// Message receiver dependencies.
  message_rx_dependencies msg_rx_dependencies;
  /// Closed reception window handler dependencies.
  close_rx_window_dependencies window_handler_dependencies;
};

/// OTA symbol boundary dispatcher for the receiver.
class ota_symbol_boundary_dispatcher : public ota_symbol_boundary_notifier
{
  std::vector<ota_symbol_boundary_notifier*> handlers;

public:
  ota_symbol_boundary_dispatcher(std::vector<ota_symbol_boundary_notifier*> handlers_) : handlers(std::move(handlers_))
  {
  }

  // See interface for documentation
  void on_new_symbol(const slot_symbol_point_context& symbol_point_context) override;
};

/// \brief Open Fronthaul receiver.
///
/// The Open Fronthaul receiver that groups and owns all the uplink related classes.
class receiver_impl : public receiver
{
public:
  receiver_impl(const receiver_config& config, receiver_impl_dependencies&& dependencies);

  // See interface for documentation.
  ota_symbol_boundary_notifier* get_ota_symbol_boundary_notifier() override;

  // See interface for documentation.
  controller& get_controller() override;

  // See interface for documentation.
  receiver_metrics_collector* get_metrics_collector() override;

private:
  closed_rx_window_handler             closed_window_handler;
  rx_window_checker                    window_checker;
  ota_symbol_boundary_dispatcher       symbol_boundary_dispatcher;
  message_receiver_impl                msg_receiver;
  ofh_message_receiver_task_dispatcher rcv_task_dispatcher;
  receiver_controller                  ctrl;
};

} // namespace ofh
} // namespace srsran
