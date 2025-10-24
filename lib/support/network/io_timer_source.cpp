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
    running.store(true, std::memory_order_relaxed);
    create_subscriber(stop_flag.get_token());
  }
}

io_timer_source::~io_timer_source()
{
  running.store(false, std::memory_order_release);
  // The dtor of stop_flag will block until all tasks using the token have completed.
}

void io_timer_source::resume()
{
  auto prev = running.exchange(true, std::memory_order_acq_rel);
  if (prev) {
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
  running.store(false, std::memory_order_release);
}

void io_timer_source::create_subscriber(scoped_sync_token token)
{
  // Note: Called inside the ticking executor, except for in the ctor.

  if (io_sub.registered()) {
    // Already created.
    return;
  }

  if (not running.load(std::memory_order_acquire)) {
    // The state has changed in the meantime and the subscriber wasn't created.
    return;
  }

  logger.info("Starting IO timer ticking source...");
  auto      fd     = create_timer_fd(tick_period);
  const int raw_fd = fd.value();
  io_sub = broker.register_fd(std::move(fd), tick_exec, [this, raw_fd, stop_token = std::move(token)]() mutable {
    read_time(raw_fd, stop_token);
  });
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
}

void io_timer_source::read_time(int raw_fd, scoped_sync_token& stop_token)
{
  // Note: Called inside the ticking executor.

  if (not running.load(std::memory_order_acquire)) {
    // Destroy subscriber and signal the completion of the stop by resetting the token.
    destroy_subscriber();
    stop_token.reset();
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
