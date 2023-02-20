/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/support/unique_thread.h"

namespace srsgnb {

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
  virtual bool register_fd(int fd, recv_callback_t handler) = 0;

  /// \brief Unregister a file descriptor from the IO interface.
  virtual bool unregister_fd(int fd) = 0;
};

} // namespace srsgnb