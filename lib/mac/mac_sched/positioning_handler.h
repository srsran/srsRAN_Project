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

#include "srsran/mac/mac_cell_control_information_handler.h"
#include "srsran/mac/mac_cell_manager.h"
#include "srsran/srslog/logger.h"

namespace srsran {

class scheduler_positioning_handler;
class task_executor;

class positioning_handler
{
public:
  virtual ~positioning_handler() = default;

  /// Handle new positioning measurement request for a given SRS Configuration.
  virtual async_task<mac_cell_positioning_measurement_response>
  handle_positioning_measurement_request(const mac_cell_positioning_measurement_request& req) = 0;

  /// Handle SRS indication from lower layers.
  virtual void handle_srs_indication(const mac_srs_indication_message& msg) = 0;
};

std::unique_ptr<positioning_handler> create_positioning_handler(scheduler_positioning_handler& sched,
                                                                task_executor&                 ctrl_exec,
                                                                srslog::basic_logger&          logger);

} // namespace srsran
