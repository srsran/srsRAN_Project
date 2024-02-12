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

#include "dpdk_ethernet_port_context.h"
#include "srsran/ofh/ethernet/ethernet_receiver.h"
#include "srsran/srslog/logger.h"
#include "srsran/support/srsran_assert.h"

namespace srsran {

class task_executor;

namespace ether {

class frame_notifier;

/// DPDK Ethernet receiver implementation.
class dpdk_receiver_impl : public receiver
{
  enum class receiver_status { idle, running, stop_requested, stopped };

public:
  dpdk_receiver_impl(task_executor&                     executor_,
                     std::shared_ptr<dpdk_port_context> port_ctx_ptr_,
                     srslog::basic_logger&              logger_);

  // See interface for documentation.
  void start(frame_notifier& notifier) override;

  // See interface for documentation.
  void stop() override;

private:
  /// Main receiving loop.
  void receive_loop();

  /// Receives new Ethernet frames from the socket.
  void receive();

private:
  srslog::basic_logger&                  logger;
  task_executor&                         executor;
  std::reference_wrapper<frame_notifier> notifier;
  std::shared_ptr<dpdk_port_context>     port_ctx_ptr;
  dpdk_port_context&                     port_ctx;
  std::atomic<receiver_status>           rx_status{receiver_status::idle};
};

} // namespace ether
} // namespace srsran
