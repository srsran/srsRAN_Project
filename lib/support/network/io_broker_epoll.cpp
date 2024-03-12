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

#include "io_broker_epoll.h"
#include <sys/epoll.h>
#include <unistd.h>

using namespace srsran;

io_broker_epoll::io_broker_epoll(const io_broker_config& config) : logger(srslog::fetch_basic_logger("IO-EPOLL"))
{
  // Init epoll socket
  epoll_fd = ::epoll_create1(0);
  if (epoll_fd == -1) {
    report_fatal_error("IO broker: failed to create epoll file descriptor. error={}", strerror(errno));
  }

  // register event to handle stop
  event_fd = ::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
  if (event_fd == -1) {
    report_fatal_error("IO broker: failed to create file descriptor for event notification. error={}", strerror(errno));
  }
  bool success = register_fd(event_fd, [](int fd) {
    // do nothing
  });
  if (!success) {
    report_fatal_error("IO broker: failed to register event file descriptor. event_fd={}", event_fd);
  }

  logger.debug("Starting event loop");
  // start thread to handle epoll events
  thread = unique_thread(config.thread_name, config.thread_prio, config.cpu_mask, [this]() {
    while (running) {
      thread_loop();
    }
  });
}

io_broker_epoll::~io_broker_epoll()
{
  if (running) {
    // inform thread to stop
    running = false;

    // send stop event to interrupt possible epoll_wait()
    uint64_t tmp = 1;
    ssize_t  ret = ::write(event_fd, &tmp, sizeof(tmp));
    if (ret == -1) {
      logger.error("Failed to write stop event. event_fd={} error={}", event_fd, strerror(errno));
    }
  }

  logger.debug("Joining event loop");
  // wait for completion
  if (thread.running()) {
    thread.join();
  }

  logger.debug("Closing epoll file descriptor. epoll_fd={}", epoll_fd);
  std::lock_guard<std::mutex> lock(event_handler_mutex);
  event_handler.clear();
  close(epoll_fd);
}

/// Function is executed in a loop until the thread is notify_stop.
void io_broker_epoll::thread_loop()
{
  // wait for event
  const int32_t      epoll_timeout_ms   = -1;
  const uint32_t     MAX_EVENTS         = 1;
  struct epoll_event events[MAX_EVENTS] = {};
  int                nof_events         = ::epoll_wait(epoll_fd, events, MAX_EVENTS, epoll_timeout_ms);

  // handle event
  if (nof_events == -1) {
    logger.error("epoll_wait failed. error={}", strerror(errno));
    /// TODO: shall we raise a fatal error here?
    return;
  }
  if (nof_events == 0) {
    logger.error("epoll_wait time out. timeout_ms={}", epoll_timeout_ms);
    return;
  }

  logger.debug("Handling events. nof_events={}", nof_events);
  for (int i = 0; i < nof_events; ++i) {
    int fd = events[i].data.fd;
    if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN))) {
      // An error or hang up happend on this file descriptor, or the socket is not ready for reading
      // TODO: add notifier for these events and let the subscriber decide on further actions (e.g. unregister fd)
      if (events[i].events & EPOLLHUP) {
        // Note: some container environments hang up stdin (fd=0) in case of non-interactive sessions
        logger.warning("Hang up on file descriptor. fd={} events={:#x}", fd, uint32_t(events[i].events));
      } else if (events[i].events & EPOLLERR) {
        logger.error("Error on file descriptor. fd={} events={:#x}", fd, uint32_t(events[i].events));
      } else {
        logger.error("Unhandled epoll event. fd={} events={:#x}", fd, uint32_t(events[i].events));
      }
      // Unregister the faulty file descriptor from epoll
      bool success = unregister_fd(fd);
      if (!success) {
        logger.error("Failed to unregister file descriptor. fd={}", fd);
      }
      return;
    }

    std::lock_guard<std::mutex> lock(event_handler_mutex);
    const auto&                 it = event_handler.find(fd);
    if (it != event_handler.end()) {
      logger.debug("Handling event. fd={}", fd);
      it->second->handle_event(fd, events[i]);
    } else {
      logger.error("Could not find event handler. fd={}", fd);
    }
  }
}

/// Adds a new file descriptor to the epoll-handler. The call is thread-safe and new
/// file descriptors can be added while the epoll_wait() is blocking.
bool io_broker_epoll::register_fd(int fd, recv_callback_t handler)
{
  logger.debug("Registering file descriptor. fd={}", fd);

  {
    // add event handler to map before registering it at epoll control
    std::lock_guard<std::mutex> lock(event_handler_mutex);
    event_handler.insert({fd, std::make_unique<epoll_receive_callback>(handler)});
  }

  struct epoll_event ev = {};
  ev.data.fd            = fd;
  ev.events             = EPOLLIN;
  if (::epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
    logger.error("Failed to register file descriptor. fd={} error={}", fd, strerror(errno));
    // remove event handler from map
    std::lock_guard<std::mutex> lock(event_handler_mutex);
    event_handler.erase(fd);
    return false;
  }

  return true;
}

/// \brief Remove fd from epoll handler.
bool io_broker_epoll::unregister_fd(int fd)
{
  struct epoll_event ev = {};
  ev.data.fd            = fd;
  ev.events             = EPOLLIN;
  if (::epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, &ev) == -1) {
    logger.error("Failed to unregister file descriptor. fd={} error={}", fd, strerror(errno));
    return false;
  }

  std::lock_guard<std::mutex> lock(event_handler_mutex);
  event_handler.erase(fd);

  logger.debug("Unregistered file descriptor. fd={}", fd);
  return true;
}
