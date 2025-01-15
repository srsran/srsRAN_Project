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
