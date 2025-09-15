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
    create_subscriber();
  }
}

void io_timer_source::resume()
{
  update_state(true);
}

void io_timer_source::request_stop()
{
  update_state(false);
}

void io_timer_source::wait_for_stop()
{
  const std::chrono::milliseconds max_wait_time(500);

  request_stop();

  // Block waiting for the last read_time() to finish.
  for (std::chrono::milliseconds elapsed_time(0);
       job_count.load(std::memory_order_acquire) > 0 and elapsed_time < max_wait_time;
       elapsed_time += tick_period) {
    std::this_thread::sleep_for(tick_period);
  }
  if (job_count.load(std::memory_order_acquire) > 0) {
    logger.error("Timer source did not stop within {} ms. Forcing its shutdown...", max_wait_time.count());
  }
  logger.info("IO timer source stopped.");
}

void io_timer_source::create_subscriber()
{
  // Note: Called inside the ticking executor, except for in the ctor.

  if (io_sub.registered()) {
    // Already created.
    return;
  }

  logger.info("Starting IO timer ticking source...");
  auto fd     = create_timer_fd(tick_period);
  int  raw_fd = fd.value();
  io_sub      = broker.register_fd(std::move(fd), tick_exec, [this, raw_fd]() { read_time(raw_fd); });
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
}

void io_timer_source::update_state(bool start)
{
  if (running.exchange(start, std::memory_order_acq_rel) == start) {
    // No change detected.
    return;
  }

  bool token_acquired = job_count.fetch_add(1, std::memory_order_acq_rel) == 0;
  if (not token_acquired) {
    // There is already a pending command to be processed.
    return;
  }

  // Dispatch task to stop ticking.
  while (not tick_exec.defer([this]() { handle_state_update(true); })) {
    std::this_thread::sleep_for(std::chrono::milliseconds{1});
  }
}

bool io_timer_source::handle_state_update(bool defer_stop)
{
  // Check if there are new commands.
  uint32_t pending       = job_count.load(std::memory_order_acquire);
  bool     sub_destroyed = false;
  while (pending > 0) {
    if (running.load(std::memory_order_acquire)) {
      request_to_stop = false;
      create_subscriber();
    } else {
      if (defer_stop and io_sub.registered()) {
        // Note: If read_time() is currently running, we need to unsubscribe from within it to avoid a deadlock.
        request_to_stop = true;
        return false;
      }
      // Called from within read_time(). We can finally destroy the subscriber.
      request_to_stop = false;
      destroy_subscriber();
      sub_destroyed = true;
    }
    pending = job_count.fetch_sub(pending, std::memory_order_acq_rel) - pending;
  }
  return sub_destroyed;
}

void io_timer_source::read_time(int raw_fd)
{
  // Note: Called inside the ticking executor.

  if (request_to_stop) {
    if (handle_state_update(false)) {
      // Subscriber was destroyed. Return.
      // Note: Do not touch any variable here as the ~io_timer_source() might be running concurrently.
      return;
    }
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
