/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/support/io/io_timer_source.h"
#include "srsran/support/error_handling.h"
#include "srsran/support/io/io_broker.h"
#include "srsran/support/timers.h"
#include <sys/timerfd.h>

using namespace srsran;

io_timer_source::io_timer_source(timer_manager&            tick_sink_,
                                 io_broker&                broker_,
                                 std::chrono::milliseconds tick_period_) :
  tick_sink(tick_sink_), broker(broker_), tick_period(tick_period_), logger(srslog::fetch_basic_logger("IO-EPOLL"))
{
  timer_fd = unique_fd{timerfd_create(CLOCK_REALTIME, 0)};
  if (not timer_fd.is_open()) {
    report_fatal_error_if_not("Failed to create timer source (errno={})", strerror(errno));
  }

  struct itimerspec timerspec = {{0, 0},
                                 {std::chrono::duration_cast<std::chrono::seconds>(tick_period).count(),
                                  std::chrono::duration_cast<std::chrono::nanoseconds>(tick_period).count()}};
  timerfd_settime(timer_fd.value(), 0, &timerspec, nullptr);

  io_sub = broker.register_fd(
      timer_fd.value(), [this]() { read_time(); }, [this](io_broker::error_code ev) { io_sub.reset(); });
}

void io_timer_source::read_time()
{
  char read_buffer[8];
  int  n = read(timer_fd.value(), read_buffer, sizeof(read_buffer));
  if (n < 0) {
    logger.error("Failed to read timerfd (errno={})", strerror(errno));
    return;
  }
  if (n == 0) {
    logger.warning("Timerfd read returned 0");
  }

  // Tick timers.
  tick_sink.tick();
}
