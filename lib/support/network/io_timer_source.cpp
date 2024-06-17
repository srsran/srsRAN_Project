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

#include "srsran/support/io/io_timer_source.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/error_handling.h"
#include "srsran/support/io/io_broker.h"
#include "srsran/support/timers.h"
#include <sys/timerfd.h>

using namespace srsran;

io_timer_source::io_timer_source(timer_manager& tick_sink_, io_broker& broker, std::chrono::milliseconds tick_period) :
  tick_sink(tick_sink_), logger(srslog::fetch_basic_logger("IO-EPOLL"))
{
  using namespace std::chrono;

  timer_fd = unique_fd{::timerfd_create(CLOCK_MONOTONIC, 0)};
  if (not timer_fd.is_open()) {
    report_fatal_error_if_not("Failed to create timer source (errno={})", strerror(errno));
  }

  auto              tsecs     = duration_cast<seconds>(tick_period);
  auto              tnsecs    = duration_cast<nanoseconds>(tick_period) - duration_cast<nanoseconds>(tsecs);
  struct timespec   period    = {tsecs.count(), tnsecs.count()};
  struct itimerspec timerspec = {period, period};
  ::timerfd_settime(timer_fd.value(), 0, &timerspec, nullptr);

  io_sub = broker.register_fd(
      timer_fd.value(), [this]() { read_time(); }, [this](io_broker::error_code ev) { io_sub.reset(); });
}

void io_timer_source::read_time()
{
  char read_buffer[8];
  int  n = ::read(timer_fd.value(), read_buffer, sizeof(read_buffer));
  if (n < 0) {
    logger.error("Failed to read timerfd (errno={})", ::strerror(errno));
    return;
  }
  if (n == 0) {
    logger.warning("Timerfd read returned 0");
  }

  // Tick timers.
  tick_sink.tick();
}
