/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

  // Register fd and event_handler to handle stops, fd registrations and fd deregistrations.
  ctrl_event_fd = ::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
  if (not handle_fd_registration(
          ctrl_event_fd, [this](int fd) { handle_enqueued_events(); }, nullptr)) {
    report_fatal_error("IO broker: failed to register control event file descriptor. ctrl_event_fd={}", ctrl_event_fd);
  }

  // start thread to handle epoll events.
  std::promise<void> p;
  std::future<void>  fut = p.get_future();
  thread                 = unique_thread(config.thread_name, config.thread_prio, [this, &p]() {
    running = true;
    p.set_value();
    thread_loop();
  });

  // Wait for the thread to start before returning.
  fut.wait();
}

io_broker_epoll::~io_broker_epoll()
{
  // Wait for completion.
  if (thread.running()) {
    enqueue_event(control_event{control_event::event_type::close_io_broker, 0, {}, nullptr});
    thread.join();
  }

  // Close epoll control event fd.
  if (ctrl_event_fd >= 0) {
    close(ctrl_event_fd);
  }

  // Close epoll socket.
  if (epoll_fd >= 0) {
    close(epoll_fd);
  }
}

/// Function is executed in a loop until the thread is notify_stop.
void io_broker_epoll::thread_loop()
{
  while (running) {
    // wait for event
    const int32_t      epoll_timeout_ms   = -1;
    const uint32_t     MAX_EVENTS         = 1;
    struct epoll_event events[MAX_EVENTS] = {};
    int                nof_events         = ::epoll_wait(epoll_fd, events, MAX_EVENTS, epoll_timeout_ms);

    // handle event
    if (nof_events == -1) {
      logger.error("epoll_wait(): {}", strerror(errno));
      /// TODO: shall we raise a fatal error here?
      continue;
    }
    if (nof_events == 0) {
      logger.error("epoll time out {} sec expired", epoll_timeout_ms / 1000.0);
      continue;
    }

    for (int i = 0; i < nof_events; ++i) {
      int fd = events[i].data.fd;
      if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN))) {
        // An error or hang up happened on this file descriptor, or the socket is not ready for reading
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
        break;
      }

      const auto& it = event_handler.find(fd);
      if (it != event_handler.end()) {
        it->second->handle_event(fd, events[i]);
      } else {
        logger.error("Could not find event handler. fd={}", fd);
      }
    }
  }
}

bool io_broker_epoll::enqueue_event(const control_event& event)
{
  // Push of an event. It may malloc.
  event_queue.enqueue(event);

  // trigger epoll event to interrupt possible epoll_wait()
  uint64_t tmp = 1;
  ssize_t  ret = ::write(ctrl_event_fd, &tmp, sizeof(tmp));
  if (ret == -1) {
    logger.error("Error writing to CTRL event_fd");
  }
  return ret >= 0;
}

void io_broker_epoll::handle_enqueued_events()
{
  // Keep popping from the event queue.
  control_event ev;
  while (event_queue.try_dequeue(ev)) {
    // Handle event dequeued.
    switch (ev.type) {
      case control_event::event_type::register_fd:
        // Register new fd and event handler.
        handle_fd_registration(ev.fd, ev.handler, ev.completed);
        break;
      case control_event::event_type::deregister_fd:
        // Deregister fd and event handler.
        handle_fd_deregistration(ev.fd, ev.completed);
        break;
      case control_event::event_type::close_io_broker:
        // Close io broker.

        // Start by deregistering all existing file descriptors, except control event fd.
        for (const auto& it : event_handler) {
          if (it.first != ctrl_event_fd) {
            handle_fd_deregistration(it.first, nullptr);
          }
        }
        event_handler.clear();

        // Clear queue.
        while (event_queue.try_dequeue(ev)) {
          // Do nothing.
        }

        // Set flag to stop thread loop.
        running = false;
        return;

        break;
      default:
        report_fatal_error("Unknown event type {}", (int)ev.type);
    }
  }
}

bool io_broker_epoll::handle_fd_registration(int                    fd,
                                             const recv_callback_t& handler,
                                             std::promise<bool>*    complete_notifier)
{
  if (event_handler.count(fd) > 0) {
    logger.error("epoll_ctl failed for fd={}. Cause: fd already registered", fd);
    if (complete_notifier != nullptr) {
      complete_notifier->set_value(false);
    }
    return false;
  }

  // Add fd to epoll handler.
  struct epoll_event epoll_ev = {};
  epoll_ev.data.fd            = fd;
  epoll_ev.events             = EPOLLIN;
  if (::epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &epoll_ev) == -1) {
    logger.error("epoll_ctl failed for fd={}", fd);
    if (complete_notifier != nullptr) {
      complete_notifier->set_value(false);
    }
    return false;
  }

  // Register the handler of the fd.
  event_handler.insert({fd, std::make_unique<epoll_receive_callback>(handler)});
  if (complete_notifier != nullptr) {
    complete_notifier->set_value(true);
  }
  return true;
}

bool io_broker_epoll::handle_fd_deregistration(int fd, std::promise<bool>* complete_notifier)
{
  auto ev_it = event_handler.find(fd);
  if (ev_it == event_handler.end()) {
    logger.error("File descriptor not found. fd={}", fd);
    if (complete_notifier != nullptr) {
      complete_notifier->set_value(false);
    }
    return false;
  }

  struct epoll_event epoll_ev = {};
  epoll_ev.data.fd            = fd;
  epoll_ev.events             = EPOLLIN;
  if (::epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, &epoll_ev) == -1) {
    logger.error("epoll_ctl failed for fd={}", fd);
    if (complete_notifier != nullptr) {
      complete_notifier->set_value(false);
    }
    return false;
  }

  // Update lookup
  event_handler.erase(ev_it);

  if (complete_notifier != nullptr) {
    complete_notifier->set_value(true);
  }
  return true;
}

/// Adds a new file descriptor to the epoll-handler. The call is thread-safe and new
/// file descriptors can be added while the epoll_wait() is blocking.
bool io_broker_epoll::register_fd(int fd, recv_callback_t handler)
{
  if (fd < 0) {
    logger.error("io_broker_epoll::register_fd: Received an invalid fd={}", fd);
    return false;
  }
  if (not running.load(std::memory_order_relaxed)) {
    logger.warning("io_broker_epoll::register_fd: io_broker is not running. fd={}", fd);
    return false;
  }

  if (std::this_thread::get_id() == thread.get_id()) {
    // Registration from within the epoll thread.
    return handle_fd_registration(fd, handler, nullptr);
  }

  std::promise<bool> p;
  std::future<bool>  fut = p.get_future();

  enqueue_event(control_event{control_event::event_type::register_fd, fd, handler, &p});

  // Wait for the registration to complete.
  return fut.get();
}

/// \brief Remove fd from epoll handler.
bool io_broker_epoll::unregister_fd(int fd)
{
  if (fd < 0) {
    logger.error("io_broker_epoll::unregister_fd: Received an invalid fd={}", fd);
    return false;
  }
  if (not running.load(std::memory_order_relaxed)) {
    logger.warning("io_broker_epoll::unregister_fd: io_broker is not running. fd={}", fd);
    return false;
  }

  if (std::this_thread::get_id() == thread.get_id()) {
    // Deregistration from within the epoll thread. No need to go through the event queue.
    return handle_fd_deregistration(fd, nullptr);
  }

  std::promise<bool> p;
  std::future<bool>  fut = p.get_future();

  if (not enqueue_event(control_event{control_event::event_type::deregister_fd, fd, {}, &p})) {
    return false;
  }

  // Wait for the deregistration to complete.
  return fut.get();
}
