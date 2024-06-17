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
#include "srsran/srslog/srslog.h"
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>

using namespace srsran;

// TODO: parameterize
const unsigned event_queue_size = 32;

io_broker_epoll::io_broker_epoll(const io_broker_config& config) :
  logger(srslog::fetch_basic_logger("IO-EPOLL")), event_queue(event_queue_size)
{
  // Init epoll socket
  epoll_fd = unique_fd{::epoll_create1(0)};
  if (not epoll_fd.is_open()) {
    report_fatal_error("IO broker: failed to create epoll file descriptor. error={}", strerror(errno));
  }

  // Register fd and event_handler to handle stops, fd registrations and fd deregistrations.
  ctrl_event_fd = unique_fd{::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK)};
  if (not ctrl_event_fd.is_open()) {
    report_fatal_error("IO broker: failed to create control event file descriptor. error={}", strerror(errno));
  }
  auto data_handler  = [this]() { handle_enqueued_events(); };
  auto error_handler = [this](error_code code) {
    logger.error("Error on control event file descriptor. Error code: {}", (int)code);
  };
  if (not handle_fd_registration(ctrl_event_fd.value(), data_handler, error_handler, nullptr)) {
    report_fatal_error("IO broker: failed to register control event file descriptor. ctrl_event_fd={}",
                       ctrl_event_fd.value());
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
  if (not ctrl_event_fd.close()) {
    logger.error("Failed to close control event socket: {}", strerror(errno));
  }

  // Close epoll socket.
  if (not epoll_fd.close()) {
    logger.error("Failed to close io epoll broker file descriptor: {}", strerror(errno));
  }

  logger.info("Closed io_broker");
}

// Function is executed in a loop until the destructor is called.
void io_broker_epoll::thread_loop()
{
  logger.debug("io_broker thread started...");
  while (running) {
    // wait for event
    const int32_t      epoll_timeout_ms   = -1;
    const uint32_t     MAX_EVENTS         = 1;
    struct epoll_event events[MAX_EVENTS] = {};
    int                nof_events         = ::epoll_wait(epoll_fd.value(), events, MAX_EVENTS, epoll_timeout_ms);

    // handle event
    if (nof_events == -1) {
      // Note: "Interrupted system call" can happen while debugging.
      logger.error("epoll_wait(): {}", strerror(errno));
      continue;
    }
    if (nof_events == 0) {
      logger.error("epoll time out {} sec expired", epoll_timeout_ms / 1000.0);
      continue;
    }

    for (int i = 0; i < nof_events; ++i) {
      int      fd           = events[i].data.fd;
      uint32_t epoll_events = events[i].events;
      if ((epoll_events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) || (!(epoll_events & EPOLLIN))) {
        error_code code = io_broker::error_code::other;
        // An error or hang up happened on this file descriptor, or the socket is not ready for reading
        if (epoll_events & (EPOLLHUP | EPOLLRDHUP)) {
          // Note: some container environments hang up stdin (fd=0) in case of non-interactive sessions
          logger.warning("fd={}: Hang up on file descriptor. Events: {:#x}", fd, epoll_events);
          code = io_broker::error_code::hang_up;
        } else if (epoll_events & EPOLLERR) {
          logger.error("fd={}: Error on file descriptor. Events={:#x}", fd, epoll_events);
          code = io_broker::error_code::error;
        } else {
          logger.error("fd={}: Unhandled epoll event. Events={:#x}", fd, epoll_events);
        }

        // Deregister the faulty file descriptor from epoll
        handle_fd_epoll_removal(fd, false, code, nullptr);
        continue;
      }

      const auto& it = event_handler.find(fd);
      if (it != event_handler.end() and it->second.registed_in_epoll()) {
        it->second.read_callback();
      } else {
        logger.error("fd={}: Ignoring event. Cause: File descriptor handler not found", fd);
      }
    }
  }
  logger.debug("io_broker thread stopped");
}

bool io_broker_epoll::enqueue_event(const control_event& event)
{
  // Push of an event
  event_queue.push_blocking(event);

  // trigger epoll event to interrupt possible epoll_wait()
  uint64_t tmp = 1;
  ssize_t  ret = ::write(ctrl_event_fd.value(), &tmp, sizeof(tmp));
  if (ret == -1) {
    logger.error("Error notifying io CTRL event_fd");
  }
  return ret >= 0;
}

void io_broker_epoll::handle_enqueued_events()
{
  // Read from the FD to avoid keep triggering the epoll.
  uint64_t ignore_counter;
  int      ignore = ::read(ctrl_event_fd.value(), &ignore_counter, sizeof(ignore_counter));
  (void)ignore;

  // Keep popping from the event queue.
  control_event ev;
  while (event_queue.try_pop(ev)) {
    // Handle event dequeued.
    switch (ev.type) {
      case control_event::event_type::register_fd:
        // Register new fd and event handler.
        handle_fd_registration(ev.fd, ev.handler, ev.err_handler, ev.completed);
        break;
      case control_event::event_type::deregister_fd:
        // Deregister fd and event handler.
        handle_fd_epoll_removal(ev.fd, true, std::nullopt, ev.completed);
        break;
      case control_event::event_type::close_io_broker:
        // Close io broker.
        stop_impl();
        return;
      default:
        report_fatal_error("Unknown event type {}", (int)ev.type);
    }
  }
}

bool io_broker_epoll::handle_fd_registration(int                     fd,
                                             const recv_callback_t&  handler,
                                             const error_callback_t& err_handler,
                                             std::promise<bool>*     complete_notifier)
{
  if (event_handler.count(fd) > 0) {
    logger.error("fd={}: Failed to register file descriptor. Cause: File descriptor already registered", fd);
    if (complete_notifier != nullptr) {
      complete_notifier->set_value(false);
    }
    return false;
  }

  // Add fd to epoll handler.
  struct epoll_event epoll_ev = {};
  epoll_ev.data.fd            = fd;
  epoll_ev.events             = EPOLLIN;
  if (::epoll_ctl(epoll_fd.value(), EPOLL_CTL_ADD, fd, &epoll_ev) == -1) {
    logger.error("fd={}: Failed to register file descriptor. Cause: epoll_ctl failed with \"{}\"", fd, strerror(errno));
    if (complete_notifier != nullptr) {
      complete_notifier->set_value(false);
    }
    return false;
  }

  // Register the handler of the fd.
  event_handler.insert(std::make_pair(fd, fd_handler{handler, err_handler}));
  if (complete_notifier != nullptr) {
    complete_notifier->set_value(true);
  }
  return true;
}

bool io_broker_epoll::handle_fd_epoll_removal(int                       fd,
                                              bool                      io_broker_deregistration_required,
                                              std::optional<error_code> epoll_error,
                                              std::promise<bool>*       complete_notifier)
{
  // The file descriptor must be already registered.
  auto ev_it = event_handler.find(fd);
  if (ev_it == event_handler.end()) {
    // File descriptor not found.
    // Note: It could have been automatically deregistered by the io broker.
    logger.error("fd={}: Failed to deregister file descriptor. Cause: File descriptor not found", fd);
    if (complete_notifier != nullptr) {
      complete_notifier->set_value(false);
    }
    return false;
  }

  // Regardless of the cause for the call, we always remove the FD from the epoll, so epoll errors won't spam the
  // logger, and so that we do not remove the same FD from the epoll more than once.
  if (ev_it->second.registed_in_epoll()) {
    // In case the cause for the FD removal was an epoll error, forward the error to the event handler.
    // Note: We avoid calling the error handling callback in case the FD removal was due to the subscriber
    // close/destruction or due to the io_broker being destroyed.
    if (epoll_error.has_value()) {
      ev_it->second.error_callback(*epoll_error);
    }

    // Remove FD from the epoll.
    struct epoll_event epoll_ev = {};
    epoll_ev.data.fd            = fd;
    epoll_ev.events             = EPOLLIN;
    if (::epoll_ctl(epoll_fd.value(), EPOLL_CTL_DEL, fd, &epoll_ev) == -1) {
      logger.error(
          "fd={}: Failed to deregister file descriptor. Cause: epoll_ctl failed with \"{}\"", fd, strerror(errno));
      event_handler.erase(ev_it);
      if (complete_notifier != nullptr) {
        complete_notifier->set_value(false);
      }
      return false;
    }

    // Mark the FD as not registered in the epoll.
    // Note: We keep the FD in the lookup table, so we avoid that the frontend points to inexistent FD.
    ev_it->second.read_callback  = {};
    ev_it->second.error_callback = {};
    logger.debug("fd={}: File descriptor deregistered from epoll interest list", fd);
  }

  if (io_broker_deregistration_required) {
    // In case it was the frontend requesting the deregistration of the FD (e.g. via subscriber destruction), remove
    // FD from the lookup as well. This will allow the same FD to be available to be re-registered later.
    event_handler.erase(ev_it);
  }

  // Notify completion of asynchronous task.
  if (complete_notifier != nullptr) {
    complete_notifier->set_value(true);
  }
  return true;
}

/// Adds a new file descriptor to the epoll-handler. The call is thread-safe and new
/// file descriptors can be added while the epoll_wait() is blocking.
io_broker::subscriber io_broker_epoll::register_fd(int fd, recv_callback_t handler, error_callback_t err_handler)
{
  if (fd < 0) {
    logger.error("File descriptor registration failed. Cause: Invalid file descriptor value fd={}", fd);
    return subscriber{};
  }
  if (not running.load(std::memory_order_relaxed)) {
    logger.warning("fd={}: Registration failed. Cause: io_broker is not running", fd);
    return subscriber{};
  }

  if (std::this_thread::get_id() == thread.get_id()) {
    // Registration from within the epoll thread.
    if (handle_fd_registration(fd, handler, err_handler, nullptr)) {
      return subscriber{*this, fd};
    }
    return subscriber{};
  }

  std::promise<bool> p;
  std::future<bool>  fut = p.get_future();

  enqueue_event(control_event{control_event::event_type::register_fd, fd, handler, err_handler, &p});

  // Wait for the registration to complete.
  if (fut.get()) {
    logger.info("fd={}: Registered file descriptor successfully", fd);
    return subscriber{*this, fd};
  }
  return subscriber{};
}

/// \brief Remove fd from epoll handler.
bool io_broker_epoll::unregister_fd(int fd)
{
  if (fd < 0) {
    logger.error("fd={}: File descriptor deregistration failed. Cause: Invalid file descriptor value", fd);
    return false;
  }
  if (not running.load(std::memory_order_relaxed)) {
    logger.warning("fd={}: Deregistration failed. Cause: io_broker is not running", fd);
    return false;
  }
  logger.debug("fd={}: Deregistering file descriptor...", fd);

  if (std::this_thread::get_id() == thread.get_id()) {
    // Deregistration from within the epoll thread. No need to go through the event queue.
    return handle_fd_epoll_removal(fd, true, std::nullopt, nullptr);
  }

  std::promise<bool> p;
  std::future<bool>  fut = p.get_future();

  if (not enqueue_event(control_event{control_event::event_type::deregister_fd, fd, {}, {}, &p})) {
    return false;
  }

  // Wait for the deregistration to complete.
  if (fut.get()) {
    logger.info("fd={}: Deregistered file descriptor successfully", fd);
    return true;
  }
  return false;
}

void io_broker_epoll::stop_impl()
{
  // Set flag to stop thread loop.
  running = false;

  // Start by deregistering all existing file descriptors.
  for (const auto& it : event_handler) {
    handle_fd_epoll_removal(it.first, false, std::nullopt, nullptr);
  }

  // Clear event queue.
  event_queue.clear();

  event_handler.erase(ctrl_event_fd.value());
  if (not event_handler.empty()) {
    logger.warning("File descriptors are still registered during io broker shutdown.");
  }
  event_handler.clear();
}
