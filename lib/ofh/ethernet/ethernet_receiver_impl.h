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

#include "ethernet_rx_buffer_pool.h"
#include "srsran/ofh/ethernet/ethernet_receiver.h"
#include "srsran/srslog/logger.h"

namespace srsran {

class task_executor;

namespace ether {

/// Implementation for the Ethernet receiver.
class receiver_impl : public receiver
{
  static constexpr unsigned BUFFER_SIZE = 9600;

  enum class receiver_status { running, stop_requested, stopped };

public:
  receiver_impl(const std::string&    interface,
                bool                  is_promiscuous_mode_enabled,
                task_executor&        executor_,
                srslog::basic_logger& logger_);
  ~receiver_impl() override;

  // See interface for documentation.
  void start(frame_notifier& notifier_) override;

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
  srslog::basic_logger&                  logger;
  task_executor&                         executor;
  std::reference_wrapper<frame_notifier> notifier;
  int                                    socket_fd = -1;
  std::atomic<receiver_status>           rx_status{receiver_status::running};
  ethernet_rx_buffer_pool                buffer_pool;
};

} // namespace ether
} // namespace srsran
