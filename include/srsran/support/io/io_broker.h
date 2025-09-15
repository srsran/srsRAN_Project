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

#include "srsran/support/executors/task_executor.h"
#include "srsran/support/executors/unique_thread.h"
#include "srsran/support/io/unique_fd.h"
#include <atomic>
#include <future>
#include <utility>

namespace srsran {

struct io_broker_config {
  io_broker_config() = default;
  explicit io_broker_config(os_thread_realtime_priority prio, os_sched_affinity_bitmask mask_ = {}) :
    thread_prio(prio), cpu_mask(std::move(mask_))
  {
  }

  std::string                 thread_name = "io_broker_epoll";
  os_thread_realtime_priority thread_prio = os_thread_realtime_priority::no_realtime();
  os_sched_affinity_bitmask   cpu_mask;
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
      fd     = other.fd.exchange(-1);
      broker = other.broker;
    }

    subscriber& operator=(subscriber&& other) noexcept
    {
      reset_impl();
      fd     = other.fd.exchange(-1);
      broker = other.broker;
      return *this;
    }

    ~subscriber() { reset(); }

    /// Checks whether the FD is connected to the broker.
    bool registered() const { return fd >= 0; }

    /// Resets the handle, deregistering the FD from the broker.
    /// \note: This function will block until the FD has been removed from the broker.
    bool reset()
    {
      std::promise<bool> p;
      std::future<bool>  fut = p.get_future();

      bool ret = reset_impl(&p);
      fut.get();

      return ret;
    }

  private:
    bool reset_impl(std::promise<bool>* complete_notifier = nullptr)
    {
      int fd_tmp = fd.exchange(-1);
      if (fd_tmp >= 0) {
        return broker->unregister_fd(fd_tmp, complete_notifier);
      }
      if (complete_notifier) {
        complete_notifier->set_value(false);
      }
      return false;
    }

    io_broker*       broker = nullptr;
    std::atomic<int> fd{-1};
  };

  /// Callback called when registered fd has data
  using recv_callback_t = std::function<void()>;

  /// Callback called when the fd detected an error. After an error is detected, the broker stops listening to the fd.
  enum class error_code { hang_up, error, other };
  using error_callback_t = std::function<void(error_code)>;

  virtual ~io_broker() = default;

  /// \brief Register a file descriptor to be handled by the IO interface.
  /// \param[in] fd File descriptor to be registered.
  /// \param[in] executor Execution context where the given callbacks will be called.
  /// \param[in] handler Callback that handles receive events.
  /// \param[in] err_handler Callback that handles error events.
  /// \return An RAII handle to the registered file descriptor. On destruction, the fd is automatically deregistered
  /// from the io_broker.
  [[nodiscard]] virtual subscriber register_fd(
      unique_fd        fd,
      task_executor&   executor,
      recv_callback_t  handler,
      error_callback_t err_handler = [](error_code) {}) = 0;

private:
  /// \brief Unregister a file descriptor from the IO interface.
  /// \param[in] fd                File descriptor to be unregistered.
  /// \param[in] complete_notifier Synchronization primitive to wait for the deregistration of the FD to be completed.
  /// \return true if the file descriptor was successfully unregistered, false otherwise.
  [[nodiscard]] virtual bool unregister_fd(int fd, std::promise<bool>* complete_notifier) = 0;
};

} // namespace srsran
