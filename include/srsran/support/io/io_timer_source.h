/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
  timer_manager&        tick_sink;
  srslog::basic_logger& logger;
  io_broker::subscriber io_sub;

public:
  io_timer_source(timer_manager&            tick_sink_,
                  io_broker&                broker_,
                  task_executor&            executor,
                  std::chrono::milliseconds tick_period);

private:
  void read_time();
};

} // namespace srsran
