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
    create_subscriber();
  }
}

void io_timer_source::resume()
{
  // Cancel any pending stop command.
  pending_stop_cmd.store(false, std::memory_order_release);

  // Dispatch task to start ticking.
  while (not tick_exec.defer([this]() { create_subscriber(); })) {
    std::this_thread::sleep_for(std::chrono::milliseconds{1});
  }
}

void io_timer_source::request_stop()
{
  pending_stop_cmd.store(true, std::memory_order_release);
}

void io_timer_source::wait_for_stop()
{
  const std::chrono::milliseconds max_wait_time(500);

  request_stop();

  // Block waiting for the last read_time() to finish.
  for (std::chrono::milliseconds elapsed_time(0); io_sub.registered() and elapsed_time < max_wait_time;
       elapsed_time += tick_period) {
    std::this_thread::sleep_for(tick_period);
  }
  if (io_sub.registered()) {
    logger.error("Timer source did not stop within {} ms. Forcing its shutdown...", max_wait_time.count());
  }
}

void io_timer_source::create_subscriber()
{
  // Note: Called inside the ticking executor, except for in the ctor.

  if (io_sub.registered()) {
    // Someone already created the subscriber.
    return;
  }
  if (pending_stop_cmd.exchange(false, std::memory_order_acq_rel)) {
    // If a stop command was pending, we should not create the subscriber.
    return;
  }
  logger.info("Starting IO timer ticking source...");
  io_sub = broker.register_fd(create_timer_fd(tick_period), tick_exec, [this]() { read_time(); });
  report_fatal_error_if_not(io_sub.value() > 0, "Failed to create timer source");
}

void io_timer_source::read_time()
{
  // Note: Called inside the ticking executor.

  if (pending_stop_cmd.exchange(false, std::memory_order_relaxed)) {
    // Stop the timer source.
    logger.info("Stopping timer source");
    io_sub.reset();
    return;
  }

  uint64_t nof_expirations = 0;
  int      n               = ::read(io_sub.value(), &nof_expirations, sizeof(nof_expirations));
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
