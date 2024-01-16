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

#include "srsran/support/io/io_broker.h"
#include <atomic>
#include <functional>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/signalfd.h>
#include <sys/timerfd.h>
#include <unistd.h>

namespace srsran {

///< A virtual interface to handle epoll events (used by timer and socket handler)
class epoll_handler
{
public:
  virtual int handle_event(int fd, epoll_event e) = 0;
  virtual ~epoll_handler()                        = default;
};

///< Callback function called when data is received on socket
class epoll_receive_callback : public epoll_handler
{
public:
  epoll_receive_callback(io_broker::recv_callback_t callback_) : callback(callback_) {}
  int handle_event(int fd, epoll_event e) override
  {
    // TODO: translate epoll events to io_broker_base::event_t
    if (e.events & EPOLLIN) {
      // just handle the normal receive event
      callback(fd);
    }
    return 0;
  }

private:
  io_broker::recv_callback_t callback;
};

///< Callback function called when timer expires
using epoll_timer_callback = std::function<void(uint64_t res)>;

///< Epoll timer handler
class epoll_timer_handler : public epoll_handler
{
public:
  epoll_timer_handler(int fd_, epoll_timer_callback callback_) : timer_fd(fd_), callback(callback_){};
  int handle_event(int fd, epoll_event e) override
  {
    uint64_t res;
    int      ret = read(fd, &res, sizeof(res));
    callback(res);
    return ret;
  }

  int get_timer_fd() { return timer_fd; };

private:
  int                  timer_fd = -1;
  epoll_timer_callback callback;
};

///< Basic epoll signal handler
class epoll_signal_handler : public epoll_handler
{
public:
  epoll_signal_handler(std::atomic<bool>& running_) : running(running_) {}

  int handle_event(int fd, epoll_event e) override
  {
    struct signalfd_siginfo info;
    if (read(fd, &info, sizeof(info)) != sizeof(info)) {
      // fprintf(stderr, "failed to read signal fd buffer\n");
      return -1;
    }
    switch (info.ssi_signo) {
      case SIGTERM:
      case SIGINT:
      case SIGHUP:
      case SIGQUIT:
        running = false;
        break;
      default:
        std::printf("got unknown signal\n");
        break;
    }
    return 0;
  }

private:
  std::atomic<bool>& running; /// TODO: we might want to use a different mechanism to inform the main thread
};

} // namespace srsran
