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

#include "ethernet_rx_buffer_pool.h"
#include "ethernet_rx_metrics_collector_impl.h"
#include "srsran/ofh/ethernet/ethernet_controller.h"
#include "srsran/ofh/ethernet/ethernet_receiver.h"
#include "srsran/ofh/ethernet/ethernet_receiver_config.h"
#include "srsran/srslog/logger.h"
#include "srsran/support/synchronization/stop_event.h"

namespace srsran {

class task_executor;

namespace ether {

/// Implementation for the Ethernet receiver.
class receiver_impl : public receiver, private receiver_operation_controller
{
  static constexpr unsigned BUFFER_SIZE = 9600;

public:
  receiver_impl(const receiver_config& config, task_executor& executor_, srslog::basic_logger& logger_);

  ~receiver_impl() override;

  // See interface for documentation.
  receiver_operation_controller& get_operation_controller() override { return *this; }

  // See interface for documentation.
  receiver_metrics_collector* get_metrics_collector() override;

private:
  // See interface for documentation.
  void start(frame_notifier& notifier_) override;

  // See interface for documentation.
  void stop() override;

  /// Main receiving loop.
  void receive_loop();

  /// Receives new Ethernet frames from the socket.
  ///
  /// \note This function will block until new frames become available.
  void receive();

private:
  srslog::basic_logger&           logger;
  task_executor&                  executor;
  frame_notifier*                 notifier;
  int                             socket_fd = -1;
  stop_event_source               stop_manager;
  ethernet_rx_buffer_pool         buffer_pool;
  receiver_metrics_collector_impl metrics_collector;
};

} // namespace ether
} // namespace srsran
