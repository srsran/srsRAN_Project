/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "cameron314/concurrentqueue.h"
#include "epoll_helper.h"
#include "srsran/support/io/io_broker.h"
#include "srsran/support/io/unique_fd.h"
#include <future>
#include <unordered_map>

namespace srsran {

/// @brief Implementation of an IO broker using epoll.
class io_broker_epoll final : public io_broker
{
public:
  explicit io_broker_epoll(const io_broker_config& config);
  ~io_broker_epoll() override;

  SRSRAN_NODISCARD bool register_fd(int fd, recv_callback_t handler, error_callback_t err_handler) override;

  // Note: Blocking function.
  SRSRAN_NODISCARD bool unregister_fd(int fd) override;

private:
  struct control_event {
    enum class event_type { close_io_broker, register_fd, deregister_fd } type;
    int                 fd;
    recv_callback_t     handler;
    error_callback_t    err_handler;
    std::promise<bool>* completed;
  };

  void thread_loop();

  // Enqueues event to be asynchronously processed by the epoll thread.
  bool enqueue_event(const control_event& event);

  // Handle events stored in the ctrl event queue.
  void handle_enqueued_events();

  // Handle the registration of a new file descriptor.
  bool handle_fd_registration(int                     fd,
                              const recv_callback_t&  callback,
                              const error_callback_t& err_handler,
                              std::promise<bool>*     complete_notifier);

  // Handle the deregistration of an existing file descriptor.
  bool handle_fd_deregistration(int fd, std::promise<bool>* complete_notifier, bool is_error);

  srslog::basic_logger& logger;

  // Main epoll file descriptor
  unique_fd epoll_fd;
  // Event file descriptor used to interrupt epoll_wait call when a stop, fd registration, or fd deregistration is
  // requested.
  unique_fd ctrl_event_fd;

  // Lookup table mapping file descriptors to handlers.
  std::unordered_map<int, std::unique_ptr<epoll_handler>> event_handler;

  // Queue used to communicate commands to the epoll broker.
  moodycamel::ConcurrentQueue<control_event> event_queue;

  std::atomic<bool> running{true};
  unique_thread     thread;
};

} // namespace srsran
