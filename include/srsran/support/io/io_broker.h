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

#include "srsran/support/unique_thread.h"

namespace srsran {

struct io_broker_config {
  std::string                 thread_name = "io_broker_epoll";
  os_thread_realtime_priority thread_prio = os_thread_realtime_priority::no_realtime();
  os_sched_affinity_bitmask   cpu_mask    = {};
  /// Constructor receives CPU affinity mask.
  io_broker_config(os_sched_affinity_bitmask mask_ = {}) : cpu_mask(mask_) {}
};

/// \brief Describes the base interface for an (async) IO broker.
/// The IO broker is responsible for handling all IO events, including
/// sockets, signals, and system timers.
/// File descriptors can be registered with the IO broker via register_fd()
/// and unregistered via unregister_fd().
class io_broker
{
protected:
  /// Allow default constructor for child.
  io_broker() = default;

public:
  /// Callback called when socket fd (passed as argument) has data
  using recv_callback_t = std::function<void(int)>;

  /// Provides default destructor.
  virtual ~io_broker() = default;

  /// Forbid copy constructor.
  io_broker(const io_broker& other) = delete;

  /// Forbid move constructor.
  io_broker(const io_broker&& other) = delete;

  /// Forbid copy assigment operator.
  io_broker& operator=(const io_broker&) = delete;

  /// Forbid move assigment operator.
  io_broker& operator=(io_broker&&) = delete;

  /// \brief Register a file descriptor to be handled by the IO interface.
  SRSRAN_NODISCARD virtual bool register_fd(int fd, recv_callback_t handler) = 0;

  /// \brief Unregister a file descriptor from the IO interface.
  SRSRAN_NODISCARD virtual bool unregister_fd(int fd) = 0;
};

} // namespace srsran
