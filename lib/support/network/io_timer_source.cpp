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

io_timer_source::io_timer_source(timer_manager&            tick_sink_,
                                 io_broker&                broker,
                                 task_executor&            executor,
                                 std::chrono::milliseconds tick_period_) :
  tick_period(tick_period_), tick_sink(tick_sink_), logger(srslog::fetch_basic_logger("IO-EPOLL"))
{
  using namespace std::chrono;

  auto timer_fd = unique_fd{::timerfd_create(CLOCK_MONOTONIC, 0)};
  report_fatal_error_if_not(timer_fd.is_open(), "Failed to create timer source (errno={})", ::strerror(errno));

  auto         tsecs     = duration_cast<seconds>(tick_period);
  auto         tnsecs    = duration_cast<nanoseconds>(tick_period) - duration_cast<nanoseconds>(tsecs);
  ::timespec   period    = {tsecs.count(), tnsecs.count()};
  ::itimerspec timerspec = {period, period};
  ::timerfd_settime(timer_fd.value(), 0, &timerspec, nullptr);

  io_sub = broker.register_fd(std::move(timer_fd), executor, [this]() { read_time(); });
  report_fatal_error_if_not(io_sub.value() > 0, "Failed to create timer source");
}

io_timer_source::~io_timer_source()
{
  while (io_sub.registered()) {
    stop_requested.store(true, std::memory_order_relaxed);
    std::this_thread::sleep_for(tick_period);
  }
}

void io_timer_source::read_time()
{
  if (stop_requested.load(std::memory_order_relaxed)) {
    // Request to stop the timer source.
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
