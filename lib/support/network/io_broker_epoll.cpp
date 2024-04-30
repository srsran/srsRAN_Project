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
#include <sys/eventfd.h>
#include <unistd.h>

using namespace srsran;

io_broker_epoll::io_broker_epoll(const io_broker_config& config) : logger(srslog::fetch_basic_logger("IO-EPOLL"))
{
  // Init epoll socket
  epoll_fd = unique_fd{::epoll_create1(0)};
  if (not epoll_fd.is_open()) {
    report_fatal_error("IO broker: failed to create epoll file descriptor. error={}", strerror(errno));
  }

  // Register fd and event_handler to handle stops, fd registrations and fd deregistrations.
  ctrl_event_fd = unique_fd{::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK)};
  if (not handle_fd_registration(
          ctrl_event_fd.value(), [this]() { handle_enqueued_events(); }, []() {}, nullptr)) {
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
}

// Function is executed in a loop until the destructor is called.
void io_broker_epoll::thread_loop()
{
  while (running) {
    // wait for event
    const int32_t      epoll_timeout_ms   = -1;
    const uint32_t     MAX_EVENTS         = 1;
    struct epoll_event events[MAX_EVENTS] = {};
    int                nof_events         = ::epoll_wait(epoll_fd.value(), events, MAX_EVENTS, epoll_timeout_ms);

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
      int      fd           = events[i].data.fd;
      uint32_t epoll_events = events[i].events;
      if ((epoll_events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) || (!(epoll_events & EPOLLIN))) {
        // An error or hang up happened on this file descriptor, or the socket is not ready for reading
        if (epoll_events & EPOLLHUP) {
          // Note: some container environments hang up stdin (fd=0) in case of non-interactive sessions
          logger.warning("fd={}: Hang up on file descriptor. Events: {:#x}", fd, epoll_events);
        } else if (epoll_events & EPOLLERR) {
          logger.error("fd={}: Error on file descriptor. Events={:#x}", fd, epoll_events);
        } else {
          logger.error("fd={}: Unhandled epoll event. Events={:#x}", fd, epoll_events);
        }

        // Deregister the faulty file descriptor from epoll
        handle_fd_deregistration(fd, nullptr, cause_t::epoll_error);
        break;
      }

      const auto& it = event_handler.find(fd);
      if (it != event_handler.end() and it->second.registered()) {
        it->second.read_callback();
      } else {
        logger.error("fd={}: Ignoring event. Cause: File descriptor handler not found", fd);
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
  ssize_t  ret = ::write(ctrl_event_fd.value(), &tmp, sizeof(tmp));
  if (ret == -1) {
    logger.error("Error notifying io CTRL event_fd");
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
        handle_fd_registration(ev.fd, ev.handler, ev.err_handler, ev.completed);
        break;
      case control_event::event_type::deregister_fd:
        // Deregister fd and event handler.
        handle_fd_deregistration(ev.fd, ev.completed, cause_t::frontend_request);
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
    logger.error("fd={}: Failed to register file descriptor. Cause: epoll_ctl failed", fd);
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

bool io_broker_epoll::handle_fd_deregistration(int fd, std::promise<bool>* complete_notifier, cause_t cause)
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

  // Regardless of the cause for the deregistration, we always remove the FD from the epoll.
  // Note: If !ev_it->second.registered(), it means that the FD has already been removed from the epoll.
  if (ev_it->second.registered()) {
    struct epoll_event epoll_ev = {};
    epoll_ev.data.fd            = fd;
    epoll_ev.events             = EPOLLIN;
    if (::epoll_ctl(epoll_fd.value(), EPOLL_CTL_DEL, fd, &epoll_ev) == -1) {
      logger.error("fd={}: Failed to deregister file descriptor. Cause: epoll_ctl failed", fd);
      event_handler.erase(ev_it);
      if (complete_notifier != nullptr) {
        complete_notifier->set_value(false);
      }
      return false;
    }

    // In case the cause for the deregistration was an epoll error, forward the error to the event handler.
    // Note: We avoid calling the error handling callback in case the deregistration was due to the subscriber
    // close/destruction or due to the io_broker being destroyed.
    if (cause == cause_t::epoll_error) {
      ev_it->second.error_callback();
    }
  }

  if (cause == cause_t::frontend_request) {
    // In case it was the frontend requesting the deregistration of the FD (e.g. via subscriber destruction), remove
    // FD from the lookup as well. This will allow the same FD to be available to be re-registered later.
    event_handler.erase(ev_it);
  } else {
    // In case it was the io_broker deregistering the FD, keep the FD in the lookup table, so we avoid that the
    // frontend points to inexistent FD. However, we deregister the event handlers so that the FD doesn't get removed
    // from the epoll more than once.
    ev_it->second.read_callback  = {};
    ev_it->second.error_callback = {};
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
    return subscriber{*this, fd};
  }
  return subscriber{};
}

/// \brief Remove fd from epoll handler.
bool io_broker_epoll::unregister_fd(int fd)
{
  if (fd < 0) {
    logger.error("File descriptor deregistration failed. Cause: Invalid file descriptor value fd={}", fd);
    return false;
  }
  if (not running.load(std::memory_order_relaxed)) {
    logger.warning("fd={}: Deregistration failed. Cause: io_broker is not running", fd);
    return false;
  }

  if (std::this_thread::get_id() == thread.get_id()) {
    // Deregistration from within the epoll thread. No need to go through the event queue.
    return handle_fd_deregistration(fd, nullptr, cause_t::frontend_request);
  }

  std::promise<bool> p;
  std::future<bool>  fut = p.get_future();

  if (not enqueue_event(control_event{control_event::event_type::deregister_fd, fd, {}, {}, &p})) {
    return false;
  }

  // Wait for the deregistration to complete.
  return fut.get();
}

void io_broker_epoll::stop_impl()
{
  // Set flag to stop thread loop.
  running = false;

  // Start by deregistering all existing file descriptors.
  for (const auto& it : event_handler) {
    handle_fd_deregistration(it.first, nullptr, cause_t::epoll_stop);
  }

  // Clear event queue.
  control_event ev;
  while (event_queue.try_dequeue(ev)) {
    // Do nothing.
  }

  event_handler.erase(ctrl_event_fd.value());
  if (not event_handler.empty()) {
    logger.warning("File descriptors are still registered during io broker shutdown.");
  }
  event_handler.clear();
}
