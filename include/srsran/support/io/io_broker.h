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

#include "srsran/support/executors/unique_thread.h"
#include <mutex>
#include <utility>

namespace srsran {

struct io_broker_config {
  std::string                 thread_name = "io_broker_epoll";
  os_thread_realtime_priority thread_prio = os_thread_realtime_priority::no_realtime();
  os_sched_affinity_bitmask   cpu_mask    = {};
  /// Constructor receives CPU affinity mask.
  io_broker_config(os_sched_affinity_bitmask mask_ = {}) : cpu_mask(mask_) {}
};

/// \brief Describes the interface for an (async) IO broker.
/// The IO broker is responsible for handling all IO events, including sockets, signals, and system timers.
/// File descriptors can be registered with the IO broker via register_fd(), which will return an RAII subscriber
/// object that handles the deregistration of the file descriptor when it goes out of scope.
class io_broker
{
public:
  /// \brief Handle to a file descriptor + callbacks that were registered in the io_broker.
  /// On destruction, the file descriptor and associated callback are automatically deregistered from the io_broker.
  class subscriber
  {
  public:
    subscriber() = default;
    subscriber(io_broker& broker_, int fd_) : broker(&broker_), fd(fd_) {}
    subscriber(subscriber&& other) noexcept
    {
      std::lock_guard<std::mutex> lock(other.mutex);
      broker = other.broker;
      fd     = std::exchange(other.fd, -1);
    }
    subscriber& operator=(subscriber&& other) noexcept
    {
      std::scoped_lock lock(mutex, other.mutex);
      reset_nolock();
      broker = other.broker;
      fd     = std::exchange(other.fd, -1);
      return *this;
    }
    ~subscriber() { reset(); }

    /// Checks whether the FD is connected to the broker.
    bool registered() const
    {
      std::lock_guard<std::mutex> lock(mutex);
      return fd >= 0;
    }

    /// Resets the handle, deregistering the FD from the broker.
    bool reset()
    {
      std::lock_guard<std::mutex> lock(mutex);
      return reset_nolock();
    }

  private:
    bool reset_nolock()
    {
      int fd_tmp = std::exchange(fd, -1);
      if (fd_tmp >= 0) {
        return broker->unregister_fd(fd_tmp);
      }
      return false;
    }

    // Mutex unique to the subscriber object.
    mutable std::mutex mutex;
    io_broker*         broker = nullptr;
    int                fd     = -1;
  };

  /// Callback called when registered fd has data
  using recv_callback_t = std::function<void()>;

  /// Callback called when the fd detected an error. After an error is detected, the broker stops listening to the fd.
  enum class error_code { hang_up, error, other };
  using error_callback_t = std::function<void(error_code)>;

  virtual ~io_broker() = default;

  /// \brief Register a file descriptor to be handled by the IO interface.
  /// \param[in] fd File descriptor to be registered.
  /// \param[in] handler Callback that handles receive events.
  /// \param[in] err_handler Callback that handles error events.
  /// \return An RAII handle to the registered file descriptor. On destruction, the fd is automatically deregistered
  /// from the io_broker.
  SRSRAN_NODISCARD virtual subscriber register_fd(
      int              fd,
      recv_callback_t  handler,
      error_callback_t err_handler = [](error_code) {}) = 0;

private:
  /// \brief Unregister a file descriptor from the IO interface.
  /// \param[in] fd File descriptor to be unregistered.
  /// \return true if the file descriptor was successfully unregistered, false otherwise.
  SRSRAN_NODISCARD virtual bool unregister_fd(int fd) = 0;
};

} // namespace srsran
