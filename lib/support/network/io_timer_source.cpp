/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/support/io/io_timer_source.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/error_handling.h"
#include "srsran/support/io/io_broker.h"
#include "srsran/support/timers.h"
#include <sys/timerfd.h>

using namespace srsran;

static unique_fd create_timer_fd(std::chrono::milliseconds tick_period)
{
  using namespace std::chrono;

  auto timer_fd = unique_fd{::timerfd_create(CLOCK_MONOTONIC, 0)};
  report_fatal_error_if_not(timer_fd.is_open(), "Failed to create timer source (errno={})", ::strerror(errno));

  auto         tsecs     = duration_cast<seconds>(tick_period);
  auto         tnsecs    = duration_cast<nanoseconds>(tick_period) - duration_cast<nanoseconds>(tsecs);
  ::timespec   period    = {tsecs.count(), tnsecs.count()};
  ::itimerspec timerspec = {period, period};
  ::timerfd_settime(timer_fd.value(), 0, &timerspec, nullptr);

  return timer_fd;
}

io_timer_source::io_timer_source(timer_manager&            tick_sink_,
                                 io_broker&                broker_,
                                 task_executor&            executor,
                                 std::chrono::milliseconds tick_period_,
                                 bool                      auto_start) :
  tick_period(tick_period_),
  tick_sink(tick_sink_),
  broker(broker_),
  tick_exec(executor),
  logger(srslog::fetch_basic_logger("IO-EPOLL"))
{
  if (auto_start) {
    cur_state.store(state_t::started, std::memory_order_relaxed);
    create_subscriber(stop_flag.get_token());
  }
}

io_timer_source::~io_timer_source()
{
  cur_state.store(state_t::stopped, std::memory_order_release);
  stop_flag.wait();
}

void io_timer_source::resume()
{
  const auto cmd  = state_t::started;
  auto       prev = cur_state.exchange(cmd, std::memory_order_acq_rel);
  if (prev == cmd) {
    // Already started. No need to dispatch task. Early exit.
    return;
  }

  // Dispatch task to start ticking.
  while (not tick_exec.defer([this, token = stop_flag.get_token()]() { create_subscriber(token); })) {
    // We cannot allow the command to be lost. Retry until we succeed.
    std::this_thread::sleep_for(std::chrono::milliseconds{1});
  }
}

void io_timer_source::request_stop()
{
  // Request in a non-blocking fashion, the stop of the timer.
  cur_state.store(state_t::stopped, std::memory_order_release);
}

void io_timer_source::create_subscriber(scoped_sync_token token)
{
  // Note: Called inside the ticking executor, except for in the ctor.

  if (io_sub.registered()) {
    // Already created.
    return;
  }

  if (cur_state.load(std::memory_order_acquire) != state_t::started) {
    // The state has changed in the meantime and the subscriber wasn't created.
    return;
  }

  logger.info("Starting IO timer ticking source...");
  auto fd         = create_timer_fd(tick_period);
  int  raw_fd     = fd.value();
  read_time_token = std::move(token);
  io_sub          = broker.register_fd(std::move(fd), tick_exec, [this, raw_fd]() { read_time(raw_fd); });
  report_fatal_error_if_not(io_sub.registered(), "Failed to create timer source");
}

void io_timer_source::destroy_subscriber()
{
  if (not io_sub.registered()) {
    // Already destroyed.
    return;
  }
  logger.info("Stopping IO timer ticking source...");
  io_sub.reset();
  logger.info("IO timer source stopped.");

  // Mark token as reset.
  read_time_token.reset();
}

void io_timer_source::read_time(int raw_fd)
{
  // Note: Called inside the ticking executor.

  if (cur_state.load(std::memory_order_acquire) != state_t::started) {
    // Destroy subscriber and signal the completion of the stop by resetting the token.
    destroy_subscriber();
    // Note: Do not touch any variable here as the ~io_timer_source() might be running concurrently.
    return;
  }

  uint64_t nof_expirations = 0;
  int      n               = ::read(raw_fd, &nof_expirations, sizeof(nof_expirations));
  if (n < 0) {
    logger.error("Failed to read timerfd (errno={})", ::strerror(errno));
    return;
  }
  if (n == 0) {
    logger.warning("Timerfd read returned 0");
    return;
  }

  while (nof_expirations-- > 0) {
    // Tick timers.
    tick_sink.tick();
  }
}
