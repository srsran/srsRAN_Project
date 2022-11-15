/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "io_broker_epoll.h"
#include <sys/epoll.h>
#include <unistd.h>

using namespace srsgnb;

io_broker_epoll::io_broker_epoll(io_broker_config config) : logger(srslog::fetch_basic_logger("IO-BRK_EPOLL"))
{
  // Init epoll socket
  epoll_fd = ::epoll_create1(0);
  if (epoll_fd == -1) {
    report_fatal_error("Failed to create epoll socket");
  }

  // register event to handle stop
  event_fd = ::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
  register_fd(event_fd, [](int fd) {
    // do nothing
  });

  // start thread to handle epoll events
  thread = unique_thread(config.thread_name, config.thread_prio, [this]() {
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
      logger.error("Error writing to event_fd");
    }
  }

  // wait for completion
  if (thread.running()) {
    thread.join();
  }

  std::lock_guard<std::mutex> lock(event_handler_mutex);
  event_handler.clear();
  close(epoll_fd);
}

/// Function is executed in a loop until the thread is stopped.
void io_broker_epoll::thread_loop()
{
  // wait for event
  const int32_t      epoll_timeout_ms   = -1;
  const uint32_t     MAX_EVENTS         = 1;
  struct epoll_event events[MAX_EVENTS] = {};
  int                nof_events         = ::epoll_wait(epoll_fd, events, MAX_EVENTS, epoll_timeout_ms);

  // handle event
  if (nof_events == -1) {
    logger.error("epoll_wait(): {}", strerror(errno));
    /// TODO: shall we raise a fatal error here?
    return;
  }
  if (nof_events == 0) {
    logger.error("time out {} sec expired", epoll_timeout_ms / 1000.0);
    return;
  }

  for (int i = 0; i < nof_events; ++i) {
    if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN))) {
      ///< An error has occured on this fd, or the socket is not ready for reading
      /// TODO: add notifier for events
      logger.error("epoll error\n");
      close(events[i].data.fd);
      unregister_fd(events[i].data.fd);
      return;
    }

    int                         fd = events[i].data.fd;
    std::lock_guard<std::mutex> lock(event_handler_mutex);
    const auto&                 it = event_handler.find(fd);
    if (it != event_handler.end()) {
      it->second->handle_event(fd, events[i]);
    }
  }
}

/// Adds a new file descriptor to the epoll-handler. The call is thread-safe and new
/// file descriptors can be added while the epoll_wait() is blocking.
bool io_broker_epoll::register_fd(int fd, recv_callback_t handler)
{
  struct epoll_event ev = {};
  ev.data.fd            = fd;
  ev.events             = EPOLLIN;
  if (::epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
    logger.error("epoll_ctl failed for fd={}", fd);
    return false;
  }

  std::lock_guard<std::mutex> lock(event_handler_mutex);
  event_handler.insert({fd, std::make_unique<epoll_receive_callback>(handler)});

  return true;
}

/// \brief Remove fd from epoll handler.
bool io_broker_epoll::unregister_fd(int fd)
{
  struct epoll_event ev = {};
  ev.data.fd            = fd;
  ev.events             = EPOLLIN;
  if (::epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, &ev) == -1) {
    logger.error("epoll_ctl failed for fd={}", fd);
    return false;
  }

  std::lock_guard<std::mutex> lock(event_handler_mutex);
  event_handler.erase(fd);

  return true;
}
