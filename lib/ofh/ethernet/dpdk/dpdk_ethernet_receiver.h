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

#include "../ethernet_rx_metrics_collector_impl.h"
#include "srsran/ofh/ethernet/dpdk/dpdk_ethernet_port_context.h"
#include "srsran/ofh/ethernet/ethernet_controller.h"
#include "srsran/ofh/ethernet/ethernet_receiver.h"
#include "srsran/srslog/logger.h"
#include "srsran/support/srsran_assert.h"

namespace srsran {

class task_executor;

namespace ether {

class frame_notifier;

/// DPDK Ethernet receiver implementation.
class dpdk_receiver_impl : public receiver, private receiver_operation_controller
{
  enum class receiver_status { running, stop_requested, stopped };

public:
  dpdk_receiver_impl(task_executor&                     executor_,
                     std::shared_ptr<dpdk_port_context> port_ctx_,
                     srslog::basic_logger&              logger_,
                     bool                               are_metrics_enabled);

  // See interface for documentation.
  receiver_operation_controller& get_operation_controller() override { return *this; }

  // See interface for documentation.
  receiver_metrics_collector* get_metrics_collector() override;

private:
  // See interface for documentation.
  void start(frame_notifier& notifier) override;

  // See interface for documentation.
  void stop() override;

  /// Main receiving loop.
  void receive_loop();

  /// Receives new Ethernet frames from the socket.
  void receive();

  srslog::basic_logger&              logger;
  task_executor&                     executor;
  frame_notifier*                    notifier;
  std::shared_ptr<dpdk_port_context> port_ctx;
  receiver_metrics_collector_impl    metrics_collector;
  std::atomic<receiver_status>       rx_status{receiver_status::running};
};

} // namespace ether
} // namespace srsran
