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

#include "srsran/adt/span.h"
#include "srsran/du/du_high/du_high_executor_mapper.h"
#include "srsran/support/executors/unique_thread.h"

namespace srsran {

class timer_manager;

namespace test_helpers {

class du_high_worker_manager
{
public:
  virtual ~du_high_worker_manager() = default;

  virtual void stop() = 0;

  virtual srs_du::du_high_executor_mapper& get_exec_mapper() = 0;

  virtual task_executor& timer_executor() = 0;

  virtual void wait_pending_tasks() = 0;
};

struct du_high_worker_config {
  unsigned             nof_cell_workers   = 1;
  bool                 use_os_thread_prio = false;
  span<const unsigned> du_cell_cores;
  timer_manager&       timers;
};

/// Create DU-high executor mapper.
std::unique_ptr<du_high_worker_manager> create_multi_threaded_du_high_executor_mapper(const du_high_worker_config& cfg);

} // namespace test_helpers
} // namespace srsran
