/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ofh/ethernet/ethernet_receiver.h"
#include "srsran/srslog/logger.h"

namespace srsran {

class task_executor;

namespace ether {

class frame_notifier;

/// DPDK Ethernet receiver implementation.
class dpdk_receiver_impl : public receiver
{
public:
  dpdk_receiver_impl(task_executor& executor_, frame_notifier& notifier_, srslog::basic_logger& logger_) :
    logger(logger_), executor(executor_), notifier(notifier_)
  {
  }

  ~dpdk_receiver_impl() override;

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

private:
  /// Main receiving loop.
  void receive_loop();

  /// Receives new Ethernet frames from the socket.
  ///
  /// \note This function will block until new frames become available.
  void receive();

private:
  srslog::basic_logger& logger;
  task_executor&        executor;
  frame_notifier&       notifier;
  const unsigned        port_id = 0;
  std::atomic<bool>     is_stop_requested{false};
};

} // namespace ether
} // namespace srsran
