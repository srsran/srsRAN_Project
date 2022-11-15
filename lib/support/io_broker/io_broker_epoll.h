/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "epoll_helper.h"
#include "srsgnb/srslog/srslog.h"
#include "srsgnb/support/io_broker/io_broker.h"
#include <mutex>
#include <unordered_map>

namespace srsgnb {

struct io_broker_config {
  std::string                 thread_name = "io_broker_epoll";
  os_thread_realtime_priority thread_prio = os_thread_realtime_priority::MIN_PRIO;
};

/// @brief Implementation of an IO broker using epoll.
class io_broker_epoll : public io_broker
{
public:
  explicit io_broker_epoll(io_broker_config config);
  ~io_broker_epoll();

  bool register_fd(int fd, recv_callback_t handler) override;
  bool unregister_fd(int fd) override;

private:
  void thread_loop();

  srslog::basic_logger& logger;

  // epoll related variables
  int epoll_fd = -1; ///< Main epoll file descriptor
  int event_fd = -1; ///< Event file descriptor used to interrupt epoll_wait
  std::unordered_map<uint32_t, std::unique_ptr<epoll_handler>> event_handler;       ///< Lookup table for handler
  std::mutex                                                   event_handler_mutex; ///< Mutex to protect event_handler

  std::atomic<bool> running{true};
  unique_thread     thread;
};

} // namespace srsgnb
