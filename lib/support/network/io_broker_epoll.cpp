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

#include "io_broker_epoll.h"
#include "srsran/srslog/srslog.h"
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>

using namespace srsran;

// TODO: parameterize
static constexpr unsigned event_queue_size = 32;

/// Tracks the FD that is currently being read in the receive callback.
thread_local int fd_read_in_callback = -1;

/// Sentinel value used by \c fd_read_in_callback to signal that the FD should not be rearmed.
static constexpr int AVOID_FD_REARMING = -2;

io_broker_epoll::io_broker_epoll(const io_broker_config& config) :
  logger(srslog::fetch_basic_logger("IO-EPOLL")), event_queue(event_queue_size)
{
  pending_fds_to_remove.reserve(16);

  // Init epoll socket
  epoll_fd = unique_fd{::epoll_create1(0)};
  if (not epoll_fd.is_open()) {
    report_fatal_error("IO broker: failed to create epoll file descriptor. error={}", ::strerror(errno));
  }

  // Register fd and event_handler to handle stops, fd registrations and fd deregistrations.
  auto ctrl_event_fd = unique_fd{::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK)};
  if (not ctrl_event_fd.is_open()) {
    report_fatal_error("IO broker: failed to create control event file descriptor. error={}", ::strerror(errno));
  }
  auto data_handler  = [this]() { handle_enqueued_events(); };
  auto error_handler = [this](error_code code) {
    logger.error("Error on control event file descriptor. Error code: {}", (int)code);
  };
  ctrl_event_raw_fd = ctrl_event_fd.value();
  if (not handle_fd_registration(std::move(ctrl_event_fd), data_handler, error_handler, nullptr, nullptr)) {
    report_fatal_error("IO broker: failed to register control event file descriptor. ctrl_event_fd={}",
                       ctrl_event_raw_fd);
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
    enqueue_event(control_event{control_event::event_type::close_io_broker, {}, -1, nullptr, {}, {}, nullptr});
    thread.join();
  }

  stop_impl();

  // Close epoll socket.
  if (not epoll_fd.close()) {
    logger.error("Failed to close io epoll broker file descriptor: {}", ::strerror(errno));
  }

  logger.info("Closed io_broker");
}

void io_broker_epoll::rearm_fd(int fd)
{
  ::epoll_event epoll_ev;
  epoll_ev.events  = EPOLLIN | EPOLLONESHOT;
  epoll_ev.data.fd = fd;

  if (::epoll_ctl(epoll_fd.value(), EPOLL_CTL_MOD, fd, &epoll_ev) == -1) {
    logger.error("Failed to rearm file descriptor: {}", fd);
  }
}

// Function is executed in a loop until the destructor is called.
void io_broker_epoll::thread_loop()
{
  logger.debug("io_broker thread started...");

  while (true) {
    // Process any pending file descriptor removals.
    for (auto it = pending_fds_to_remove.begin(); it != pending_fds_to_remove.end();) {
      if (auto event_it = event_handler.find(it->first); event_it != event_handler.end()) {
        if (not event_it->second.is_executing_recv_callback.load(std::memory_order_acquire)) {
          handle_fd_epoll_removal(it->first, true, std::nullopt, it->second);
          it = pending_fds_to_remove.erase(it);
          continue;
        }
      }
      ++it;
    }

    // wait for event
    const int32_t      epoll_timeout_ms   = -1;
    const uint32_t     MAX_EVENTS         = 8;
    struct epoll_event events[MAX_EVENTS] = {};
    int                nof_events         = ::epoll_wait(epoll_fd.value(), events, MAX_EVENTS, epoll_timeout_ms);

    // handle event
    if (nof_events == -1) {
      // Note: "Interrupted system call" can happen while debugging.
      if (errno != EINTR) {
        logger.error("epoll_wait(): {}", ::strerror(errno));
      }
      continue;
    }
    if (nof_events == 0) {
      logger.error("epoll time out {} sec expired", epoll_timeout_ms / 1000.0);
      continue;
    }

    for (int i = 0; i < nof_events; ++i) {
      int      fd           = events[i].data.fd;
      uint32_t epoll_events = events[i].events;

      if ((epoll_events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) or not(epoll_events & EPOLLIN)) {
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

      const auto it = event_handler.find(fd);
      if (it == event_handler.end() or not it->second.registed_in_epoll()) {
        logger.info("fd={}: Ignoring event. Cause: File descriptor handler not found", fd);
        continue;
      }

      if (fd == ctrl_event_raw_fd) {
        it->second.read_callback();
        // No rearming needed when the stop command has been executed by the callback.
        if (running.load(std::memory_order_relaxed)) {
          rearm_fd(fd);
        } else {
          logger.debug("io_broker thread stopped");
          return;
        }
        continue;
      }

      // Avoid enqueuing the callback if this FD is enqueued for deletion. It will be removed in the next loop
      // iteration.
      if (auto pending_it = std::find_if(pending_fds_to_remove.begin(),
                                         pending_fds_to_remove.end(),
                                         [fd](const auto& elem) { return fd == elem.first; });
          pending_it != pending_fds_to_remove.end()) {
        continue;
      }

      it->second.is_executing_recv_callback.store(true, std::memory_order_release);
      if (not it->second.executor->defer([this,
                                          fd,
                                          callback       = &it->second.read_callback,
                                          is_in_callback = &it->second.is_executing_recv_callback]() {
            // Track the current FD that is being read by this executor.
            fd_read_in_callback = fd;
            (*callback)();
            is_in_callback->store(false, std::memory_order_release);
            // Avoid rearming this FD if the callback unregistered it.
            if (fd_read_in_callback != AVOID_FD_REARMING) {
              rearm_fd(fd);
            }
            fd_read_in_callback = -1;
          })) {
        rearm_fd(fd);
        it->second.is_executing_recv_callback.store(false, std::memory_order_release);
        logger.error("Could not enqueue task for processing file descriptor: {}", fd);
      }
    }
  }
}

bool io_broker_epoll::enqueue_event(control_event&& event)
{
  // Push of an event
  event_queue.push_blocking(std::move(event));

  // trigger epoll event to interrupt possible epoll_wait()
  uint64_t tmp = 1;
  ssize_t  ret = ::write(ctrl_event_raw_fd, &tmp, sizeof(tmp));
  if (ret == -1) {
    logger.error("Error notifying io CTRL event_fd");
  }
  return ret >= 0;
}

void io_broker_epoll::handle_enqueued_events()
{
  // Read from the FD to avoid keep triggering the epoll.
  uint64_t ignore_counter;
  int      ignore = ::read(ctrl_event_raw_fd, &ignore_counter, sizeof(ignore_counter));
  (void)ignore;

  // Keep popping from the event queue.
  control_event ev;
  while (event_queue.try_pop(ev)) {
    // Handle event dequeued.
    switch (ev.type) {
      case control_event::event_type::register_fd:
        // Register new fd and event handler.
        handle_fd_registration(std::move(ev.fd), ev.handler, ev.err_handler, ev.executor, ev.completed);
        break;
      case control_event::event_type::deregister_fd:
        if (auto it = event_handler.find(ev.raw_fd); it != event_handler.end()) {
          // It is safe to directly deregister the FD if we are not reading from it.
          if (not it->second.is_executing_recv_callback.load(std::memory_order_acquire)) {
            handle_fd_epoll_removal(ev.raw_fd, false, std::nullopt, ev.completed);
            break;
          }
          // Enqueue fd deregistration.
          pending_fds_to_remove.emplace_back(ev.raw_fd, ev.completed);
          break;
        }
        // FD may have been already deregistered.
        ev.completed->set_value(false);
        break;
      case control_event::event_type::close_io_broker:
        // Set flag to stop thread loop.
        running.store(false, std::memory_order_release);
        return;
      default:
        report_fatal_error("Unknown event type {}", (int)ev.type);
    }
  }
}

bool io_broker_epoll::handle_fd_registration(unique_fd               fd,
                                             const recv_callback_t&  handler,
                                             const error_callback_t& err_handler,
                                             task_executor*          executor,
                                             std::promise<bool>*     complete_notifier)
{
  if (event_handler.count(fd.value()) > 0) {
    logger.error("fd={}: Failed to register file descriptor. Cause: File descriptor already registered", fd.value());
    if (complete_notifier != nullptr) {
      complete_notifier->set_value(false);
    }
    return false;
  }

  int raw_fd = fd.value();

  // Add fd to epoll handler.
  struct epoll_event epoll_ev = {};
  epoll_ev.data.fd            = raw_fd;
  epoll_ev.events             = EPOLLIN | EPOLLONESHOT;
  if (::epoll_ctl(epoll_fd.value(), EPOLL_CTL_ADD, raw_fd, &epoll_ev) == -1) {
    logger.error(
        "fd={}: Failed to register file descriptor. Cause: epoll_ctl failed with \"{}\"", raw_fd, ::strerror(errno));
    if (complete_notifier != nullptr) {
      complete_notifier->set_value(false);
    }
    return false;
  }

  // Register the handler of the fd.
  event_handler.emplace(std::piecewise_construct,
                        std::forward_as_tuple(raw_fd),
                        std::forward_as_tuple(executor, handler, err_handler, std::move(fd)));

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
    // Note: It could have been automatically deregistered by the io broker and the subscriber could trigger the removal
    // on its destruction.
    logger.error("fd={}: Failed to deregister file descriptor. Cause: File descriptor not found", fd);
    if (complete_notifier != nullptr) {
      complete_notifier->set_value(false);
    }
    return false;
  }

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
        "fd={}: Failed to deregister file descriptor. Cause: epoll_ctl failed with \"{}\"", fd, ::strerror(errno));
    event_handler.erase(ev_it);
    if (complete_notifier != nullptr) {
      complete_notifier->set_value(false);
    }
    return false;
  }

  logger.debug("fd={}: File descriptor deregistered from epoll interest list", fd);
  event_handler.erase(ev_it);

  // Notify completion of asynchronous task.
  if (complete_notifier != nullptr) {
    complete_notifier->set_value(true);
  }
  return true;
}

/// Adds a new file descriptor to the epoll-handler. The call is thread-safe and new
/// file descriptors can be added while the epoll_wait() is blocking.
io_broker::subscriber io_broker_epoll::register_fd(unique_fd        fd,
                                                   task_executor&   executor,
                                                   recv_callback_t  handler,
                                                   error_callback_t err_handler)
{
  if (not fd.is_open()) {
    logger.error("File descriptor registration failed. Cause: Invalid file descriptor value");
    return subscriber{};
  }

  if (not running.load(std::memory_order_acquire)) {
    logger.warning("fd={}: Registration failed. Cause: io_broker is not running", fd.value());
    return subscriber{};
  }

  int raw_fd = fd.value();

  if (std::this_thread::get_id() == thread.get_id()) {
    // Registration from within the epoll thread.
    if (handle_fd_registration(std::move(fd), handler, err_handler, nullptr, nullptr)) {
      return subscriber{*this, raw_fd};
    }
    return subscriber{};
  }

  report_error_if_not(fd_read_in_callback < 0, "Cannot register a new file descriptor inside the read callback");

  std::promise<bool> p;
  std::future<bool>  fut = p.get_future();

  enqueue_event(control_event{
      control_event::event_type::register_fd, std::move(fd), raw_fd, &executor, handler, err_handler, &p});

  // Wait for the registration to complete.
  if (fut.get()) {
    logger.info("fd={}: Registered file descriptor successfully", raw_fd);
    return subscriber{*this, raw_fd};
  }
  return subscriber{};
}

/// \brief Remove fd from epoll handler.
bool io_broker_epoll::unregister_fd(int fd, std::promise<bool>* complete_notifier)
{
  if (fd < 0) {
    logger.error("fd={}: File descriptor deregistration failed. Cause: Invalid file descriptor value", fd);
    if (complete_notifier) {
      complete_notifier->set_value(false);
    }
    return false;
  }
  if (not running.load(std::memory_order_acquire)) {
    logger.warning("fd={}: Deregistration failed. Cause: io_broker is not running", fd);
    if (complete_notifier) {
      complete_notifier->set_value(false);
    }
    return false;
  }
  logger.debug("fd={}: Deregistering file descriptor...", fd);

  report_error_if_not(not(fd_read_in_callback >= 0 and fd_read_in_callback != fd),
                      "Cannot deregister an unrelated file descriptor inside the read callback");

  // Handle the case of calling unregister from the read callback.
  if (fd_read_in_callback == fd) {
    // No rearming is needed as the FD is going to be being removed and no more callbacks should be called for it.
    fd_read_in_callback = AVOID_FD_REARMING;
    if (complete_notifier) {
      complete_notifier->set_value(true);
      complete_notifier = nullptr;
    }
  }

  if (not enqueue_event(
          control_event{control_event::event_type::deregister_fd, {}, fd, nullptr, {}, {}, complete_notifier})) {
    if (complete_notifier) {
      complete_notifier->set_value(false);
    }
    return false;
  }

  return true;
}

void io_broker_epoll::stop_impl()
{
  // Process any pending file descriptor removals.
  for (auto it = pending_fds_to_remove.begin(); it != pending_fds_to_remove.end();) {
    if (auto event_it = event_handler.find(it->first); event_it != event_handler.end()) {
      while (event_it->second.is_executing_recv_callback.load(std::memory_order_acquire)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
      }
      handle_fd_epoll_removal(it->first, true, std::nullopt, it->second);
      it = pending_fds_to_remove.erase(it);
      continue;
    }
    ++it;
  }

  // Check if there are any alive file descriptors apart from the control event file descriptor.
  if (event_handler.size() > 1) {
    logger.warning("File descriptors are still registered during io broker shutdown.");
  }

  // Deregistering all existing file descriptors.
  for (auto it = event_handler.begin(); it != event_handler.end();) {
    while (it->second.is_executing_recv_callback.load(std::memory_order_acquire)) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    struct epoll_event epoll_ev = {};
    epoll_ev.data.fd            = it->first;
    epoll_ev.events             = EPOLLIN;
    if (::epoll_ctl(epoll_fd.value(), EPOLL_CTL_DEL, it->first, &epoll_ev) == -1) {
      logger.error("fd={}: Failed to deregister file descriptor. Cause: epoll_ctl failed with \"{}\"",
                   it->first,
                   ::strerror(errno));
    }
    it = event_handler.erase(it);
  }

  // Clear event queue.
  event_queue.clear();
  // Clear event handler map.
  event_handler.clear();
}
