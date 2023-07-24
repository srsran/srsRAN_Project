/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "epoll_helper.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/io/io_broker.h"
#include <mutex>
#include <unordered_map>

namespace srsran {

struct io_broker_config {
  std::string                 thread_name = "io_broker_epoll";
  os_thread_realtime_priority thread_prio = os_thread_realtime_priority::no_realtime();
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

} // namespace srsran
