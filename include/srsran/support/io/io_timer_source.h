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

#include "srsran/support/io/io_broker.h"
#include "srsran/support/io/unique_fd.h"
#include "srsran/support/timers.h"

namespace srsran {

/// \brief Interface for a timer source.
class io_timer_source
{
public:
  io_timer_source(timer_manager&            tick_sink_,
                  io_broker&                broker_,
                  std::chrono::milliseconds tick_period = std::chrono::milliseconds{1});

private:
  void read_time();

  timer_manager&            tick_sink;
  io_broker&                broker;
  std::chrono::milliseconds tick_period;
  srslog::basic_logger&     logger;

  unique_fd timer_fd;

  io_broker::subscriber io_sub;
};

} // namespace srsran