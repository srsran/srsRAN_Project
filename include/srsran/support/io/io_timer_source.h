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

#pragma once

#include "srsran/support/io/io_broker.h"
#include "srsran/support/synchronization/sync_event.h"

namespace srsran {

class timer_manager;

/// \brief Interface for a timer source.
class io_timer_source
{
public:
  io_timer_source(timer_manager&            tick_sink_,
                  io_broker&                broker_,
                  task_executor&            executor,
                  std::chrono::milliseconds tick_period,
                  bool                      auto_start = true);

  /// This call blocks until the last tick is processed.
  ~io_timer_source();

  /// Resume ticking in case it was previously halted.
  void resume();

  /// \brief Request the timer source to stop ticking.
  /// Note: This call does not block, so a tick might take place after this call.
  void request_stop();

private:
  void create_subscriber(scoped_sync_token token);
  void destroy_subscriber();

  void read_time(int raw_fd, scoped_sync_token& token);

  const std::chrono::milliseconds tick_period;
  timer_manager&                  tick_sink;
  io_broker&                      broker;
  task_executor&                  tick_exec;
  srslog::basic_logger&           logger;
  io_broker::subscriber           io_sub;

  // Current state of the timer source.
  std::atomic<bool> running{false};

  // Synchronization primitive to stop the timer source.
  sync_event stop_flag;
};

} // namespace srsran
