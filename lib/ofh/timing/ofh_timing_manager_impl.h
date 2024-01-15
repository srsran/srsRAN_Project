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

#pragma once

#include "realtime_timing_worker.h"
#include "srsran/ofh/timing/ofh_timing_manager.h"

namespace srsran {
namespace ofh {

class timing_manager_impl : public timing_manager
{
  realtime_timing_worker worker;

public:
  timing_manager_impl(srslog::basic_logger& logger, task_executor& executor, const realtime_worker_cfg& config) :
    worker(logger, executor, config)
  {
  }

  // See interface for documentation.
  controller& get_controller() override;

  // See interface for documentation.
  ota_symbol_boundary_notifier_manager& get_ota_symbol_boundary_notifier_manager() override;
};

} // namespace ofh
} // namespace srsran
