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
  const std::chrono::milliseconds max_wait_time(500);
  for (std::chrono::milliseconds elapsed_time(0); io_sub.registered() and elapsed_time < max_wait_time;
       elapsed_time += tick_period) {
    stop_requested.store(true, std::memory_order_relaxed);
    std::this_thread::sleep_for(tick_period);
  }
  if (io_sub.registered()) {
    logger.error("Timer source did not stop within {} ms. Forcing its shutdown...", max_wait_time.count());
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
